#ifndef __SEM_H__
#define __SEM_H__

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
#include <sys/types.h>
#include <sys/sem.h>

#define PATHNAME      "../common"
#define PROJECT_ID    101

int sem_create(int nsems, unsigned short values[]);
int sem_p(int semid, int semnum);
int sem_v(int semid, int semnum);
int sem_del(int semid);

#endif
