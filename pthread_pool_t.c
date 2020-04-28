/*
 * @Description: 
 * @Author: Neo
 * @Github: https://github.com/PeterYe2014
 * @Date: 2020-04-18 21:05:40
 * @LastEditTime: 2020-04-18 21:06:53
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
// 定义线程需要执行的任务，通过任务队列来表示所有的任务
typedef struct worker_t{	
	void* (* process) (void *arg); // 执行的任务方法
	void *arg; // 执行任务的参数
	struct worker_t  *next; // 指向的下一个任务
} worker_t;
// 定义线程池
typedef struct pthread_pool_t{
	pthread_mutex_t	mutex; // 互斥锁
	pthread_cond_t	cond; // 线程池条件变量
	worker_t	*worker_queue; // 线程池的任务队列
	int shutdown;	// 标记线程池是否被销毁
	pthread_t *tids; // 线程池所有线程的线程id，用于等待线程完成
	int core_thread_num; // 线程池里面核心线程的数量
	int worker_queue_size; // 
} pthread_pool_t;

int 	pool_add_worker(void* (*process)(void* arg), void *arg);
void *pthread_run(void *arg);
static pthread_pool_t *pool = NULL;

void pool_init(int core_thread_num){
	pool = (pthread_pool_t*)  malloc(sizeof(pthread_pool_t));
	pool->worker_queue = NULL;
	pool->shutdown = 0;
	pool->worker_queue_size = 0;
pool->core_thread_num = core_thread_num;
pool->tids = (pthread_t*) malloc(sizeof(pthread_t) * core_thread_num);
int i;
for(i=0; i < core_thread_num; i++){
	pthread_create(&(pool->tids[i]), NULL, pthread_run, NULL);
}
}

int pool_add_worker(void* (*process)(void* arg), void *arg){
	worker_t* new_worker = (worker_t*) malloc(sizeof(worker_t));
	new_worker->process = process;
	new_worker->arg = arg;
	new_worker->next = NULL:
	pthread_mutex_lock(&(pool->mutex));
	worker_t* queue_head = pool->worker_queue;
	if( queue_head != NULL){
		while(queue_head->next != NULL)
			queue_head = queue_head->next;
		queue_head->next = new_worker;
}
	else
		pool->worker_queue = new_worker;
	pool->worker_queue_size ++;
	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_signal(&(pool->cond));
	return 0;
}

int pool_destroy(){
	if(pool->shutdown)
		return -1;
	pool->shutdown = 1;
	pthread_cond_broadcast(&(pool->cond));
	int i;
	for(i=0; i < pool->core_thread_num; i++){
		pthread_join(pool->tids[i], NULL);
}
free(pool->tids);
worker_t *head = pool->worker_queue;
while(head != NULL){
	head = head->next;
	free(head);
}
pthread_mutex_destroy(&(pool->mutex));
pthread_cond_destory(&(pool->cond));
free(pool);
pool = NULL;
return 0;
}

void	*phtread_run(void* arg){
	while(1){
		pthread_mutex_lock(&(pool->mutex));
		while(pool ->worker_queue_size == 0 && pool -> shutdown != 1){
			pthread_signal_wait(&(pool->cond), &(pool->mutex));
}
if(pool -> shutdown){
	pthread_mutex_unlock(&(pool->mutex));
	pthread_exit(NULL);
}
pool->worker_queue_size --;
			worker_t *worker = pool->worker_queue;
			pool->worker_queue = pool->worker_queue->next;
			pthread_mutex_unlock(&(pool->mutex));
			(*(worker->process)) (woker->arg));
			free(worker);
			worker = NULL;
		  }
	 pthread_exit(NULL);
}
}
