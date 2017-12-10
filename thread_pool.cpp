#include "thread_pool.h"
#include <math.h>

ThreadPool::ThreadPool(int numThreads__) : numThreads(numThreads__) 
{ 
	thrds = vector<pthread_t>(numThreads);
	queueMutex_t = new pthread_mutex_t;
	pthread_mutex_init(queueMutex_t, nullptr);
	queueCond_t = new pthread_cond_t;
	pthread_cond_init(queueCond_t, nullptr);
	for(int tid = 0; tid < numThreads; tid++) {
		pthread_mutex_t * mutex = new pthread_mutex_t;
		pthread_cond_t * p = new pthread_cond_t;
		pthread_cond_t * c = new pthread_cond_t;
		pthread_mutex_init(mutex, nullptr);
		pthread_cond_init(p, nullptr);
		pthread_cond_init(c, nullptr);
		threadMutex.push_back(mutex);
		consumerCond.push_back(c);
		producerCond.push_back(p);
		thread_pool_params_t * par = new thread_pool_params_t;
		par->tid = tid;
		par->mutex = mutex;
		par->p = p;
		par->c = c;
		par->queueMutex_t = queueMutex_t;
		par->queueCond_t = queueCond_t;
		ThreadStatus * status;
		status = new ThreadStatus;
		*status = ThreadStatus::SLEEP;
		par->status = status;
		par->idle = &idle;
		pars.push_back(par);
		int ret = pthread_create(&thrds[tid], nullptr, start_routine, (void*)(par));
		if(ret) fprintf(stderr, "ERROR: return code from pthread_create() is %d\n", ret);
	}
}

ThreadPool::~ThreadPool() 
{
	for(int tid = 0; tid < numThreads; tid++) {
		pthread_mutex_t * mutex = threadMutex[tid];
		pthread_cond_t * c = consumerCond[tid];
		thread_pool_params_t * par = pars[tid];
		ThreadStatus * status = par->status;
		pthread_mutex_lock(mutex);
		*status	= ThreadStatus::EXIT;
		pthread_cond_signal(c);
		pthread_mutex_unlock(mutex);
	}
	for(int tid = 0; tid < numThreads; tid++) {
		void * status;
		int ret = pthread_join(thrds[tid], &status);
		if(ret) fprintf(stderr, "ERROR: return code from pthread_join() with thread Id %d is %d\n", tid, ret);
	}
	for(int tid = 0; tid < numThreads; tid++) {
		pthread_mutex_destroy(threadMutex[tid]);
		delete threadMutex[tid];
		threadMutex[tid] = nullptr;
		pthread_cond_destroy(consumerCond[tid]);
		delete consumerCond[tid];
		consumerCond[tid] = nullptr;
		pthread_cond_destroy(producerCond[tid]);
		delete producerCond[tid];
		producerCond[tid] = nullptr;
		delete pars[tid]->status;
		delete pars[tid];
		pars[tid] = nullptr;	
	}
	pthread_mutex_destroy(queueMutex_t);
	delete queueMutex_t;
	pthread_cond_destroy(queueCond_t);
	delete queueCond_t;
}

void ThreadPool::addTask(task_params_t task)
{
	taskList.push_back(task);
}

void ThreadPool::addTask(pthreadFunction_t pfunc, void * arg)
{
	task_params_t task(pfunc, arg);

	taskList.push_back(task);
}

void ThreadPool::run()
{
	for(int tid = 0; tid < numThreads; tid++) {
		idle.push(tid);
	}

	while(!taskList.empty()) {
		task_params_t task = taskList.back();
		taskList.pop_back();
		int tid;
		pthread_mutex_lock(queueMutex_t);
		while(idle.empty()) {
			pthread_cond_wait(queueCond_t, queueMutex_t);
		}
		tid = idle.front();
		idle.pop();
		pthread_mutex_unlock(queueMutex_t);
		pthread_mutex_t * mutex = threadMutex[tid];
		pthread_cond_t * p = producerCond[tid];
		pthread_cond_t * c = consumerCond[tid];
		thread_pool_params_t * par = pars[tid];
		ThreadStatus * status = par->status;
		pthread_mutex_lock(mutex);
		while(*status == ThreadStatus::AWAKE) {
			pthread_cond_wait(p, mutex);
		}
		*status = ThreadStatus::AWAKE;
		par->prog = task.prog;
		par->params = task.arg;
		pthread_cond_signal(c);		
		pthread_mutex_unlock(mutex);
	}
}

static void * start_routine(void * arg) {
	thread_pool_params_t * pthrd = (thread_pool_params_t*)arg;
	int tid = pthrd->tid;
	pthread_mutex_t * mutex = pthrd->mutex;
	pthread_cond_t * p = pthrd->p;
	pthread_cond_t * c = pthrd->c;
	ThreadStatus * status = pthrd->status;	
	pthread_mutex_t * queueMutex_t = pthrd->queueMutex_t;
	pthread_cond_t * queueCond_t = pthrd->queueCond_t;
	queue<int> * idle = pthrd->idle;

	while(1) {
		pthread_mutex_lock(mutex);
		while(*status == ThreadStatus::SLEEP) {
			pthread_cond_wait(c, mutex);
		}
		if(*status == ThreadStatus::EXIT) break;
		*status = ThreadStatus::SLEEP;
		pthreadFunction_t prog = pthrd->prog;
		void * params = pthrd->params;	
		(*prog)(params);
		pthread_cond_signal(p);
		pthread_mutex_unlock(mutex);
		pthread_mutex_lock(queueMutex_t);
		idle->push(tid);
		pthread_cond_signal(queueCond_t);	
		pthread_mutex_unlock(queueMutex_t);
	}
	pthread_exit(nullptr);
}

void * print_sin(void * arg)
{
	float * theta = (float*)arg;
	
	fprintf(stderr, "sin(%.6f) = %.6f\n", *theta, sin(*theta));

	return nullptr;
}

int main(int argc, char * argv[])
{
	const int numThreads = 8;
	ThreadPool tpl(numThreads);
	int N = 200;
	float theta[N];
	for(int i = 0; i < N; i++) {
		theta[i] = M_PI * i / N;
	}
	for(int i = 0; i < N; i++) {
		tpl.addTask(print_sin, (void*)&theta[i]);
	}
	fprintf(stdout, "INFO: add task\n");
	fflush(stdout);
	tpl.run();
	fprintf(stdout, "INFO: thread pool run complete\n");
	return EXIT_SUCCESS;
	
}
