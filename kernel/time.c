#include <arch/time.h>

volatile uint64_t jiffies;

uint64_t x86_get_tsc() {
	unsigned long tscl = 0;
	unsigned long tsch = 0;

	asm volatile ("rdtsc" : "=a"(tscl), "=d"(tsch));

	return ((uint64_t)tsch << 32 ) | tscl;
}
