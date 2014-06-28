#ifndef GDT_HEADER
#define GDT_HEADER

#include <sys/types.h>

#define MAX_GDT_ENTRIES 5

/* GDT descriptor structure */
typedef struct GDT_desc {
	uint8_t limit_third;
	uint8_t limit_second;
	uint8_t base_fourth;
	uint8_t base_third;
	uint8_t base_second;
	uint8_t flags;
	uint8_t gran_lim;
	uint8_t base_first;
} __attribute__((packed)) gdt_desc;

/* GDTR register structure */
typedef struct GDT_reg_val {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_reg_val;

/* GDT functions */
void sys_gdt_init();
void sys_gdt_seg_reg_reload();
void sys_gdt_get_value(uint32_t);

#endif	/* GDT_HEADER */
