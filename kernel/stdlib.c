#include <sys/io.h>
#include <sys/types.h>
#include <sys/stdlib.h>
#include <sys/keyboard.h>

int strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' || *str2 != '\0') {
		if (*(str1++) != *(str2++))
			return 1;
    }
    return 0;
}

void strcat(char* dst, const char* src) {
    while (*dst != '\0')
        dst++;

	while (*src != '\0')
	    *dst++=*src++;
}

void strcpy(char* dst, const char* src) {
    while (*src != '\0')
		*(dst++) = *(src++);
}

int strlen(const char* str) {
    int i = 0;
    while (*(str + i) != '\0')
		i++;

	return i;
}

const char *strchr(const char *str, int ch) {
	int i = 0;
	while (*(str + i) != '\0') {
		if (*(str + i) == ch)
			return str + i;
		i++;
	}
	return NULL;
}

char* strtok(char* str, const char* delim) {

	static char* last;

	if (!delim)
		return 0;

	int i = 0;
	int j = 0;

	if (!str) {
		str = last;
		if ((str + strlen(str)) == last)
			return 0;
	}

	for (i = 0; i < strlen(str); i++) {
		for (j = 0; j < strlen(delim); j++)
			if (str[i] == delim[j]) {
				str[i] = 0;
				last = str + i + 1;
				return str;
			}
	}

	last=str + i;

	return str;
}

void memcpy(void* dst, void* src, uint32_t num) {
	uint32_t counter = 0;
	while (counter!=num) {
		*(char*)(dst++) = *(char*)(src++);
		counter++;
	}
}

void memset(void* dst, int val, uint32_t num) {
	uint32_t counter = 0;
	while (counter!=num) {
		*(char*)(dst++) = val;
		counter++;
	}
}

int memcmp(void* dst, void* src, uint32_t num) {
	uint32_t counter = 0;
	while (counter!= num) {
		if (*(char*)(dst++) != *(char*)(src++))
			return 1;
		counter++;
	}
	return 0;
}

char* itoa(char* num_buffer, uint8_t sz_buf, size_t value, uint8_t base) {

	if (base < 2 || base > 16)
		return 0;

	const char digits[] = "0123456789ABCDEF";
	uint8_t i = sz_buf-1;
	uint8_t j = 0;

	num_buffer[i--] = '\0';

	do {
		num_buffer[i--] = digits[value % base];
		value = value / base;
	} while (value);

	for ( ; j < sz_buf - i; j++)
		num_buffer[j] = num_buffer[j + i + 1];

	return &num_buffer[0];
}

int getchar(void) {
	return (int)sys_keyboard_read_char();
}

char *gets(char *s) {
	int i = 0;
	int ch = getchar();

	while(ch != '\n') {
		s[i++] = ch;
		printf("%c", ch);
		ch = getchar();
	}

	s[i++] = '\0';

	return s;
}

int sprintf(char *s, const char *fmt, ...) {

	int i			= 0;	/* format symbol counter */
	int counter     = 0;	/* bytes, that would be written */
	int num_param	= 1;	/* parameter number */
	char byte		= 0;	/* '%' / '\' or smth else*/
	char type		= 0;	/* 'd' / 's' / 'x' / 'b' / 'c' or smth else */
	char num_buf[MAX_NUM_DIGITS] = {0};	/* buffer for convertation */
	void* ptr 		= &fmt;	/* first parameter's address */

	for (i = 0; i < strlen(fmt); i++) {

		byte = *(fmt + i);
		type = *(fmt + i + 1);

		switch (byte) {
			case '%':
				switch (type) {
					case 's':
						strcpy(s + strlen(s), *(char**)(ptr + (num_param++ * 4)));
						i++;
						continue;
						break;
					case 'd':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 10);
						strcpy(s + strlen(s), num_buf);
						i++;
						continue;
						break;
					case 'x':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 16);
						strcpy(s + strlen(s), num_buf);
						i++;
						continue;
						break;
					case 'b':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 2);
						strcpy(s + strlen(s), num_buf);
						i++;
						continue;
						break;
					case 'c':
						s[strlen(s)] = *(char*)(ptr + (num_param++ * 4));
						i++;
						continue;
						break;

					default:
						s[strlen(s)] = byte;
						break;
				}
				break;

			default:
				s[strlen(s)] = byte;
				break;
		}

	}

	s[strlen(s)] = 0;	/* appending NULL-terminator in the end */
	counter = strlen(s);
	return (counter) ? counter : -1;
}

void printf(char* fmt, ...) {

    int i			= 0;
    int num_param	= 1;
	char byte		= 0;
	char type		= 0;
	char u_str[2]	= {0, 0};
	char num_buf[MAX_NUM_DIGITS] = {0};
    void* ptr 		= &fmt;

	for (i = 0; i < strlen(fmt); i++) {

		byte = *(fmt + i);
		type = *(fmt + i + 1);

		switch (byte) {
			case '%':
				switch (type) {
					case 's':
						sys_print(*(char**)(ptr + (num_param++ * 4)));
						i++;
						continue;
						break;

					case 'd':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 10);
						sys_print(num_buf);
						i++;
						continue;
						break;
					case 'x':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 16);
						sys_print(num_buf);
						i++;
						continue;
						break;
					case 'b':
						itoa(num_buf, sizeof(num_buf), *(int*)(ptr + (num_param++ * 4)), 2);
						sys_print(num_buf);
						i++;
						continue;
						break;
					case 'c':
						u_str[0] = *(char*)(ptr + (num_param++ * 4));
						sys_print(u_str);
						i++;
					break;

					default:
						u_str[0] = byte;
						sys_print(u_str);
						break;
				}
				break;
			default:
				u_str[0] = byte;
				sys_print(u_str);
				break;
		}

	}

}
