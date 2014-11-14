#ifndef SCHED_HEADER
#define SCHED_HEADER

#include <sys/list.h>
#include <sys/types.h>

struct alta_task {

	uint8_t pid;			/* PID */
	uint8_t prio;			/* priority */
	uint8_t status;			/* 0 - suspended, 1 - running */
	uint8_t parent_id;		/* PID of the parent process */
	uint8_t is_executable;	/* should the task be scheduled while processing the task list */

	/* Value of registers */
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;

	struct list_head head;
};

struct alta_sched {
	uint32_t task_max_pid;
	struct list_head task_head;		/* main head of task list */
	struct list_head *task_tail;	/* using as the pointer to the end of list */
};

enum {
	TASK_STATUS_SUSPENDED = 0,
	TASK_STATUS_SCHEDULING,
	TASK_STATUS_PROCESSED
} TASK_STATUS;

void sys_task_head_init();
uint32_t sys_add_task(uint8_t parent_id);

void sys_scheduler();

#endif /* SCHED_HEADER */
