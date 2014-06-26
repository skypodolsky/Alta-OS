#include <sys/types.h>
#include <sys/keyboard.h>

char keybuf[MAX_KEYBUF];
uint8_t keybuf_tail;
uint8_t keybuf_head;

void sys_keyboard_handler(uint8_t scan_code) {

	if (keybuf_tail >= MAX_KEYBUF) {
		keybuf_tail = 0;
	}

	keybuf_tail++;
	keybuf[keybuf_tail - 1] = scan_code;

}
