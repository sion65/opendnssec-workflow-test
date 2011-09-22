#ifndef _KEYSTATE_EXPORT_CMD_H_
#define _KEYSTATE_EXPORT_CMD_H_

#include "daemon/engine.h"

#ifdef __cplusplus
extern "C" {
#endif

void help_keystate_export_cmd(int sockfd);

int handled_keystate_export_cmd(int sockfd, engine_type* engine,
                                const char *cmd, ssize_t n);

#ifdef __cplusplus
}
#endif

#endif
