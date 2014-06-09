#ifndef ARCH_IO_HEADER
#define ARCH_IO_HEADER

#include <sys/types.h>

extern uint8_t g_tty_x;
extern uint8_t g_tty_y;

void arch_tty_byte_output( char, uint8_t );
void arch_tty_get_mem( uint8_t, uint8_t, uint16_t* );
void arch_tty_set_mem( uint8_t, uint8_t, uint16_t );

#endif /* IO_HEADER */
