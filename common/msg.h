#ifndef __MSG_H__
#define __MSG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include <errno.h>
#include <poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>


int msg_create();
int msg_send(int msgid, long mtype, char *buf);
int msg_recv(int msgid, long mtype, char *buf, int len);
int msg_del(int msgid);

#endif
