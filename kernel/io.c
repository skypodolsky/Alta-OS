#include <arch/io.h>
#include <sys/io.h>
#include <sys/types.h>

uint8_t g_tty_x;
uint8_t g_tty_y;
uint8_t g_tty_attr;

void sys_print(char* str) {
	int i = 0;
	while (str[i] != 0) {
		if (g_tty_x == MAX_TTY_X) {

			if (g_tty_y != MAX_TTY_Y - 1)
				g_tty_y++;
			else
				sys_tty_scroll();

			g_tty_x = 0;
		}

/* checking for newlines, tabs, or backspaces */
/* TODO: tabs, backspaces */

		if (str[i] == '\n') {
				g_tty_x = 0;
				g_tty_y++;
				i++;
		} else {
			arch_tty_byte_output(str[i], g_tty_attr);
			g_tty_x++;
			i++;
		}
	}
}

void sys_set_tty_xy(uint8_t x, uint8_t y) {
	g_tty_x = x;
	g_tty_y = y;
}

void sys_tty_get_mem(uint8_t x, uint8_t y, uint16_t* res) {
	arch_tty_get_mem(x, y, res);
}

void sys_tty_set_mem(uint8_t x, uint8_t y, uint16_t val) {
	arch_tty_set_mem(x, y, val);
}

void sys_tty_scroll() {
    uint8_t i, j;
    uint16_t res;

    for ( i = 0; i < MAX_TTY_Y; i++ )
		for ( j = 0; j < MAX_TTY_X; j++ ) {
			sys_tty_get_mem(j, i, &res);
				if ( (res & 0x00FF) != 0 ) {
					sys_tty_set_mem(j, i-1, res);
					sys_tty_set_mem(j, i, 1792);
				}
		}
}

void sys_tty_clear_screen() {
	int i, j;

	for (i = 0; i < 80; i++) {
		for (j = 0; j < 25; j++) {
			arch_tty_set_mem(i, j, (g_tty_attr << 8));	/* filling with null */
		}
	}

	sys_set_tty_xy(0, 0);
}

void sys_tty_set_attr(uint8_t foreground, uint8_t background) {
	g_tty_attr = background;
	g_tty_attr |= foreground;
}
