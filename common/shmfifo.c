#include "shmfifo.h"

shm_fifo_t *shmfifo_init(int blocks, int blksz)
{
	int shm_sz = blocks * blksz + sizeof(shm_head_t);
	unsigned short values[] = {0, blocks, 1};
	int shmid;

	shm_fifo_t *p_shm_fifo = (shm_fifo_t *)malloc(sizeof(shm_fifo_t));
	if(p_shm_fifo == NULL){perror("malloc():\n"); return NULL;}
		
	enum shm_create_status s_create_status = shm_create(shm_sz, &shmid);
	if(s_create_status != SHM_CREATE_ERROR)
	{
		p_shm_fifo->shmid = shmid;
		p_shm_fifo->p_head = shm_at(shmid);
		if(s_create_status == SHM_CREATE_NEW)
		{
			p_shm_fifo->p_head->rpos = 0;
			p_shm_fifo->p_head->wpos = 0;
			p_shm_fifo->p_head->blocks = blocks;
			p_shm_fifo->p_head->blksz = blksz;
			p_shm_fifo->p_head->semid = sem_create(3, values);
		}
		
		p_shm_fifo->p_payload = (char *)(p_shm_fifo->p_head + 1);
		
		return p_shm_fifo;
	}
	
	free(p_shm_fifo);
	return NULL;
}

void shmfifo_destroy(shm_fifo_t *fifo)
{
	if(fifo == NULL) return;
	
	sem_del(fifo->p_head->semid);
	shm_dt(fifo->p_head);
	shm_del(fifo->shmid);
	free(fifo);
}

void shmfifo_put(shm_fifo_t *fifo, const void *buf)
{
	sem_p(fifo->p_head->semid, SEM_FULL_ID);
	sem_p(fifo->p_head->semid, SEM_MUTEX_ID);
	
	int pos = fifo->p_head->wpos * fifo->p_head->blksz;
	
	memcpy(fifo->p_payload + pos, buf, fifo->p_head->blksz);
	fifo->p_head->wpos = (fifo->p_head->wpos + 1) % (fifo->p_head->blocks);
	
	sem_v(fifo->p_head->semid, SEM_MUTEX_ID);
	sem_v(fifo->p_head->semid, SEM_EMPTY_ID);
}

void shmfifo_get(shm_fifo_t *fifo, void *buf)
{
	sem_p(fifo->p_head->semid, SEM_EMPTY_ID);
	sem_p(fifo->p_head->semid, SEM_MUTEX_ID);
	
	int pos = fifo->p_head->rpos * fifo->p_head->blksz;
	
	memcpy(buf, fifo->p_payload + pos, fifo->p_head->blksz);
	fifo->p_head->rpos = (fifo->p_head->rpos + 1) % (fifo->p_head->blocks);
	
	sem_v(fifo->p_head->semid, SEM_MUTEX_ID);
	sem_v(fifo->p_head->semid, SEM_FULL_ID);
}
