#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "../common/protocol.h"
#include "../common/shm.h"
#include "../common/shmfifo.h"
#include "../common/msg.h"
#include "../common/config.h"
#include "../common/net.h"

void client_init(const config_t *cfg);
void client_shutdown();
void subscribe(char *topic);
void publish(char *topic, char *content);
void unsubscribe();

#endif
