/************************************************
* Alta OS *
* Memory manager implementation *
* *
* Stanislav Podolsky *
* *
************************************************/

#include <sys/mem.h>
#include <sys/stdlib.h>
#include <sys/errors.h>

/*************************************************************
  Function 'virtual_represent_by_table'
  representation of virtual address by the PDE and PTE
 *************************************************************/

size_t* virtual_represent_by_table(uint16_t pde, uint16_t pte, uint16_t offset) {
	uint32_t res = ( (pde << 22) | (pte << 12) ) + offset;
	return (size_t*)res;
}

/*************************************************************
  Function 'index_by_vurtual'
  retrieves index of page in the page list by virtual address
 *************************************************************/

uint32_t index_by_virtual(size_t* addr) {
	return ( ( (size_t)addr >> 22 ) * 1024 ) + ( ( (size_t)(addr) & 0x3FF000 ) >> 12 );
}

/*************************************************************
  Function 'kpage_mark'
  marks page in page list with specified index to specified condition
 *************************************************************/

void kpage_mark(uint32_t index, uint8_t cond) {
	uint8_t* mem_ptr = (uint8_t*)(MEM_CTRL_BASE + index);
	*mem_ptr = cond;
}

/*************************************************************
  Function 'kmap page'
  mounts specified page
 *************************************************************/

uint8_t kmap_page(uint16_t pde, uint16_t pte, uint8_t flags) {

	if ( (pde > 1023) || (pte > 1023) )
		return 0;

	uint32_t tmp_addr = (( *(size_t*)(KERNEL_PDE_PHYS - VIRTUAL_OFFSET + (pde * 4)) ) & 0xFFFFF000) - VIRTUAL_OFFSET;
	*(size_t*)(tmp_addr + (pte * 4)) = MEM_HEAP_START_PHYS + (0x1000 * pde) + (pte * 4 * 1024 ) + 19;

	kpage_mark((1024 * pde) + pte, flags); /* PAGE_ALLOC | PAGE_DPL0); */

	return 1;

}

/*************************************************************
  Function 'kdel_page'
  unmounts specified page
 *************************************************************/

void kdel_page(uint16_t pde, uint16_t pte) {

	if ( (pde > 1023) || (pte > 1023) )
		return;

	uint32_t tmp_addr = (( *(size_t*)(KERNEL_PDE_PHYS - VIRTUAL_OFFSET + (pde * 4)) ) & 0xFFFFF000) - VIRTUAL_OFFSET;
	*(size_t*)(tmp_addr + (pte * 4)) = 0;

	kpage_mark((1024 * pde) + pte, 0);

}

/*************************************************************
  Function 'kmem_table_init'
  initializes kernel's code, data and stack, in page list
 *************************************************************/
void kmem_table_init() {

	uint32_t i = 0;

	for ( i = 0; i < 1024 * 3; i++ )	/* pages for code, data and stack */
		kpage_mark(i, PAGE_ALLOC | PAGE_DPL0);

}

/*************************************************************
  Function 'kmalloc'
  allocates specified amount of bytes
 *************************************************************/

void* kmalloc(uint32_t size, uint8_t flags) {

	uint8_t* addr = (uint8_t*)(MEM_CTRL_BASE + SYS_PAGE_OFFSET); /* SYS_PAGE_OFFSET = 1024 * 3 -> kernel + data + stack */
	uint32_t pages = (size / PAGE_SIZE) + 1;
	uint8_t* tmp_addr = addr;
	uint32_t i = 0;

	if ( size == 0 )
		return 0;

SCAN:
	while ( ( *(addr) & 0x80 ) ) {
		addr++;
		if ( addr == (uint8_t*)(MEM_CTRL_BASE + 0x100000) ) /* 1024 * 1024 entries */
			return NULL;
	}

	tmp_addr = addr;
	for ( i = 1; i < pages; i++ ) {
		if ( (*(tmp_addr + i) & 0x80) ) {
			addr = tmp_addr + i;
			goto SCAN;
		}
	}

	uint32_t index = (uint32_t)(addr - MEM_CTRL_BASE);
	uint16_t pde = index / 1024;
	uint16_t pte = index % 1024;

	uint16_t tmp_pde = pde;
	uint16_t tmp_pte = pte;

	for ( i = 0; i < pages; i++ ) {

		if ( (tmp_pte + i) > 1023 ) {
			tmp_pde++;
			tmp_pte=0;
		}

		kmap_page(tmp_pde, tmp_pte + i, flags);

	}

	/* size marker */

	size_t* res = virtual_represent_by_table(pde, pte, 0);
	*(res) = size;

	return res + 1;

}

