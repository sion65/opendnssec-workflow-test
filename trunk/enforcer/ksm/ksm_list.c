/*
 * $Id$
 *
 * Copyright (c) 2008-2009 Nominet UK. All rights reserved.
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

/*
 * ksm_list.c - List various aspects of the current configuration
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ksm/database.h"
#include "ksm/database_statement.h"
#include "ksm/datetime.h"
#include "ksm/db_fields.h"
#include "ksm/debug.h"
#include "ksm/ksmdef.h"
#include "ksm/ksm.h"
#include "ksm/ksm_internal.h"
#include "ksm/message.h"
#include "ksm/string_util.h"
#include "ksm/string_util2.h"

/*+
 * KsmListBackups - Output a list of all backups perfomed
 *
 *
 * Arguments:
 *
 *      int repo_id
 *          ID of the repository (-1 for all)
 *
 * Returns:
 *      int
 *          Status return.  0 on success.
 *                          other on fail
 */

int KsmListBackups(int repo_id)
{
    char*       sql = NULL;     /* SQL query */
    int         status = 0;     /* Status return */
    char        stringval[KSM_INT_STR_SIZE];  /* For Integer to String conversion */
    DB_RESULT	result;         /* Result of the query */
    DB_ROW      row = NULL;     /* Row data */

    char*       temp_date = NULL; /* place to store date returned */
    char*       temp_repo = NULL; /* place to store repository returned */

    /* Select rows */
    StrAppend(&sql, "select k.backup, s.name from keypairs k, securitymodules s ");
    StrAppend(&sql, "where s.id = k.securitymodule_id ");
    if (repo_id != -1) {
        StrAppend(&sql, "and s.id = ");
        snprintf(stringval, KSM_INT_STR_SIZE, "%d", repo_id);
        StrAppend(&sql, stringval);
    }
    StrAppend(&sql, " group by backup order by backup");

    DusEnd(&sql);

    status = DbExecuteSql(DbHandle(), sql, &result);

    if (status == 0) {
        status = DbFetchRow(result, &row);
        printf("Date:                    Repository:\n");
        while (status == 0) {
            /* Got a row, print it */
            DbString(row, 0, &temp_date);
            DbString(row, 1, &temp_repo);

            printf("%-24s %s\n", temp_date, temp_repo);
            
            status = DbFetchRow(result, &row);
        }

        /* Convert EOF status to success */

        if (status == -1) {
            status = 0;
        }

        DbFreeResult(result);
    }

    DusFree(sql);

    return status;
}

/*+
 * KsmListRepos - Output a list of all repositories available
 *
 *
 * Arguments:
 *
 *      none
 *
 * Returns:
 *      int
 *          Status return.  0 on success.
 *                          other on fail
 */

int KsmListRepos()
{
    char*       sql = NULL;     /* SQL query */
    int         status = 0;     /* Status return */
    DB_RESULT	result;         /* Result of the query */
    DB_ROW      row = NULL;     /* Row data */

    char*       temp_name = NULL;   /* place to store name returned */
    char*       temp_cap = NULL;    /* place to store capacity returned */
    int         temp_back = 0;      /* place to store backup flag returned */

    /* Select rows */
    StrAppend(&sql, "select name, capacity, requirebackup from securitymodules ");
    StrAppend(&sql, "order by name");

    DusEnd(&sql);

    status = DbExecuteSql(DbHandle(), sql, &result);

    if (status == 0) {
        status = DbFetchRow(result, &row);
        printf("Name:                            Capacity:    RequireBackup:\n");
        while (status == 0) {
            /* Got a row, print it */
            DbString(row, 0, &temp_name);
            DbString(row, 1, &temp_cap);
            DbInt(row, 2, &temp_back);

            printf("%-32s %-12s %s\n", temp_name, (strlen(temp_cap) == 0) ? "unset" : temp_cap, (temp_back == 0) ? "No" : "Yes");
            
            status = DbFetchRow(result, &row);
        }

        /* Convert EOF status to success */

        if (status == -1) {
            status = 0;
        }

        DbFreeResult(result);
    }

    DusFree(sql);

    return status;
}

/*+
 * KsmListPolicies - Output a list of all policies available
 *
 *
 * Arguments:
 *
 *      none
 *
 * Returns:
 *      int
 *          Status return.  0 on success.
 *                          other on fail
 */

int KsmListPolicies()
{
    char*       sql = NULL;     /* SQL query */
    int         status = 0;     /* Status return */
    DB_RESULT	result;         /* Result of the query */
    DB_ROW      row = NULL;     /* Row data */

    char*       temp_name = NULL;   /* place to store name returned */
    char*       temp_desc = NULL;   /* place to store description returned */

    /* Select rows */
    StrAppend(&sql, "select name, description from policies ");
    StrAppend(&sql, "order by name");

    DusEnd(&sql);

    status = DbExecuteSql(DbHandle(), sql, &result);

    if (status == 0) {
        status = DbFetchRow(result, &row);
        printf("Name:                            Description:\n");
        while (status == 0) {
            /* Got a row, print it */
            DbString(row, 0, &temp_name);
            DbString(row, 1, &temp_desc);

            printf("%-32s %s\n", temp_name, (strlen(temp_desc) == 0) ? "unset" : temp_desc);
            
            status = DbFetchRow(result, &row);
        }

        /* Convert EOF status to success */

        if (status == -1) {
            status = 0;
        }

        DbFreeResult(result);
    }

    DusFree(sql);

    return status;
}

