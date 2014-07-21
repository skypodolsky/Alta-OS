#include <sys/fs.h>
#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/types.h>
#include <sys/debug.h>
#include <sys/stdlib.h>

#define AFS_SECTORS_TO_READ (((AFS_MAX_FILES * sizeof(struct file_desc)) / 512) + 1)


/******************************************************************************/
/********************************** GLOBALS ***********************************/

uint32_t *node_cache = NULL;		/* nodes cache */
struct table_iterator *iterator;

/******************************************************************************/



/******************************************************************************/
/******************************* Static functions *****************************/
static void afs_cache_init();
static void afs_cache_mark(uint32_t index, uint32_t state);
static uint32_t afs_cache_search_valid();

/******************************************************************************/



static void afs_cache_init() {
	uint32_t i = 0;
	node_cache = kvalloc(AFS_MAX_FILES * sizeof(uint32_t));
	if (node_cache) {
	
		for (i = 0; i < AFS_MAX_FILES; i++)
			node_cache[i] = AFS_CACHE_NODE_FREE;

		DBG_PRINT(KERN_INFO"Node cache initialized\n");
	}
}

static void afs_cache_mark(uint32_t index, uint32_t state) {
	node_cache[index] = state;
}

static uint32_t afs_cache_search_valid() {
	uint32_t node;
	for (node = 0; node < AFS_MAX_FILES; node++) {
		if (node_cache[node] == AFS_CACHE_NODE_FREE)
			return node;
	}
	return AFS_CACHE_NODE_LOCK;
}

/******************************************************************************/
/*************************** External AFS functions ***************************/
/******************************************************************************/

void sys_afs_init() {
	uint32_t rw_res = 0;	/* read/write operation result */
	uint32_t node = 0;		/* current node */

	struct file_desc node_tmp;

	afs_cache_init();	/* cache initialization */

	iterator->ptr = (uint8_t *)kvalloc(AFS_MAX_FILES * sizeof(struct file_desc));	/* table storage initialization */
		if (iterator->ptr) {
			iterator->old_ptr = iterator->ptr;
			rw_res = sys_ata_read_sectors(0, AFS_SECTORS_TO_READ, AFS_TABLE_ROOT, iterator->ptr);
			if (rw_res == ATA_SUCCESS) {
				/* updating cache table */
				do {

					memcpy(&node_tmp, iterator->ptr, sizeof(struct file_desc));
					if  ((node_tmp.attr & AFS_NODE_TYPE_ISEXIST)) {
						afs_cache_mark(node, AFS_CACHE_NODE_LOCK);
						node++;
					} else {
						break;	/* FIXME: BUG! */
					}

					iterator->ptr += sizeof(struct file_desc);
				} while (node != AFS_MAX_FILES);
				
				iterator->ptr = iterator->old_ptr;	/* rewinding it back */

				DBG_PRINT(KERN_INFO"File system initialized\n");
			}
		}
}

void sys_afs_close() {
	kvfree(iterator->ptr);
}

uint32_t sys_afs_table_add_node(uint8_t sys_byte, uint32_t parent, uint32_t child, uint16_t attr, const char *name) {

	uint32_t node = 0;					/* node index */
	uint8_t rw_res = ATA_UNDEF;			/* read/write operation status */

	struct file_desc node_tmp;		/* temporary node for filling */

	if (iterator->ptr != NULL) {

		node = afs_cache_search_valid();
		if (node != AFS_CACHE_NODE_LOCK) {
			printf("cache node found: %d\n", node);
			iterator->ptr += (node * sizeof(struct file_desc));
			afs_cache_mark(node, AFS_CACHE_NODE_LOCK);
		} else {
			node = AFS_NODE_INVALID;
			iterator->ptr = iterator->old_ptr;
			goto exit;
		}

		memset(&node_tmp, 0, sizeof(struct file_desc));

		/* initializing node info */
		node_tmp.sys_byte = sys_byte;
		node_tmp.LBA = AFS_TABLE_LBA + node;
		node_tmp.parent_id = parent;
		node_tmp.child_id = child;
		node_tmp.attr = attr;
		strcpy(node_tmp.name, name);

		/* replacing free place */
		memcpy(iterator->ptr, &node_tmp, sizeof(struct file_desc));

		/* TODO: add actual disk to read */
		rw_res = sys_ata_write_sectors(0, 2, AFS_TABLE_ROOT + ((node * sizeof(struct file_desc)) / 512), iterator->old_ptr + ((node * sizeof(struct file_desc)) / 512) * 512);

		/* an error has been occured */
		if (rw_res != ATA_SUCCESS)
			node = AFS_NODE_INVALID;
	}

	iterator->ptr = iterator->old_ptr;

exit:
	if (node != AFS_NODE_INVALID)
		return node;
	else 
		return AFS_NODE_INVALID;
	}

uint32_t sys_afs_table_del_node(uint32_t node) {
	uint8_t rw_res = 0;
	uint32_t status = node;
	struct file_desc node_tmp;

	if (iterator->ptr != NULL) {
		iterator->ptr += node * sizeof(struct file_desc);
		memcpy(&node_tmp, iterator->ptr, sizeof(struct file_desc));
		node_tmp.sys_byte = 0x00;
		memcpy(iterator->ptr, &node_tmp, sizeof(struct file_desc));
		rw_res = sys_ata_write_sectors(0, 2, AFS_TABLE_ROOT + ((node * sizeof(struct file_desc)) / 512), iterator->old_ptr + ((node * sizeof(struct file_desc)) / 512) * 512);
		afs_cache_mark(node, AFS_CACHE_NODE_FREE);

		if (rw_res != ATA_SUCCESS)
			status = AFS_NODE_INVALID;

		iterator->ptr = iterator->old_ptr;
	} else {
		status = AFS_NODE_INVALID;
	}
	return status;
}

uint32_t sys_get_node_by_name(const char *name, uint32_t parent) {

	uint32_t node = 0;
	uint32_t res = AFS_NODE_INVALID;

	struct file_desc node_tmp;

	do {
		memcpy(&node_tmp, iterator->ptr, sizeof(struct file_desc));
		if (node_tmp.parent_id == parent) {
			if (!strcmp(node_tmp.name, name)) {
				res = node;
				goto exit;
			}
		}
		iterator->ptr += sizeof(struct file_desc);
		node++;
	} while(node < AFS_MAX_FILES);

exit:
	iterator->ptr = iterator->old_ptr;
	return res;
}

uint32_t sys_get_node_by_index(uint32_t index, struct file_desc *node_out) {

	if (index < AFS_MAX_FILES) {
		iterator->ptr = iterator->old_ptr + (sizeof(struct file_desc) * index);
		memcpy(node_out, iterator->ptr, sizeof(struct file_desc));

		iterator->ptr = iterator->old_ptr;

		return index;
	} else {
		return AFS_NODE_INVALID;
	}
}

uint32_t sys_afs_table_update_entry(uint32_t index, struct file_desc *node) {

	uint32_t state = AFS_NODE_INVALID;
	uint8_t rw_res = 0;

	iterator->ptr = iterator->old_ptr + (index * sizeof(struct file_desc));
	memcpy(iterator->ptr, node, sizeof(struct file_desc));
	rw_res = sys_ata_write_sectors(0, 2, AFS_TABLE_ROOT + ((index * sizeof(struct file_desc)) / 512), iterator->old_ptr + ((index * sizeof(struct file_desc)) / 512) * 512);
	if (rw_res == ATA_SUCCESS)
		state = index;

	iterator->ptr = iterator->old_ptr;

	return state;
}
