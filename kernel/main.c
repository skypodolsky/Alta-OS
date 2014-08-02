#include <arch/idt.h>
#include <arch/time.h>
#include <arch/color.h>

#include <sys/io.h>
#include <sys/fs.h>
#include <sys/gdt.h>
#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/file.h>
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

	uint32_t node;
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "123");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "234");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "345");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "456");
	node = sys_afs_table_add_node(AFS_NODE_TYPE_ISEXIST, 0, AFS_NODE_NOCHILD, AFS_NODE_ATTR_ISFILE, "567");
	
	FILE *file = fopen("345", "w");
	char abc[1027];
	for (int i = 0; i < 1027; i++) {
		abc[i] = 0x20;
	}
	abc[1025] = 0x40;
	abc[1026] = 0x10;

	fwrite(abc, 1, 1027, file);
	fclose(file);

	char f[1028];
	fread(f, 1, 1027, file);
	for (int i = 0; i < 1027; i++) {
		printf("%x ", f[i]);
	}

    sys_tty_clear_screen();
	char buff[100];
	snprintf(buff, 100, "TEST1");
	printf(buff);

	while(1) {
	}
}
