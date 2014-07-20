#ifndef FS_HEADER
#define FS_HEADER

#include <sys/types.h>
#include <sys/stdlib.h>

#define AFS_MAX_FILE_NAME 32

#define AFS_MAX_FILES	0x1000
#define AFS_TABLE_ROOT	0x80
#define AFS_TABLE_LBA	(AFS_TABLE_ROOT + ((sizeof(struct file_desc) * AFS_MAX_FILES) / 512))

#define AFS_CACHE_NODE_FREE		0x00000000
#define AFS_CACHE_NODE_LOCK		0xFFFFFFFF

#define AFS_NODE_INVALID	0xFFFFFFFF
#define AFS_NODE_NOCHILD	AFS_NODE_INVALID

/* file types */

#define AFS_NODE_TYPE_ISEXIST 0x80
#define AFS_NODE_TYPE_ISFRAGM 0x40

#define AFS_NODE_ATTR_ISFILE	0x80
#define AFS_NODE_ATTR_ISDIR		0x40
#define AFS_NODE_ATTR_ISLNK		0x20

#define AFS_ITERATOR_MAX_BUF	0x20

struct file_desc {

	uint8_t		sys_byte;					/* system parameters */
	uint64_t	LBA;						/* LBA address of node's data*/
	uint32_t	parent_id;					/* parent's ID */
	uint32_t	child_id;					/* fragmented part's file_desc structure */
	uint16_t	attr;						/* node attribute */
	uint32_t	size;						/* node size */
	char		name[AFS_MAX_FILE_NAME];	/* file name */

} __attribute__((packed));

struct table_iterator {
	uint8_t *ptr;
	uint8_t *old_ptr;
};

/******************************************************************************/
/*                               FUNCTIONS                                    */
/******************************************************************************/

void		sys_afs_init();
uint32_t	sys_afs_table_add_node(uint8_t type, uint32_t parent, uint32_t child, uint16_t attr, const char* name);
uint32_t	sys_afs_table_del_node(uint32_t node);
uint32_t 	sys_afs_table_update_entry(uint32_t index, struct file_desc *node_in);

uint32_t 	sys_get_node_by_name(const char *name, uint32_t parent);
void		sys_get_node_by_index(uint32_t index, struct file_desc *node_out);

#endif /* FS_HEADER */