/*+
 * KsmListRollovers - Output a list of expected rollovers
 *
 *
 * Arguments:
 *
 *      int zone_id
 *          ID of the zone (-1 for all)
 *
 * Returns:
 *      int
 *          Status return.  0 on success.
 *                          other on fail
 */

int KsmListRollovers(int zone_id)
{
    char*       sql = NULL;     /* SQL query */
    int         status = 0;     /* Status return */
    char        stringval[KSM_INT_STR_SIZE];  /* For Integer to String conversion */
    DB_RESULT	result;         /* Result of the query */
    DB_ROW      row = NULL;     /* Row data */

    char*       temp_zone = NULL;   /* place to store zone name returned */
    int         temp_type = 0;      /* place to store key type returned */
    char*       temp_date = NULL;   /* place to store date returned */

    /* Select rows */
    StrAppend(&sql, "select z.name, k.keytype, k.retire from zones z, KEYDATA_VIEW k where z.id = k.zone_id and active is not null ");
    if (zone_id != -1) {
        StrAppend(&sql, "and zone_id = ");
        snprintf(stringval, KSM_INT_STR_SIZE, "%d", zone_id);
        StrAppend(&sql, stringval);
    }
    StrAppend(&sql, " order by zone_id");

    DusEnd(&sql);

    status = DbExecuteSql(DbHandle(), sql, &result);

    if (status == 0) {
        status = DbFetchRow(result, &row);
        printf("Zone:                           Keytype:      Rollover expected:\n");
        while (status == 0) {
            /* Got a row, print it */
            DbString(row, 0, &temp_zone);
            DbInt(row, 1, &temp_type);
            DbString(row, 2, &temp_date);

            printf("%-31s %-13s %s\n", temp_zone, (temp_type == KSM_TYPE_KSK) ? "KSK" : "ZSK", temp_date);
            
            status = DbFetchRow(result, &row);
        }

        /* Convert EOF status to success */

        if (status == -1) {
            status = 0;
        }

        DbFreeResult(result);
    }

    DusFree(sql);

    return status;
}

/*+
 * KsmListKeys - Output a list of Keys
 *
 *
 * Arguments:
 *
 *      int zone_id
 *          ID of the zone (-1 for all)
 *
 * Returns:
 *      int
 *          Status return.  0 on success.
 *                          other on fail
 */

int KsmListKeys(int zone_id)
{
    char*       sql = NULL;     /* SQL query */
    int         status = 0;     /* Status return */
    char        stringval[KSM_INT_STR_SIZE];  /* For Integer to String conversion */
    DB_RESULT	result;         /* Result of the query */
    DB_ROW      row = NULL;     /* Row data */

    char*       temp_zone = NULL;   /* place to store zone name returned */
    int         temp_type = 0;      /* place to store key type returned */
    int         temp_state = 0;     /* place to store key state returned */
    char*       temp_ready = NULL;  /* place to store ready date returned */
    char*       temp_active = NULL; /* place to store active date returned */
    char*       temp_retire = NULL; /* place to store retire date returned */
    char*       temp_dead = NULL;   /* place to store dead date returned */

    /* Select rows */
    StrAppend(&sql, "select z.name, k.keytype, k.state, k.ready, k.active, k.retire, k.dead from zones z, KEYDATA_VIEW k where z.id = k.zone_id and state != 6 and zone_id is not null ");
    if (zone_id != -1) {
        StrAppend(&sql, "and zone_id = ");
        snprintf(stringval, KSM_INT_STR_SIZE, "%d", zone_id);
        StrAppend(&sql, stringval);
    }
    StrAppend(&sql, " order by zone_id");

    DusEnd(&sql);

    status = DbExecuteSql(DbHandle(), sql, &result);

    if (status == 0) {
        status = DbFetchRow(result, &row);
        printf("Zone:                           Keytype:      State:    Date of next transition:\n");
        while (status == 0) {
            /* Got a row, print it */
            DbString(row, 0, &temp_zone);
            DbInt(row, 1, &temp_type);
            DbInt(row, 2, &temp_state);
            DbString(row, 3, &temp_ready);
            DbString(row, 4, &temp_active);
            DbString(row, 5, &temp_retire);
            DbString(row, 6, &temp_dead);

            if (temp_state == KSM_STATE_PUBLISH) {
                printf("%-31s %-13s %-9s %s\n", temp_zone, (temp_type == KSM_TYPE_KSK) ? "KSK" : "ZSK", KsmKeywordStateValueToName(temp_state), temp_ready);
            }
            else if (temp_state == KSM_STATE_READY) {
                printf("%-31s %-13s %-9s %s\n", temp_zone, (temp_type == KSM_TYPE_KSK) ? "KSK" : "ZSK", KsmKeywordStateValueToName(temp_state), "next rollover");
            }
            else if (temp_state == KSM_STATE_ACTIVE) {
                printf("%-31s %-13s %-9s %s\n", temp_zone, (temp_type == KSM_TYPE_KSK) ? "KSK" : "ZSK", KsmKeywordStateValueToName(temp_state), temp_retire);
            }
            else if (temp_state == KSM_STATE_RETIRE) {
                printf("%-31s %-13s %-9s %s\n", temp_zone, (temp_type == KSM_TYPE_KSK) ? "KSK" : "ZSK", KsmKeywordStateValueToName(temp_state), temp_dead);
            }
            
            status = DbFetchRow(result, &row);
        }

        /* Convert EOF status to success */

        if (status == -1) {
            status = 0;
        }

        DbFreeResult(result);
    }

    DusFree(sql);

    return status;
}
