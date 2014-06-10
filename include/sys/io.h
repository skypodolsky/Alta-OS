#ifndef SYS_IO_HEADER
#define SYS_IO_HEADER

#include <sys/types.h>

extern uint8_t g_tty_x;
extern uint8_t g_tty_y;
extern uint8_t g_tty_attr;

#define MAX_TTY_X 80
#define MAX_TTY_Y 25

void sys_print( char* );
void sys_tty_set_xy( uint8_t, uint8_t );
void sys_tty_clear_screen();
void sys_tty_set_attr( uint8_t, uint8_t );
void sys_tty_scroll();

#endif /* SYS_IO_HEADER */
