#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <queue>

using std::vector;
using std::queue;

enum class ThreadStatus : char { AWAKE, SLEEP, EXIT };

static void * start_routine(void * arg);

typedef void*(*pthreadFunction_t)(void *arg);
struct task_params_t {
	pthreadFunction_t prog;
	void * arg;
	task_params_t() : prog(nullptr), arg(nullptr) { };
	task_params_t(pthreadFunction_t prog__, void * arg__) : prog(prog__), arg(arg__) { };
};

struct thread_pool_params_t {
	int tid;
	pthread_mutex_t * mutex;
	pthread_cond_t * p;
	pthread_cond_t * c;
	ThreadStatus * status;
	pthread_mutex_t * queueMutex_t;
	pthread_cond_t * queueCond_t;
	pthreadFunction_t prog;
	void * params;
	queue<int> * idle;
	thread_pool_params_t() : tid(0), mutex(nullptr), p(nullptr), c(nullptr), status(nullptr), queueMutex_t(nullptr), queueCond_t(nullptr), prog(nullptr), params(nullptr) { };	
};


class ThreadPool {
public:
	ThreadPool() { };
	ThreadPool(int numThreads__);
	~ThreadPool();
	void addTask(task_params_t task);
	void addTask(pthreadFunction_t prog, void * arg);
	void run();
public:
	int numThreads;
	vector<pthread_t> thrds;
	vector<task_params_t> taskList;
	queue<int> idle;

	pthread_cond_t * queueCond_t;
	pthread_mutex_t * queueMutex_t;
	
	vector<thread_pool_params_t*> pars;
	vector<pthread_mutex_t*> threadMutex;
	vector<pthread_cond_t*> consumerCond;
	vector<pthread_cond_t*> producerCond;	
};

#endif
