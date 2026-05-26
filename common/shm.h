#ifndef __SHM_H__
#define __SHM_H__

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

#define PATHNAME      "../common"
#define PROJECT_ID    101

enum shm_create_status
{
	SHM_HAS_EXIST = 0,
	SHM_CREATE_NEW,
	SHM_CREATE_ERROR,
};

enum shm_create_status shm_create(size_t size, int *pshmid);
void *shm_at(int shmid);
int shm_dt(const void *shmaddr);
int shm_del(int shmid);

#endif
