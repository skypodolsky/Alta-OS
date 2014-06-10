#include <sys/io.h>
#include <sys/stdlib.h>
#include <arch/color.h>
#include <sys/gdt.h>
#include <arch/idt.h>
#include <sys/debug.h>

int kernel_start() {

	sys_tty_clear_screen();
	sys_tty_set_xy(0, 0);

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	printf("Kernel started!\n");

	gdt_init();
	idt_init();

	*(uint32_t*)(0xFFFFFF) = 2;

	while(1) {
		/* hlt */
	}
}
