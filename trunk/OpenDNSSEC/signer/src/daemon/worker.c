/*
 * $Id$
 *
 * Copyright (c) 2009 NLNet Labs. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * The hard workers.
 *
 */

#include "adapter/adapi.h"
#include "daemon/engine.h"
#include "daemon/worker.h"
#include "shared/allocator.h"
#include "scheduler/schedule.h"
#include "shared/locks.h"
#include "shared/log.h"
#include "shared/status.h"
#include "shared/util.h"
#include "signer/tools.h"
#include "signer/zone.h"

#include <time.h> /* time() */

ods_lookup_table worker_str[] = {
    { WORKER_WORKER, "worker" },
    { WORKER_DRUDGER, "drudger" },
    { 0, NULL }
};


/**
 * Convert worker type to string.
 *
 */
static const char*
worker2str(worker_id type)
{
    ods_lookup_table *lt = ods_lookup_by_id(worker_str, type);
    if (lt) {
        return lt->name;
    }
    return NULL;
}


/**
 * Create worker.
 *
 */
worker_type*
worker_create(allocator_type* allocator, int num, worker_id type)
{
    worker_type* worker;
    if (!allocator) {
        return NULL;
    }
    worker = (worker_type*) allocator_alloc(allocator, sizeof(worker_type));
    if (!worker) {
        return NULL;
    }
    ods_log_debug("[%s[%i]] create", worker2str(type), num+1);
    lock_basic_init(&worker->worker_lock);
    lock_basic_set(&worker->worker_alarm);
    lock_basic_lock(&worker->worker_lock);
    worker->allocator = allocator;
    worker->thread_num = num +1;
    worker->engine = NULL;
    worker->task = NULL;
    worker->working_with = TASK_NONE;
    worker->need_to_exit = 0;
    worker->type = type;
    worker->clock_in = 0;
    worker->jobs_appointed = 0;
    worker->jobs_completed = 0;
    worker->jobs_failed = 0;
    worker->sleeping = 0;
    worker->waiting = 0;
    lock_basic_unlock(&worker->worker_lock);
    return worker;
}


/**
 * Worker working with...
 *
 */
static void
worker_working_with(worker_type* worker, task_id with, task_id next,
    const char* str, const char* name, task_id* what, time_t* when)
{
    worker->working_with = with;
    ods_log_verbose("[%s[%i]] %s zone %s", worker2str(worker->type),
       worker->thread_num, str, name);
    *what = next;
    *when = time_now();
    return;
}


/**
 * Has this worker measured up to all appointed jobs?
 *
 */
static int
worker_fulfilled(worker_type* worker)
{
    return (worker->jobs_completed + worker->jobs_failed) ==
        worker->jobs_appointed;
}


/**
 * Perform task.
 *
 */
