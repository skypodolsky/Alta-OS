#ifndef IRQ_HEADER
#define IRQ_HEADER

void irq_pic_init();

void __irq_timer();
void __irq_keyboard();

#define IRQ_HANDLER(name) void name(); void _ ## name(); \
asm(".globl " #name " \n"); \
asm(#name ": pusha \n call _" #name " \n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n iret"); \
void _ ## name()

#endif	/* IRQ_HEADER */
