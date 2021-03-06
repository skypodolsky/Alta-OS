#include <arch/idt.h>
#include <arch/irq.h>
#include <arch/exceptions.h>

#include <sys/types.h>
#include <sys/debug.h>

irq_desc sys_irq_desc[MAX_IRQ_DESC_NUM];
idt_reg_val sys_idt_reg_val;

void sys_idt_init() {

	sys_irq_init();
	sys_exceptions_init();

    sys_idt_reg_val.limit   = (MAX_IRQ_DESC_NUM * 8) - 1;
    sys_idt_reg_val.base    = (uint32_t)&sys_irq_desc[0];

    asm volatile ("lidt (,%0,)"::"a"(&sys_idt_reg_val));    /* loading the interrupt table */

	sys_irq_pic_init();
	DBG_PRINT(KERN_INFO"IDT initialized\n");

	asm("sti");

}

void sys_exceptions_init() {
	sys_irq_set_handler(0, (uint32_t)__exception_zero_divide, 0x8E);
	sys_irq_set_handler(13, (uint32_t)__exception_general_protection_fault, 0x8E);
	sys_irq_set_handler(14, (uint32_t)__exception_general_page_fault, 0x8E);
}

void sys_irq_init() {
	sys_irq_set_handler(0x20, (uint32_t)__irq_timer, 0x8E);
	sys_irq_set_handler(0x21, (uint32_t)__irq_keyboard, 0x8E);
	DBG_PRINT(KERN_INFO"IRQ initialized\n");
}

void sys_irq_set_handler(uint8_t index, uint32_t address, uint8_t type) {

    sys_irq_desc[index].address_low  = address & 0xFFFF;
	sys_irq_desc[index].selector     = 0x8;
    sys_irq_desc[index].reserved     = 0;
    sys_irq_desc[index].type     	 = type;
    sys_irq_desc[index].address_high = address >> 16;

}


