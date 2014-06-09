#include <sys/gdt.h>
#include <sys/stdlib.h>

static gdt_desc sys_gdt_desc[MAX_GDT_ENTRIES];
static gdt_reg_val sys_gdt_reg_val;

/* creates predefined GDT entry */
void gdt_set_predefined_entry(uint16_t index, uint8_t flags) {

	sys_gdt_desc[index].limit_third 	= 0xFF;
	sys_gdt_desc[index].limit_second 	= 0xFF;
	sys_gdt_desc[index].flags			= flags;
	sys_gdt_desc[index].gran_lim 		= 0xCF;
	sys_gdt_desc[index].base_fourth		= 0x00;
	sys_gdt_desc[index].base_third 		= 0x00;
	sys_gdt_desc[index].base_second 	= 0x00;
	sys_gdt_desc[index].base_first 		= 0x00;

}

/* Initializes all segments */
void gdt_init() {

	memset(&sys_gdt_desc[0], 0, sizeof(gdt_desc));	/* 'dummy' segment */
	gdt_set_predefined_entry(1, 0x9A);				/* code segment */
	gdt_set_predefined_entry(2, 0x92);				/* data segment */
	gdt_set_predefined_entry(3, 0x96);				/* stack segment */

	sys_gdt_reg_val.limit = 8291;
	sys_gdt_reg_val.base = (uint32_t)&sys_gdt_desc[0];

	asm volatile ("lgdt (,%0,)"::"a"(&sys_gdt_reg_val));

	gdt_seg_reg_reload();

}

void gdt_get_value(uint32_t address) {
    asm volatile ("sgdt (,%0,)"::"a"(address));
}

/****************************************************************************************
*    gdt_seg_reg_reload - reloads segment registers after the GDT has been reloaded.		*
*	INDEX #0: dummy segment.															*
*	INDEX #1: code segment																*
*	INDEX #2: data segment																*
*	INDEX #3: stack segment																*
*																						*
*	Note: 'es' segment will be set to 'ss' value. I don't remember exactly, why do we	*
*	need this. As I remember, it is due to GCC generated code constructions, which are	*
*	using it.																			*
****************************************************************************************/

void gdt_seg_reg_reload() {

	asm volatile ("mov $16, %eax\n"
		"mov %ds, %eax\n"

		"mov $24, %eax\n"
		"mov %ss, %eax\n"
		"mov %es, %eax");

}
