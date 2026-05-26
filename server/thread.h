#ifndef __THREAD_H__
#define __THREAD_H__

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


typedef struct tpool_task
{
	void (*function)(void *arg);
	void *arg;
}tpool_task_t;


typedef struct tpool//对于有多个数量的在结构体内都用指针定义方便后面申请堆空间
{
	tpool_task_t *tp_task;//指向任务队列的指针
	
	int tp_qcapacity;//可容纳任务数量
	int tp_qsize;//当前任务长度
	
	int tp_qfront;
	int tp_qrear;//任务队列尾
	pthread_t *tp_work_thread_ids;//线程id
	
	int tp_number_of_threads;
	int tp_threads_alive; // 存活的线程数
	
	pthread_mutex_t tp_mutex_pool;
	
	pthread_cond_t tp_cond_empty;//任务队列是否为空
	pthread_cond_t tp_cond_full;
	pthread_cond_t tp_cond_all_idle; // 所有线程空闲
	
	bool tp_shutdown;//线程销毁标志
}tpool_t;


//初始化返回的句柄，都是针对这个结构体来的
tpool_t *thread_pool_init(int cnt, int queuesize);//线程数量，任务队列大小
void thread_pool_add_task(tpool_t *pool, void (*task)(void *arg), void *arg);
void *tp_worker(void *arg);
int thread_pool_destroy(tpool_t *pool);


#endif











