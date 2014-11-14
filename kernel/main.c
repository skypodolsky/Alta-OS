#include <arch/idt.h>
#include <arch/time.h>
#include <arch/color.h>

#include <sys/io.h>
#include <sys/fs.h>
#include <sys/gdt.h>
#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/list.h>
#include <sys/sched.h>
#include <sys/debug.h>
#include <sys/stdlib.h>
#include <sys/keyboard.h>

int kernel_start() {

	sys_tty_clear_screen();
	sys_tty_set_xy(0, 0);

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	DBG_PRINT("Kernel started!\n");

	sys_gdt_init();
	sys_idt_init();
	sys_keyboard_init();
	sys_kmem_table_init();
	sys_ata_init();
	sys_afs_init();
	sys_task_head_init();

/*
	uint32_t pid = sys_add_task(0);
	pid = sys_add_task(pid);
	pid = sys_add_task(pid);
	pid = sys_add_task(pid);
	pid = sys_add_task(pid);
	pid = sys_add_task(pid);
	pid = sys_add_task(pid);
*/

	while(1) {
		sys_scheduler();
	}
}
