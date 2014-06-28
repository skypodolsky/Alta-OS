#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

#include <sys/types.h>

#define MAX_KEYBUF 16
#define MAX_CHARSET 256

extern char keybuf[MAX_KEYBUF];		/* keyboard buffer */
extern uint8_t keybuf_head;
extern uint8_t keybuf_tail;

void sys_keyboard_handler(uint8_t);
uint8_t sys_keyboard_get_scan_code();
char sys_keyboard_read_char();
void sys_keyboard_charset_init();
void sys_keyboard_init();

#endif /* KEYBOARD_HEADER */
