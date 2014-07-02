#include <arch/idt.h>
#include <arch/color.h>

#include <sys/io.h>
#include <sys/gdt.h>
#include <sys/mem.h>
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

	while(1) {
		char string[256];
		gets(string);
		printf("\n%s\n", string);
	}
}