static void
worker_perform_task(worker_type* worker)
{
    engine_type* engine = NULL;
    zone_type* zone = NULL;
    task_type* task = NULL;
    task_id what = TASK_NONE;
    time_t when = 0;
    time_t never = (3600*24*365);
    ods_status status = ODS_STATUS_OK;
    int fallthrough = 0;
    int backup = 0;
    uint32_t tmpserial = 0;
    time_t start = 0;
    time_t end = 0;

    if (!worker || !worker->task || !worker->task->zone || !worker->engine) {
        return;
    }
    engine = (engine_type*) worker->engine;
    task = (task_type*) worker->task;
    zone = (zone_type*) worker->task->zone;
    ods_log_debug("[%s[%i]] perform task %s for zone %s at %u",
       worker2str(worker->type), worker->thread_num, task_what2str(task->what),
       task_who2str(task), (uint32_t) worker->clock_in);
    /* do what you have been told to do */
    switch (task->what) {
        case TASK_SIGNCONF:
            /* perform 'load signconf' task */
            worker_working_with(worker, TASK_SIGNCONF, TASK_READ,
                "configure", task_who2str(task), &what, &when);
            status = tools_signconf(zone);
            if (status == ODS_STATUS_UNCHANGED) {
                if (!zone->signconf->last_modified) {
                    ods_log_debug("[%s[%i]] no signconf.xml for zone %s yet",
                        worker2str(worker->type), worker->thread_num,
                        task_who2str(task));
                    status = ODS_STATUS_ERR;
                }
            }
            if (status == ODS_STATUS_UNCHANGED) {
                if (task->halted != TASK_NONE) {
                    goto task_perform_continue;
                }
                status = ODS_STATUS_OK;
            }

            if (status == ODS_STATUS_OK) {
                status = zone_publish_dnskeys(zone);
            }
            if (status == ODS_STATUS_OK) {
                status = zone_publish_nsec3param(zone, 0);
            }
            if (status == ODS_STATUS_OK) {
                status = zonedata_commit(zone->zonedata);
            }

            if (status == ODS_STATUS_OK) {
                zone->prepared = 1;
                task->interrupt = TASK_NONE;
                task->halted = TASK_NONE;
            } else {
                if (task->halted == TASK_NONE) {
                    goto task_perform_fail;
                }
                goto task_perform_continue;
            }
            fallthrough = 0;
            break;
        case TASK_READ:
            /* perform 'read input adapter' task */
            worker_working_with(worker, TASK_READ, TASK_SIGN,
                "read", task_who2str(task), &what, &when);
            if (!zone->prepared) {
                ods_log_debug("[%s[%i]] no valid signconf.xml for zone %s yet",
                    worker2str(worker->type), worker->thread_num,
                    task_who2str(task));
                status = ODS_STATUS_ERR;
            } else {
                status = tools_input(zone);
            }

            /* what to do next */
            what = TASK_NSECIFY;
            when = time_now();
            if (status != ODS_STATUS_OK) {
                if (task->halted == TASK_NONE) {
                    goto task_perform_fail;
                }
                goto task_perform_continue;
            }
            fallthrough = 1;
        case TASK_NSECIFY:
            worker->working_with = TASK_NSECIFY;
            ods_log_verbose("[%s[%i]] nsecify zone %s",
                worker2str(worker->type), worker->thread_num,
                task_who2str(task));
            status = tools_nsecify(zone);

            /* what to do next */
            what = TASK_SIGN;
            when = time_now();
            if (status == ODS_STATUS_OK) {
                if (task->interrupt > TASK_SIGNCONF) {
                    task->interrupt = TASK_NONE;
                    task->halted = TASK_NONE;
                }
            } else {
                if (task->halted == TASK_NONE) {
                    goto task_perform_fail;
                }
                goto task_perform_continue;
            }
            fallthrough = 1;
        case TASK_SIGN:
            /* perform 'sign' task */
            worker_working_with(worker, TASK_SIGN, TASK_WRITE,
                "sign", task_who2str(task), &what, &when);

            tmpserial = zone->zonedata->internal_serial;
            status = zone_update_serial(zone);
            if (status != ODS_STATUS_OK) {
                ods_log_error("[%s[%i]] unable to sign zone %s: "
                    "failed to increment serial",
                    worker2str(worker->type), worker->thread_num,
                    task_who2str(task));
            } else {
                /* start timer */
                start = time(NULL);
                if (zone->stats) {
                    lock_basic_lock(&zone->stats->stats_lock);
                    if (!zone->stats->start_time) {
                        zone->stats->start_time = start;
                    }
                    zone->stats->sig_count = 0;
                    zone->stats->sig_soa_count = 0;
                    zone->stats->sig_reuse = 0;
                    zone->stats->sig_time = 0;
                    lock_basic_unlock(&zone->stats->stats_lock);
                }

                /* queue menial, hard signing work */
                status = zonedata_queue(zone->zonedata, engine->signq, worker);
                ods_log_debug("[%s[%i]] wait until drudgers are finished "
                    " signing zone %s, %u signatures queued",
                    worker2str(worker->type), worker->thread_num,
                    task_who2str(task), worker->jobs_appointed);

                /* sleep until work is done */
                if (!worker->need_to_exit) {
                    worker_sleep_unless(worker, 0);
                }
                if (worker->jobs_failed) {
                    ods_log_error("[%s[%i]] sign zone %s failed: %u of %u "
                        "signatures failed", worker2str(worker->type),
                        worker->thread_num, task_who2str(task),
                        worker->jobs_failed, worker->jobs_appointed);
                    status = ODS_STATUS_ERR;
                } else if (!worker_fulfilled(worker)) {
                    ods_log_error("[%s[%i]] sign zone %s failed: %u of %u "
                        "signatures completed", worker2str(worker->type),
                        worker->thread_num, task_who2str(task),
                        worker->jobs_completed, worker->jobs_appointed);
                    status = ODS_STATUS_ERR;
                } else {
                    ods_log_debug("[%s[%i]] sign zone %s ok: %u of %u "
                       "signatures succeeded", worker2str(worker->type),
                        worker->thread_num, task_who2str(task),
                        worker->jobs_completed, worker->jobs_appointed);
                    ods_log_assert(worker->jobs_appointed ==
                        worker->jobs_completed);
                }
                worker->jobs_appointed = 0;
                worker->jobs_completed = 0;
                worker->jobs_failed = 0;

                /* stop timer */
                end = time(NULL);
                if (status == ODS_STATUS_OK && zone->stats) {
                    lock_basic_lock(&zone->stats->stats_lock);
                    zone->stats->sig_time = (end-start);
                    lock_basic_unlock(&zone->stats->stats_lock);
                }
            }

            /* what to do next */
            if (status != ODS_STATUS_OK) {
                if (task->halted == TASK_NONE) {
                    goto task_perform_fail;
                }
                goto task_perform_continue;
            }
            /* break; */
        case TASK_WRITE:
            /* perform 'write to output adapter' task */
            worker_working_with(worker, TASK_WRITE, TASK_SIGN,
                "write", task_who2str(task), &what, &when);
            status = tools_output(zone, engine->config->working_dir,
                engine->config->cfg_filename);
            zone->processed = 1;

            /* what to do next */
            if (status != ODS_STATUS_OK) {
                if (task->halted == TASK_NONE) {
                    goto task_perform_fail;
                }
                goto task_perform_continue;
            } else {
                if (task->interrupt > TASK_SIGNCONF) {
                    task->interrupt = TASK_NONE;
                    task->halted = TASK_NONE;
                }
            }
            if (duration2time(zone->signconf->sig_resign_interval)) {
                what = TASK_SIGN;
                when = time_now() +
                    duration2time(zone->signconf->sig_resign_interval);
            } else {
                what = TASK_NONE;
                when = time_now() + never;
            }
            backup = 1;
            fallthrough = 0;
            break;
        case TASK_NONE:
            worker->working_with = TASK_NONE;
            ods_log_warning("[%s[%i]] none task for zone %s",
                worker2str(worker->type), worker->thread_num,
                task_who2str(task));
            when = time_now() + never;
            fallthrough = 0;
            break;
        default:
            ods_log_warning("[%s[%i]] unknown task, trying full sign zone %s",
                worker2str(worker->type), worker->thread_num,
                task_who2str(task));
            what = TASK_SIGNCONF;
            when = time_now();
            fallthrough = 0;
            break;
    }

    /* no error, reset backoff */
    task->backoff = 0;

    /* set next task */
    if (fallthrough == 0 && task->interrupt != TASK_NONE &&
        task->interrupt != what) {
        ods_log_debug("[%s[%i]] interrupt task %s for zone %s",
            worker2str(worker->type), worker->thread_num,
            task_what2str(what), task_who2str(task));

        task->what = task->interrupt;
        task->when = time_now();
        task->halted = what;
    } else {
        ods_log_debug("[%s[%i]] next task %s for zone %s",
            worker2str(worker->type), worker->thread_num,
            task_what2str(what), task_who2str(task));

        task->what = what;
        task->when = when;
        if (!fallthrough) {
            task->interrupt = TASK_NONE;
            task->halted = TASK_NONE;
        }
    }
    /* backup the last successful run */
    if (backup) {
        status = zone_backup(zone);
        if (status != ODS_STATUS_OK) {
            ods_log_warning("[%s[%i]] unable to backup zone %s: %s",
            worker2str(worker->type), worker->thread_num,
            task_who2str(task), ods_status2str(status));
            /* just a warning */
            status = ODS_STATUS_OK;
        }
        backup = 0;
    }
    return;

task_perform_fail:
    /* in case of failure, also mark zone processed (for single run usage) */
    zone->processed = 1;

    if (task->backoff) {
        task->backoff *= 2;
        if (task->backoff > ODS_SE_MAX_BACKOFF) {
            task->backoff = ODS_SE_MAX_BACKOFF;
        }
    } else {
        task->backoff = 60;
    }
    ods_log_info("[%s[%i]] backoff task %s for zone %s with %u seconds",
        worker2str(worker->type), worker->thread_num,
        task_what2str(task->what), task_who2str(task), task->backoff);

    task->when = time_now() + task->backoff;
    return;

task_perform_continue:
    ods_log_info("[%s[%i]] continue task %s for zone %s",
        worker2str(worker->type), worker->thread_num,
        task_what2str(task->halted), task_who2str(task));

    what = task->halted;
    task->what = what;
    task->when = time_now();
    task->interrupt = TASK_NONE;
    task->halted = TASK_NONE;
    if (zone->processed) {
        task->when += duration2time(zone->signconf->sig_resign_interval);
    }
    return;
}


