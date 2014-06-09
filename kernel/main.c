#include <sys/io.h>
#include <sys/stdlib.h>
#include <arch/color.h>

uint8_t g_tty_x;
uint8_t g_tty_y;
uint8_t g_tty_attr;

int kernel_start() {

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	printf("Kernel started!\n");

	while(1) {
		/* hlt */
	}
}
