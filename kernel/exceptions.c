#include <arch/exceptions.h>
#include <arch/exceptions_api.h>
#include <sys/types.h>

void __exception_zero_divide() {

	Exception_ZeroDivide();

	while (1) {
	/* hlt */
	}
}

void __exception_general_protection_fault(uint32_t selector) {

	Exception_GProtFault(selector);

	while (1) {
	/* hlt */
	}
}

void __exception_general_page_fault(uint32_t code) {

	Exception_GPageFault(code);

	while (1) {
	/* hlt */
	}
}

