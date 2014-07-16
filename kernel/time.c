#include <arch/time.h>
#include <sys/time.h>

volatile uint64_t jiffies;

uint64_t x86_get_tsc() {
	unsigned long tscl = 0;
	unsigned long tsch = 0;

	asm volatile ("rdtsc" : "=a"(tscl), "=d"(tsch));

	return ((uint64_t)tsch << 32) | tscl;
}

uint64_t get_cpu_freq() {

	uint64_t tsc_tmp;
	uint64_t tsc_tmp_n;
	tsc_tmp = x86_get_tsc();
	sleep(1);
	tsc_tmp_n = x86_get_tsc();

	if (tsc_tmp < tsc_tmp_n)
		return tsc_tmp_n - tsc_tmp;
	else
		return -1;
}
