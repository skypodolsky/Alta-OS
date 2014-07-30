#include <sys/fs.h>
#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/file.h>
#include <sys/math.h>
#include <sys/types.h>
#include <sys/stdlib.h>

/* --------------------------------- *
########### Operation mode ###########
======================================

+------------------------------------------------+
| Rsvd | Rsvd | Rsvd | Rsvd | Rsvd | T/B | W | R |
+------------------------------------------------+

'Rsvd' - reserved
'T/B' - 0/1, text or binary
'W' - write operation mode
'R' - read operation mode

* ----------------------------------*/

#define FILE_MODE_READ	0x01
#define FILE_MODE_WRITE	0x02
#define FILE_MODE_TB	0x04

#define EOF				-1

static uint8_t file_mode_to_bit_field(const char *mode) {
	
	uint8_t res = 0;

	if (!strlen(mode))
		return 0xFF;

	if (strchr(mode, 'r')) {
		/* read operation is enable */
		res |= FILE_MODE_READ;
	}
	
	if (strchr(mode, 'w')) {
		/* write operation is enable */
		res |= FILE_MODE_WRITE;
	}

	if (strchr(mode, 'b')) {
		/* binary file mode */
		res |= FILE_MODE_TB;
	} else {
		/* text file mode */
		res &= ~FILE_MODE_TB;
	}

	return res;
}

FILE *fopen(const char *name, const char *mode) {
	uint32_t f_node_idx = 0;
	uint8_t f_mode = 0;

	FILE *file = NULL;
	struct file_desc node;

	/* FIXME: add current dir */
	f_mode = file_mode_to_bit_field(mode);
	f_node_idx = sys_get_node_by_name(name, 0);

	/* if file DOES exist and specified mode is correct */
	if (f_node_idx != AFS_NODE_INVALID && f_mode != 0xFF) {
		/* sanity checking succeeded*/
		if (sys_get_node_by_index(f_node_idx, &node) != AFS_NODE_INVALID) {

			file = (FILE *)kvalloc(sizeof(FILE));
			if (file) {
				file->f_pos			= 0;
				file->f_LBA			= node.LBA;
				file->f_start_index = f_node_idx;
				file->f_curr_index	= f_node_idx;
				file->f_size		= node.size;
				file->f_mode		= f_mode;
			} else {
				return NULL;
			}
		} else {
			return NULL;
		}
	}
	else if (f_mode & FILE_MODE_WRITE) {
		/* FIXME: add parent */
		f_node_idx = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, name);
		sys_get_node_by_index(f_node_idx, &node);

		file = (FILE *)kvalloc(sizeof(FILE));
		if (file) {
			file->f_pos			= 0;
			file->f_LBA			= node.LBA;
			file->f_start_index = f_node_idx;
			file->f_curr_index	= f_node_idx;
			file->f_size		= 0;
			file->f_mode		= f_mode;
		} else {
			return NULL;
		}
	} else {
		/* none of that cases */
		return NULL;
	}

	return file;
}

int fclose(FILE *fd) {

	uint32_t i				= 0;
	uint32_t f_index		= fd->f_start_index;
	uint32_t f_tmp_index	= 0;
	uint32_t sectors		= fd->f_size / 512;

	struct file_desc node;

	if ( !(fd->f_mode & FILE_MODE_WRITE))
		return 0;

	do {

		/* get part of node */
		if (sys_get_node_by_index(f_index, &node) != AFS_NODE_INVALID) {

			/* FIXME: add appropriate drive index */
			sys_ata_write_sectors(0, 1, node.LBA, fd->f_buf + (i * 512));
			fd->f_size -= 512;

			/* if we have size for saving yet and current child does NOT has fragmented part */
			if ( (fd->f_size > 0) && (node.child_id == AFS_NODE_NOCHILD) ) {

				/* creating a child with its parent */
				f_tmp_index = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST | AFS_NODE_TYPE_ISFRAGM,	
						f_index, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "");

				if (f_tmp_index == AFS_NODE_INVALID) {
					/* sanity check */
					return EOF;
				}

				node.child_id = f_tmp_index;
				/* updating the parent node */
				sys_afs_table_update_entry(f_index, &node);
				f_index = f_tmp_index;
			} else {
				f_index = node.child_id;
			}
		}

		i++;
	} while (i < sectors);

	/* should work, but not tested */
	/* deleting another (unused) fragmented parts */
	f_tmp_index = node.child_id;

	while (f_tmp_index != AFS_NODE_NOCHILD) {
		f_tmp_index = node.child_id;
		sys_get_node_by_index(f_tmp_index, &node);
		sys_afs_table_del_node(f_tmp_index);
	}

	kvfree(fd->f_buf);

	printf("fclose succeeded!\n");

	return 0;
}

size_t fwrite(const void *ptr, size_t size,	size_t count, FILE *fd) {

	size_t i			= 0;
	size_t byte_size	= 0;
	
	if (!(fd->f_mode & FILE_MODE_WRITE))
		return 0;

	byte_size = ceil_to_val(count * size, 512);

	fd->f_buf = kvalloc(byte_size);
	if (fd->f_buf) {

		for (i = 0; i < count * size; i++) {
			*(fd->f_buf + i) = *(uint8_t *)ptr;
			ptr++;
		}

		fd->f_size = byte_size;

		return i;

	} else {
		return -1;
	}
}

size_t fread(void *ptr, size_t size, size_t count, FILE *fd) {

	size_t offset		= 0;
	size_t byte_size	= 0;

	uint8_t tmp_buf[512];
	uint32_t f_tmp_index = 0;

	struct file_desc node_tmp;

	if (!fd->f_mode & FILE_MODE_READ)
		return 0;

	byte_size = ceil_to_val(count * size, 512);
	fd->f_buf = kvalloc(byte_size);
	if (fd->f_buf) {
		f_tmp_index = fd->f_start_index;
		do {
			sys_get_node_by_index(f_tmp_index, &node_tmp);
			sys_ata_read_sectors(0, 1, node_tmp.LBA, tmp_buf);

			if (byte_size - offset >= 512)
				memcpy(ptr + offset, tmp_buf, 512);
			else
				memcpy(ptr + offset, tmp_buf, byte_size - offset);

			f_tmp_index = node_tmp.child_id;
			offset += 512;
		} while(node_tmp.child_id != AFS_NODE_NOCHILD);

		return byte_size;
	} else {
		return -1;
	}
}
