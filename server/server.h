#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common/protocol.h"
#include "../common/shm.h"
#include "../common/shmfifo.h"
#include "../common/config.h"
#include "../common/net.h"
#include "thread.h"
#include "hashmap.h"
#include "../common/msg.h"
#include <signal.h>

void server_init(const config_t *cfg);
void server_shutdown();
void runloop();
void task_handler(void *arg);
#endif