/*************************************************************
  Function 'kfree'
  deallocates allocated memory by it's pointer
 *************************************************************/

void kfree(void* ptr) {

	uint32_t index = index_by_virtual(ptr - 4);
	uint16_t pde	= index / 1024;
	uint16_t pte = index % 1024;
	uint32_t size = *(uint32_t*)(ptr - 4);

	uint32_t pages = (size / PAGE_SIZE) + 1;
	uint32_t i = 0;

	for ( i = 0; i < pages; i++ ) {
		kdel_page(pde, pte + i);
		if ( (pte + i) > 1023 )
			pde++;
	}

}

void* get_page_start_by_index(uint32_t index) {
	size_t* res = virtual_represent_by_table(index / 1024, index % 1024, 0);
	return (void*)res;
}

void* kvalloc(uint32_t size) {

	uint32_t* res = NULL;
	uint32_t* tmp_32 = NULL;

	if (size == 0)
		return NULL;

	if (size > (4096 - 12) ) {	/* 12 = 4 + 8, lvl1 + lvl2 headers */

		res = kmalloc(size, PAGE_ALLOC | PAGE_DPL0 | PAGE_DENY);	/* allocating a full page */

		if (!res)
			return NULL;

		*res = size;	/* lvl2 marker */
		*(res + 1) = 1;
		return res + 2;
	} else {

		uint8_t* table_addr = (uint8_t*)(MEM_CTRL_BASE + SYS_PAGE_OFFSET);
		while ( table_addr != (uint8_t*)MEM_CTRL_BASE + 0x100000 ) {

			if ( ( (*table_addr) & 0x80) && ( !( (*table_addr) & 0x01 ) ) )	{	/* if table DOES exist */
				uint8_t* page_start = get_page_start_by_index( (uint32_t)(table_addr - MEM_CTRL_BASE) );
				uint8_t* page_tmp = page_start;
				uint16_t free = 4084;
				page_tmp += 4;	/* first header */
				tmp_32 = (uint32_t*)page_tmp;
				while ( (*tmp_32) && (free > 0) ) {

					if ( ( *(tmp_32 + 1) == 0 ) && ( *(tmp_32) >= size ) ) {
						*(tmp_32 + 1) = 1;
						return (tmp_32 + 2);
					}

					free -= ( (*tmp_32) + 8);
					page_tmp += ( (*tmp_32) + 8 );
					tmp_32 = (uint32_t*)page_tmp;
				}

				/* else */

				if ( free >= (size + 8) ) {
					*(tmp_32) = size;
					*((tmp_32 + 1)) = 1;
					return (tmp_32 + 2);
				}
			}
			table_addr++;

		}

		/* now, if it is NO pages, we call 'kmalloc' */

		res = kmalloc(size + 8, PAGE_ALLOC | PAGE_DPL0);	/* allocating a full page */
		*res = size;	/* lvl2 marker */
		*(res + 1) = 1;
		return (res + 2);
	}

	return NULL;
}

void kvfree(void* ptr) {

	uint32_t* tmp_val = (uint32_t*)ptr;
	*(tmp_val - 1) = 0;

	uint8_t* page_start = (uint8_t*)((size_t)ptr & 0xFFFFF000);
	uint8_t* page_tmp = page_start + 4;
	while ( page_tmp < (page_start + 0xFFC) ) {

		uint32_t* tmp_32 = (uint32_t*)page_tmp;
		if ( *(tmp_32 + 1 ) == 1 ) {
			return;
		}

		if (*tmp_32)
			page_tmp += (*tmp_32 + 8);
		else
			break;

	}
	memset(page_start + 4, 0, 0xFFC);
	kfree(page_start + 4);

}
