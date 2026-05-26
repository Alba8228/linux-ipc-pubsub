#include "thread.h"

tpool_t *thread_pool_init(int cnt, int queuesize)
{
	//给要永久使用的数据结构分配空间，而不是像栈一样函数结束就没有了
	tpool_t *pool= (tpool_t *)malloc(sizeof(tpool_t));
	if(pool == NULL){perror("malloc():\n"); exit(1);}
	
	// 对于内部成原数量大于1还应另外申请空间//前面为整个结构体分配的空间中threadid的空间是没有用到的
	pool->tp_work_thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * cnt);
	if(pool->tp_work_thread_ids == NULL){perror("malloc():\n"); exit(1);}
	
	pool->tp_number_of_threads = cnt;
	pool->tp_threads_alive = cnt;
	
	 int ret = pthread_mutex_init(&pool->tp_mutex_pool, NULL);
	 if(ret != 0){perror("[error] pthread_mutex_init():\n"); exit(1);}
	 
	 ret = pthread_cond_init(&pool->tp_cond_empty, NULL);
	 if(ret != 0){perror("[error] pthread_cond_init():\n"); exit(1);}
	 ret = pthread_cond_init(&pool->tp_cond_full, NULL);
	 if(ret != 0){perror("[error] pthread_cond_init():\n"); exit(1);}
	 ret = pthread_cond_init(&pool->tp_cond_all_idle, NULL);
	 if(ret != 0){perror("[error] pthread_cond_init():\n"); exit(1);}
	 
	 pool->tp_qcapacity = queuesize;
	 
	 //为任务队列申请空间
	 pool->tp_task = (tpool_task_t *)malloc(sizeof(tpool_task_t) * queuesize);
	if(pool->tp_task == NULL){perror("malloc():\n"); exit(1);}
	
	pool->tp_qsize = 0;
	pool->tp_qfront = 0;
	pool->tp_qrear = 0;
	
	pool->tp_shutdown = false;
	
	for(int i = 0; i < cnt; i++)
	{
		ret = pthread_create(&pool->tp_work_thread_ids[i], NULL, tp_worker, pool);
		if(ret != 0){perror("[error] pthread_create():\n"); exit(1);}
	}	
	
	return pool;
}

void thread_pool_add_task(tpool_t *pool, void (*task)(void *arg), void *arg)//向任务队列中添加任务
{
	pthread_mutex_lock(&pool->tp_mutex_pool);
	
	while(pool->tp_qcapacity == pool->tp_qsize && !pool->tp_shutdown)//任务队列满了
	{
		pthread_cond_wait(&pool->tp_cond_full, &pool->tp_mutex_pool);//占着队列满的锁 表队列满 
	}
	
	if(pool->tp_shutdown){ pthread_mutex_unlock(&pool->tp_mutex_pool);return;}
	
	pool->tp_task[pool->tp_qrear].function = task;//向任务队列添加函数
	pool->tp_task[pool->tp_qrear].arg = arg;
	pool->tp_qrear = (pool->tp_qrear + 1) % pool->tp_qcapacity;
	pool->tp_qsize++;
	
	pthread_cond_signal(&pool->tp_cond_empty);//释放队列空的锁 表示队列不空
	pthread_mutex_unlock(&pool->tp_mutex_pool);
}
void *tp_worker(void *arg)//消耗任务队列
{
	tpool_t *pool = (tpool_t *)arg;
	tpool_task_t task;//如果task要在此函数外用应malloc在堆区申请而不是现再的栈区
	while(1)
	{
		pthread_mutex_lock(&pool->tp_mutex_pool);
		while(pool->tp_qsize == 0 && !pool->tp_shutdown)
		{
			pthread_cond_wait(&pool->tp_cond_empty, &pool->tp_mutex_pool);
		}
		if(pool->tp_shutdown)
		{
			pool->tp_threads_alive--;
			if(pool->tp_threads_alive == 0)
			{
				pthread_cond_signal(&pool->tp_cond_all_idle);
			}
			pthread_mutex_unlock(&pool->tp_mutex_pool);
			return NULL;
		}
		
		
		task.function = pool->tp_task[pool->tp_qfront].function;
		task.arg = pool->tp_task[pool->tp_qfront].arg;
		pool->tp_qfront = (pool->tp_qfront + 1) % pool->tp_qcapacity;
		pool->tp_qsize--;
		
		pthread_cond_signal(&pool->tp_cond_full);
		pthread_mutex_unlock(&pool->tp_mutex_pool);
		
		
		task.function(task.arg);
		// 注意：任务参数的释放由调用者管理，或者在这里根据约定释放
	}
}

int thread_pool_destroy(tpool_t *pool)
{
	if(pool == NULL){return -1;}
	
	pthread_mutex_lock(&pool->tp_mutex_pool);
	pool->tp_shutdown = true;
	pthread_mutex_unlock(&pool->tp_mutex_pool);
	
	// 唤醒所有等待的工作线程
	for(int i = 0; i < pool->tp_number_of_threads; i++)
	{
		pthread_cond_signal(&pool->tp_cond_empty);	
	}
	
	// 等待所有线程退出
	pthread_mutex_lock(&pool->tp_mutex_pool);
	while(pool->tp_threads_alive > 0)
	{
		pthread_cond_wait(&pool->tp_cond_all_idle, &pool->tp_mutex_pool);
	}
	pthread_mutex_unlock(&pool->tp_mutex_pool);
	
	// 现在可以安全销毁同步对象和释放内存
	pthread_mutex_destroy(&pool->tp_mutex_pool);
	pthread_cond_destroy(&pool->tp_cond_full);
	pthread_cond_destroy(&pool->tp_cond_empty);
	pthread_cond_destroy(&pool->tp_cond_all_idle);
	
	free(pool->tp_work_thread_ids);
	free(pool->tp_task);
	free(pool);
	
	return 0;
}






















