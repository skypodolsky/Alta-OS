#ifndef FILE_HEADER
#define FILE_HEADER

#include <sys/types.h>

typedef struct AFS_FILE {

    uint64_t 	f_LBA;			/* LBA address of this part of file */
    uint16_t 	f_pos;			/* position in the file */
    uint16_t 	f_start_index;	/* index of the first file's part */
    uint16_t 	f_curr_index;	/* current index of file's part */
    uint32_t 	f_size;			/* whole file size */
    uint8_t	   *f_buf;			/* file buffer window */
    uint8_t 	f_mode;			/* operation mode */
} FILE;

FILE *fopen(const char *name, const char *mode);
int fclose(FILE *);

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *fd);

#endif	/* FILE_HEADER */
