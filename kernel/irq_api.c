#include <arch/irq_api.h>
#include <arch/time.h>

#include <sys/types.h>
#include <sys/stdlib.h>
#include <sys/keyboard.h>

void irq_process_keyboard() {

uint8_t status;
uint8_t scan_code;

    inportb(0x60, scan_code);

	sys_keyboard_handler(scan_code);

    inportb(0x61, status);      /* ready to get another symbol */
    status |= 1;
    outportb(0x61, status);

}

void irq_process_timer() {
	/* we update system timer every 10 ms */
	jiffies++;
}