/**
 * Work.
 *
 */
static void
worker_work(worker_type* worker)
{
    time_t now, timeout = 1;
    engine_type* engine = NULL;
    zone_type* zone = NULL;
    ods_status status = ODS_STATUS_OK;

    ods_log_assert(worker);
    ods_log_assert(worker->type == WORKER_WORKER);

    engine = (engine_type*) worker->engine;
    while (worker->need_to_exit == 0) {
        ods_log_debug("[%s[%i]] report for duty", worker2str(worker->type),
            worker->thread_num);
        lock_basic_lock(&engine->taskq->schedule_lock);
        /* [LOCK] schedule */
        worker->task = schedule_pop_task(engine->taskq);
        /* [UNLOCK] schedule */
        if (worker->task) {
            worker->working_with = worker->task->what;
            lock_basic_unlock(&engine->taskq->schedule_lock);

            zone = worker->task->zone;
            lock_basic_lock(&zone->zone_lock);
            /* [LOCK] zone */
            ods_log_debug("[%s[%i]] start working on zone %s",
                worker2str(worker->type), worker->thread_num, zone->name);

            worker->clock_in = time(NULL);
            worker_perform_task(worker);

            zone->task = worker->task;

            ods_log_debug("[%s[%i]] finished working on zone %s",
                worker2str(worker->type), worker->thread_num, zone->name);
            /* [UNLOCK] zone */

            lock_basic_lock(&engine->taskq->schedule_lock);
            /* [LOCK] zone, schedule */
            worker->task = NULL;
            worker->working_with = TASK_NONE;
            status = schedule_task(engine->taskq, zone->task, 1);
            /* [UNLOCK] zone, schedule */
            lock_basic_unlock(&engine->taskq->schedule_lock);
            lock_basic_unlock(&zone->zone_lock);

            timeout = 1;
        } else {
            ods_log_debug("[%s[%i]] nothing to do", worker2str(worker->type),
                worker->thread_num);

            /* [LOCK] schedule */
            worker->task = schedule_get_first_task(engine->taskq);
            /* [UNLOCK] schedule */
            lock_basic_unlock(&engine->taskq->schedule_lock);

            now = time_now();
            if (worker->task && !engine->taskq->loading) {
                timeout = (worker->task->when - now);
            } else {
                timeout *= 2;
                if (timeout > ODS_SE_MAX_BACKOFF) {
                    timeout = ODS_SE_MAX_BACKOFF;
                }
            }
            worker->task = NULL;
            worker_sleep(worker, timeout);
        }
    }
    return;
}


