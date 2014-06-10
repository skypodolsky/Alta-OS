#include <sys/io.h>
#include <sys/stdlib.h>
#include <arch/color.h>
#include <sys/gdt.h>
#include <sys/debug.h>

int kernel_start() {

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	printf("Kernel started!\n");

	gdt_init();
	printf("GDT entries reloaded!\n");
	DBG_PRINT(KERN_WARN "This may be dangerous!\n");

	while(1) {
		/* hlt */
	}
}
