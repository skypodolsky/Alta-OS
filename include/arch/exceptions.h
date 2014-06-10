#ifndef EXCEPTIONS_HEADER
#define EXCEPTIONS_HEADER

#include <sys/types.h>

void __exception_zero_divide();
void __exception_general_page_fault(uint32_t code);
void __exception_general_protection_fault(uint32_t selector);

#endif	/* EXCEPTIONS_HEADER */