/**
 * Drudge.
 *
 */
static void
worker_drudge(worker_type* worker)
{
    engine_type* engine = NULL;
    zone_type* zone = NULL;
    task_type* task = NULL;
    rrset_type* rrset = NULL;
    ods_status status = ODS_STATUS_OK;
    worker_type* chief = NULL;
    hsm_ctx_t* ctx = NULL;

    ods_log_assert(worker);
    ods_log_assert(worker->type == WORKER_DRUDGER);

    ctx = hsm_create_context();
    if (ctx == NULL) {
        ods_log_error("[%s[%i]] unable to drudge: error "
            "creating libhsm context", worker2str(worker->type),
            worker->thread_num);
    }
    engine = (engine_type*) worker->engine;
    while (worker->need_to_exit == 0) {
        ods_log_debug("[%s[%i]] report for duty", worker2str(worker->type),
            worker->thread_num);
        chief = NULL;
        zone = NULL;
        task = NULL;

        lock_basic_lock(&engine->signq->q_lock);
        /* [LOCK] schedule */
        rrset = (rrset_type*) fifoq_pop(engine->signq, &chief);
        /* [UNLOCK] schedule */
        lock_basic_unlock(&engine->signq->q_lock);
        if (rrset) {
            /* set up the work */
            if (chief) {
                task = chief->task;
            }
            if (task) {
                zone = task->zone;
            }
            if (!zone) {
                ods_log_error("[%s[%i]] unable to drudge: no zone reference",
                    worker2str(worker->type), worker->thread_num);
            }
            if (zone && ctx) {
                ods_log_assert(rrset);
                ods_log_assert(zone);
                ods_log_assert(zone->apex);
                ods_log_assert(zone->signconf);
                ods_log_assert(ctx);

                worker->clock_in = time(NULL);
                status = rrset_sign(ctx, rrset, zone->apex, zone->signconf,
                    chief->clock_in, zone->stats);
            } else {
                status = ODS_STATUS_ASSERT_ERR;
            }

            if (chief) {
                lock_basic_lock(&chief->worker_lock);
                if (status == ODS_STATUS_OK) {
                    chief->jobs_completed += 1;
                } else {
                    chief->jobs_failed += 1;
                    /* destroy context? */
                }
                lock_basic_unlock(&chief->worker_lock);

                if (worker_fulfilled(chief) && chief->sleeping) {
                    ods_log_debug("[%s[%i]] wake up chief[%u], work is done",
                        worker2str(worker->type), worker->thread_num,
                        chief->thread_num);
                    worker_wakeup(chief);
                    chief = NULL;
                }
            }
            rrset = NULL;
        } else {
            ods_log_debug("[%s[%i]] nothing to do", worker2str(worker->type),
                worker->thread_num);
            worker_wait(&engine->signq->q_lock,
                &engine->signq->q_threshold);
        }
    }
    /* wake up chief */
    if (chief && chief->sleeping) {
        ods_log_debug("[%s[%i]] wake up chief[%u], i am exiting",
            worker2str(worker->type), worker->thread_num, chief->thread_num);
         worker_wakeup(chief);
    }

    /* cleanup open HSM sessions */
    hsm_destroy_context(ctx);
    ctx = NULL;
    return;
}


