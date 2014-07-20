/************************************************
 *     		     Alta OS    		*
 *	Header for the standard library    	*
 *    						*
 *		Stanislav Podolsky    		*
 *    						*
 ************************************************/

#ifndef STDLIB_HEADER
#define STDLIB_HEADER

#include <sys/types.h>

#define outportb(port, value) asm volatile ("outb %b0, %w1"::"a"(value),"d"(port));
#define outportw(port, value) asm volatile ("outw %w0, %w1"::"a"(value),"d"(port));
#define outportl(port, value) asm volatile ("outl %0, %w1"::"a"(value),"d"(port));

#define inportb(port, out_value) asm volatile ("inb %w1, %b0":"=a"(out_value):"d"(port));
#define inportw(port, out_value) asm volatile ("inw %w1, %w0":"=a"(out_value):"d"(port));
#define inportl(port, out_value) asm volatile ("inl %w1, %0":"=a"(out_value):"d"(port));

#define MAX_NUM_DIGITS 32

/* print functions */

void 	printf 	( char* , ... );
char* 	itoa   	( char* , uint8_t, size_t, uint8_t  );

/* string functions */

void 	strcpy 	( char*, const char* );
void 	strcat 	( char*, const char* );
int 	strcmp 	( const char*, const char* );
int 	strlen 	( const char* );
char* 	strtok 	( char*, const char* );
const char*	strchr	( const char*, int );
char*	gets	( char* );

/* memory functions */

void 	memset 	( void*, int, uint32_t ); 
void 	memcpy 	( void*, void*, uint32_t );
int 	memcmp 	( void*, void*, uint32_t );

#endif
