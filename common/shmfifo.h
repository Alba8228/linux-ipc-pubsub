#ifndef __SHMFIFO_H__
#define __SHMFIFO_H__

#include "shm.h"
#include "sem.h"

#define SEM_EMPTY_ID 0
#define SEM_FULL_ID 1
#define SEM_MUTEX_ID 2

typedef struct
{
	int rpos;
	int wpos;
	int blocks;
	int blksz;
	int semid;
}shm_head_t;

typedef struct
{
	shm_head_t *p_head;
	char *p_payload;	
	int shmid; // 保存当前实例的共享内存 ID
}shm_fifo_t;

shm_fifo_t *shmfifo_init(int blocks, int blksz);
void shmfifo_destroy(shm_fifo_t *fifo); // 修复拼写
void shmfifo_put(shm_fifo_t *fifo, const void *buf);
void shmfifo_get(shm_fifo_t *fifo, void *buf);

#endif