/**
 * Start worker.
 *
 */
void
worker_start(worker_type* worker)
{
    ods_log_assert(worker);
    switch (worker->type) {
        case WORKER_DRUDGER:
            worker_drudge(worker);
            break;
        case WORKER_WORKER:
            worker_work(worker);
            break;
        default:
            ods_log_error("[worker] illegal worker (id=%i)", worker->type);
            return;
    }
    return;
}


/**
 * Put worker to sleep.
 *
 */
void
worker_sleep(worker_type* worker, time_t timeout)
{
    ods_log_assert(worker);
    lock_basic_lock(&worker->worker_lock);
    /* [LOCK] worker */
    worker->sleeping = 1;
    lock_basic_sleep(&worker->worker_alarm, &worker->worker_lock,
        timeout);
    /* [UNLOCK] worker */
    lock_basic_unlock(&worker->worker_lock);
    return;
}


/**
 * Put worker to sleep unless worker has measured up to all appointed jobs.
 *
 */
void
worker_sleep_unless(worker_type* worker, time_t timeout)
{
    ods_log_assert(worker);
    lock_basic_lock(&worker->worker_lock);
    /* [LOCK] worker */
    while (!worker->need_to_exit && !worker_fulfilled(worker)) {
        worker->sleeping = 1;
        lock_basic_sleep(&worker->worker_alarm, &worker->worker_lock,
            timeout);

        ods_log_debug("[%s[%i]] somebody poked me, check completed jobs %u "
           "appointed, %u completed, %u failed", worker2str(worker->type),
           worker->thread_num, worker->jobs_appointed, worker->jobs_completed,
           worker->jobs_failed);
    }
    /* [UNLOCK] worker */
    lock_basic_unlock(&worker->worker_lock);
    return;
}


