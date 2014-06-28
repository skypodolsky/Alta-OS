#include <sys/types.h>
#include <sys/debug.h>
#include <sys/keyboard.h>

char key_buf[MAX_KEYBUF];
volatile uint8_t key_buf_tail;
volatile uint8_t key_buf_head;

char low_charset[MAX_CHARSET];
char big_charset[MAX_CHARSET];

void sys_keyboard_init() {
	sys_keyboard_charset_init();
	DBG_PRINT(KERN_INFO"Charset initialized\n");
}

void sys_keyboard_handler(uint8_t scan_code) {

	if (key_buf_tail >= MAX_KEYBUF)
		key_buf_tail = 0;

	key_buf_tail++;
	key_buf[key_buf_tail - 1] = scan_code;

}

uint8_t sys_keyboard_get_scan_code() {

	uint8_t scan_code;
	if ( key_buf_head != key_buf_tail ) {
		if ( key_buf_head >= MAX_KEYBUF ) {
			key_buf_head = 0;
		}

		scan_code = key_buf[key_buf_head];
		key_buf_head++;

		return scan_code;

	} else
		return 0;

}

char sys_keyboard_read_char() {

	char chr = 0;
	uint8_t scan = 0;
	static bool shift_on;

WAIT_MORE:
	do {
		scan = sys_keyboard_get_scan_code();

		if (scan & 0x80) {
			if (scan == 0x2A + 0x80) {
				shift_on = 0;
				scan = 0;
			}
			else
				scan = 0;
		}

	} while (!scan);

	if (scan != 0) {
		if (scan == 0x2A) {
			shift_on = 1;
			goto WAIT_MORE;
		}

		if (shift_on)
			chr = big_charset[scan];
		else
			chr = low_charset[scan];
	}

	return chr;
}

void sys_keyboard_charset_init() {

/* Low charset */

	uint8_t i = 0;
	for ( i = 0; i < MAX_CHARSET - 1; i++ ) {
		low_charset[i] = 0;
		big_charset[i] = 0;
	}

    low_charset[1]=27;		low_charset[15]=9;		low_charset[30]='a';	low_charset[44]='z';
    low_charset[2]='1';		low_charset[16]='q';	low_charset[31]='s';	low_charset[45]='x';
    low_charset[3]='2';		low_charset[17]='w';	low_charset[32]='d';	low_charset[46]='c';
    low_charset[4]='3';		low_charset[18]='e';	low_charset[33]='f';	low_charset[47]='v';
    low_charset[5]='4';		low_charset[19]='r';	low_charset[34]='g';	low_charset[48]='b';
    low_charset[6]='5';		low_charset[20]='t';	low_charset[35]='h';	low_charset[49]='n';
    low_charset[7]='6';		low_charset[21]='y';	low_charset[36]='j';	low_charset[50]='m';
    low_charset[8]='7';		low_charset[22]='u';	low_charset[37]='k';	low_charset[51]=',';
    low_charset[9]='8';		low_charset[23]='i';	low_charset[38]='l';	low_charset[52]='.';
    low_charset[10]='9';	low_charset[24]='o';	low_charset[39]=';';	low_charset[53]='/';
    low_charset[11]='0';	low_charset[25]='p';	low_charset[40]='\'';	low_charset[57]=' ';
    low_charset[12]='-';	low_charset[26]='[';
    low_charset[13]='=';	low_charset[27]=']';
    low_charset[14]=8;		low_charset[28]=10;

/* Big charset */

    big_charset[1]=27;		big_charset[15]=9;		big_charset[30]='A';	big_charset[44]='Z';
    big_charset[2]='!';		big_charset[16]='Q';	big_charset[31]='S';	big_charset[45]='X';
    big_charset[3]='@';		big_charset[17]='W';	big_charset[32]='D';	big_charset[46]='C';
    big_charset[4]='#';		big_charset[18]='E';	big_charset[33]='F';	big_charset[47]='V';
    big_charset[5]='$';		big_charset[19]='R';	big_charset[34]='G';	big_charset[48]='B';
    big_charset[6]='%';		big_charset[20]='T';	big_charset[35]='H';	big_charset[49]='N';
    big_charset[7]='^';		big_charset[21]='Y';	big_charset[36]='J';	big_charset[50]='M';
    big_charset[8]='&';		big_charset[22]='U';	big_charset[37]='K';	big_charset[51]='<';
    big_charset[9]='*';		big_charset[23]='I';	big_charset[38]='L';	big_charset[52]='>';
    big_charset[10]='(';	big_charset[24]='O';	big_charset[39]=':';	big_charset[53]='?';
    big_charset[11]=')';	big_charset[25]='P';	big_charset[40]='"';	big_charset[57]=' ';
    big_charset[12]='_';	big_charset[26]='{';
    big_charset[13]='+';	big_charset[27]='}';
    big_charset[14]=8;		big_charset[28]=10;

}
