#ifndef MEMORY_HEADER
#define MEMORY_HEADER

#include <sys/types.h>

#define PAGE_SIZE 0x1000
#define SYS_PAGE_OFFSET 0xC00

#define VIRTUAL_OFFSET 0x8000
#define KERNEL_PDE_PHYS 0x28000
#define MEM_HEAP_START_PHYS 0xC08000

#define MEM_CTRL_BASE 0x600000

#define PAGE_ALLOC 0x80
#define PAGE_DPL0 0x00
#define PAGE_DPL1 0x20
#define PAGE_DPL2 0x40
#define PAGE_DPL3 0x60
#define PAGE_DENY 0x01

void sys_kmem_table_init();								/* memory manager initialization */

uint8_t	kmap_page( uint16_t, uint16_t, uint8_t );	/* mounts page specified by the PDE and PTE parameters */
void	kdel_page( uint16_t, uint16_t );			/* unmounts page specified by the PDE and PTE parameters */

void *kmalloc( uint32_t, uint8_t );	/* lvl 1 allocator */
void *kvalloc( uint32_t );	/* lvl 2 allocator */

void kfree( void* );
void kvfree( void* );

#endif /* MEMORY_HEADER */
