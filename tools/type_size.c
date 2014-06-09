#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("\nSIZE OF HOST SYSTEM TYPES:\n"
	    "CHAR:\t%d\n"
	    "SHORT:\t%d\n"
	    "INT:\t%d\n"
	    "FLOAT:\t%d\n"
	    "DOUBLE:\t%d\n\n", sizeof(char), sizeof(short), sizeof(int), sizeof(float), sizeof(double));
return 0;
}