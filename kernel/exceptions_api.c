#include <arch/exceptions_api.h>
#include <sys/stdlib.h>
#include <sys/stdlib.h>

void Exception_ZeroDivide() {
	printf("ZERO DIVIDE!\n");
}

void Exception_GProtFault(uint32_t selector) {
	printf("GENERAL PROTECTION FAULT!\n");
}

void Exception_GPageFault(uint32_t code) {
	printf("GENERAL PAGE FAULT!\n");
}
