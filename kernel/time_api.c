#include <sys/time.h>
#include <arch/time.h>
extern volatile uint64_t jiffies;

void sleep (uint8_t sec) {
	uint64_t jiffies_tmp;
	jiffies_tmp = jiffies;
	if (sec >= 1) {
		while(jiffies != jiffies_tmp + (sec*100)) 
		{

		}
	}
}

uint64_t get_cpu_freq() {
	uint64_t tsctmp;
	uint64_t tsctmpn;
	tsctmp = x86_get_tsc();
	sleep(1);
	tsctmpn = x86_get_tsc();
	if (tsctmp < tsctmpn)
		return tsctmpn - tsctmp;
	else
		return -1;
}
