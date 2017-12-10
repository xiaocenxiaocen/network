#ifndef __TASK_H__
#define __TASK_H__

#define MAX_PROGNAME_SIZE 256
#define MAX_PARAMS_SIZE 256
#define MAX_TIME_SIZE 32
#define MAX_SLAVERNAME_SIZE 32 

enum TaskStatus { UNDO, PROCESSING, COMPLETED };
typedef struct {
	unsigned int task_id;
	char prog[MAX_PROGNAME_SIZE];
	char par[MAX_PARAMS_SIZE];
	char start_time[MAX_TIME_SIZE];
	char complete_time[MAX_TIME_SIZE];
	char slaver_name[MAX_SLAVERNAME_SIZE];
	TaskStatus status;	
}task_t;

#endif
