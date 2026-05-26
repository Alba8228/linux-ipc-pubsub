#include "shm.h"

enum shm_create_status shm_create(size_t size, int *pshmid)
{
	key_t key = ftok(PATHNAME, PROJECT_ID);
	if(key == -1){perror("ftok():\n"); exit(1);}
	
	*pshmid = shmget(key, size, 0);
	if(*pshmid == -1)
	{	
		*pshmid = shmget(key, size, IPC_CREAT | 0644);
		if(*pshmid == -1){perror("shmget():\n");return SHM_CREATE_ERROR;} 
		return SHM_CREATE_NEW;
	}
	else return SHM_HAS_EXIST;
}

void *shm_at(int shmid)
{
	void *addr = NULL;
	addr = shmat(shmid, NULL, 0);
	if(addr == (void *)-1){perror("shmat():\n");return NULL;}
	
	return addr;
}

int shm_dt(const void *shmaddr)
{
	return shmdt(shmaddr);
}

int shm_del(int shmid)
{
	return shmctl(shmid, IPC_RMID, NULL);
}