/**
 * Wake up worker.
 *
 */
void
worker_wakeup(worker_type* worker)
{
    ods_log_assert(worker);
    if (worker && worker->sleeping && !worker->waiting) {
        ods_log_debug("[%s[%i]] wake up", worker2str(worker->type),
           worker->thread_num);
        lock_basic_lock(&worker->worker_lock);
        /* [LOCK] worker */
        lock_basic_alarm(&worker->worker_alarm);
        worker->sleeping = 0;
        /* [UNLOCK] worker */
        lock_basic_unlock(&worker->worker_lock);
    }
    return;
}


/**
 * Worker waiting.
 *
 */
void
worker_wait(lock_basic_type* lock, cond_basic_type* condition)
{
    lock_basic_lock(lock);
    /* [LOCK] worker */
    lock_basic_sleep(condition, lock, 0);
    /* [UNLOCK] worker */
    lock_basic_unlock(lock);
    return;
}


/**
 * Notify a worker.
 *
 */
void
worker_notify(lock_basic_type* lock, cond_basic_type* condition)
{
    lock_basic_lock(lock);
    /* [LOCK] lock */
    lock_basic_alarm(condition);
    /* [UNLOCK] lock */
    lock_basic_unlock(lock);
    return;
}


/**
 * Notify all workers.
 *
 */
void
worker_notify_all(lock_basic_type* lock, cond_basic_type* condition)
{
    lock_basic_lock(lock);
    /* [LOCK] lock */
    lock_basic_broadcast(condition);
    /* [UNLOCK] lock */
    lock_basic_unlock(lock);
    return;
}


/**
 * Clean up worker.
 *
 */
void
worker_cleanup(worker_type* worker)
{
    allocator_type* allocator;
    cond_basic_type worker_cond;
    lock_basic_type worker_lock;

    if (!worker) {
        return;
    }
    allocator = worker->allocator;
    worker_cond = worker->worker_alarm;
    worker_lock = worker->worker_lock;

    allocator_deallocate(allocator, (void*) worker);
    lock_basic_destroy(&worker_lock);
    lock_basic_off(&worker_cond);
    return;
}
