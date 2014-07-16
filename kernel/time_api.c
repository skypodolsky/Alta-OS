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

