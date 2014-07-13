#include <arch/idt.h>
#include <arch/color.h>

#include <sys/io.h>
#include <sys/fs.h>
#include <sys/gdt.h>
#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/time.h>
#include <sys/debug.h>
#include <sys/stdlib.h>
#include <sys/keyboard.h>

int kernel_start() {

	sys_tty_clear_screen();
	sys_tty_set_xy(0, 0);

	sys_tty_set_attr(FOREGROUND_WHITE, BACKGROUND_BLACK);
	DBG_PRINT("Kernel started!\n");

	sys_gdt_init();
	sys_idt_init();
	sys_keyboard_init();
	sys_kmem_table_init();
	sys_ata_init();
	sys_afs_init();

	char abc[20] = {0};

lab:

	gets(abc);
	int node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, abc);
	printf("-> %d\n", node);

goto lab;

	while(1) {
	}
}
