#include <sys/io.h>
#include <sys/stdlib.h>
#include <arch/color.h>

int kernel_start() {

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	printf("Kernel started!\n");

	gdt_init();

	while(1) {
		/* hlt */
	}
}
