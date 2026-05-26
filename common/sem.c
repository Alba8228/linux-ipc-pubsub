#include "sem.h"

int sem_create(int nsems, unsigned short values[])
{
	key_t key = ftok(PATHNAME, PROJECT_ID);
	if(key == -1){perror("ftok():\n"); exit(1);}
	
	int semid = semget(key, nsems, IPC_CREAT | 0644);
	if(semid == -1){perror("semget():\n");exit(1);} 
	
	int ret = semctl(semid, 0, SETALL, values);
	if(ret == -1){perror("semctl():\n"); exit(1);}
	
	return semid;
}

int sem_p(int semid, int semnum)
{
	struct sembuf sops;
	sops.sem_num = semnum;
	sops.sem_op = -1;
	
	return semop(semid, &sops, 1);
}

int sem_v(int semid, int semnum)
{
	struct sembuf sops;
	sops.sem_num = semnum;
	sops.sem_op = 1;
	
	return semop(semid, &sops, 1);
}

int sem_del(int semid)
{
	return semctl(semid, 0, IPC_RMID, NULL);
}
