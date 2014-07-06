#ifndef TIME_HEADER
#define SYS_TIME_HEADER

#include <sys/types.h>

extern volatile uint64_t jiffies;

void sleep(uint8_t sec);

#endif /* TIME_HEADER */


