#include <sys/math.h>
#include <sys/types.h>
#include <sys/stdlib.h>

int ceil_to_val(int val, int num) {
	return (val % num != 0) ? ((val / num) + 1) * num : (val / num) * num;
}

