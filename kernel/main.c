#include <arch/idt.h>
#include <arch/time.h>
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

	uint32_t node;

	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "123");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "456");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "789");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "ASDASF");
	node = sys_afs_table_del_node(node);
	printf("del : %x\n", node);
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "stanislav");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "BURZUM");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, 0, AFS_NODE_ATTR_ISFILE, "AGGA");

	while(1) {
	printf("\nCPU FREQUENCY: %d", get_cpu_freq() );
	sleep(1);
	}
}
