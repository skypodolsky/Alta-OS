#ifndef IDT_HEADER
#define IDT_HEADER

#include <sys/types.h>

typedef struct IRQ_Desc {

    uint16_t    address_low;
    uint16_t    selector;
    uint8_t     reserved;
    uint8_t     type;
    uint16_t    address_high;

} __attribute__((packed)) irq_desc;

typedef struct IDT_reg_val {
    uint16_t    limit;
    uint32_t    base;
} __attribute__((packed)) idt_reg_val;

#define MAX_IRQ_DESC_NUM 255

void sys_idt_init();
void sys_irq_init();
void sys_exceptions_init();
void sys_irq_set_handler(uint8_t, uint32_t, uint8_t);

#endif	/* IDT_HEADER */
