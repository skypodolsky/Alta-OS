/****************************************************************
*			      Alta OS				*
*			Kernel image maker			*
*		      by Stanislav Podolsky			*
*								*
*	Creates image of kernel by a raw kernels binary code	*
*     	Complements kernel with nulls to 512-aliquot size	*
*								*
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define IMAGE_PATH "../image/"

int main(int argc, char* argv[]) {

    uint16_t 		size 	= 0;
    uint16_t 		add	= 0;

    char* 		file_buf 	= 0;
    char* 		data_file 	= 0;
    unsigned char 	zero_val	= 0;

    FILE* file;

/* Main code section */

    if (argc<2)
	{
	    printf("Give me an argument! [kernel binary file]\n");
	    return 1;
	}

    data_file = argv[1];

    file = fopen(data_file, "rb");
	if (!file)
	    {
		printf("File does not exist!\n");
		return 1;
	    }

/* Detecting the size of kernel */

    fseek(file, 0, SEEK_END);
	size = ftell(file);
    fseek(file, 0, SEEK_SET);

/* */

if (size<=512)
    add = 512-size;
else
    add = 512-(size%512);

    file_buf = malloc(size+add);
	fread(file_buf, sizeof(char), size, file);

/* Printing info */

	    printf("Size:\t %d\n",size);
	    printf("Add:\t %d\n", add);
	    printf("CRC:\t %d\n",size+add);

    memset((file_buf+size), zero_val, add);

/* Writing to kernel file */

    FILE* new_f = fopen(IMAGE_PATH"kernel", "wb");
    fwrite(file_buf, 1, size+add, new_f);
    fclose(new_f);

    free(file_buf);

    fclose(file);

    return 0;
}