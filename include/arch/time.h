#ifndef TIME_HEADER
#define TIME_HEADER

#include <sys/types.h>

extern volatile uint64_t jiffies;

uint64_t x86_get_tsc();

#endif /* TIME_HEADER */
