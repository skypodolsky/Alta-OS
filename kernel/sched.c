#include <sys/mem.h>
#include <sys/list.h>
#include <sys/sched.h>
#include <sys/types.h>
#include <sys/stdlib.h>

uint32_t max_pid;

struct alta_sched *sched;

void sys_task_head_init() {
	sched = kvalloc(sizeof(struct alta_sched));
	if (!sched) {
		/* TODO: panic() function */
	}
	
	memset(sched, 0, sizeof(struct alta_sched));
	LIST_HEAD_INIT(&sched->task_head);
	sched->task_tail = &sched->task_head;
}

uint32_t sys_add_task(uint8_t parent_id) {
	struct alta_task *task = kvalloc(sizeof(struct alta_task));
	if (!task)
		return 0;

	task->pid			= max_pid++;
	task->parent_id		= parent_id;
	task->status		= TASK_STATUS_SUSPENDED;
	task->is_executable = 0;
	
	LIST_INSERT(&task->head, sched->task_tail);
	sched->task_tail = &task->head;
	return task->pid;
}

void sys_scheduler() {
	struct list_head *task_tmp;		/* pointer to surf the pointers */
	struct alta_task *the_task;

	task_tmp = &sched->task_head;
	list_for_each(task_tmp) {
		the_task = GET_LIST_DATA(struct alta_task, task_tmp);
		printf("scheduling PID = %d\n", the_task->pid);
	}
}
