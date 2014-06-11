#ifndef EXCEPTIONS_API_HEADER
#define EXCEPTIONS_API_HEADER

#include <sys/types.h>

void Exception_ZeroDivide();
void Exception_GProtFault(uint32_t);
void Exception_GPageFault(uint32_t);

#endif /* EXCEPTIONS_API_HEADER */
