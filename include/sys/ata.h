#ifndef ATA_HEADER
#define ATA_HEADER

#include <sys/types.h>

/* ATA return values */

#define ATA_SUCCESS 0x00
#define ATA_NEXIST	0xFF
#define ATA_UNDEF	0xCF
#define ATA_NOHNDL	0xDF

/* ATA commands */

#define ATA_DATA	0x00
#define ATA_ERROR	0x01
#define ATA_SECTOR	0x02
#define ATA_LBA_L	0x03
#define ATA_LBA_M	0x04
#define ATA_LBA_H	0x05
#define ATA_DRIVE	0x06
#define ATA_STATUS	0x07
#define ATA_CMD		0x07

/* ATA status register */

#define ATA_BSY		0x80
#define ATA_DRDY	0x40
#define ATA_DF		0x20
#define ATA_DSC		0x10
#define ATA_DRQ		0x08
#define ATA_CORR	0x04
#define ATA_IDX		0x02
#define ATA_ERR		0x01

/* ATA error register */
#define ATA_ERR_BBK		0x80
#define ATA_ERR_UNC		0x40
#define ATA_ERR_MC		0x20
#define ATA_ERR_IDNF	0x10
#define ATA_ERR_MCR		0x08
#define ATA_ERR_ABRT	0x04
#define ATA_ERR_TK0NF	0x02
#define ATA_ERR_AMNF	0x01

#define IDE_TYPE_INVALID	0xFF
#define IDE_TYPE_ATA		0x00
#define IDE_TYPE_ATAPI		0x01

#define ATA_MAX_OPERATIONS	0xFFFF
#define ATA_MAX_IDE_NUM		4

struct ide_dev_ops {
	uint8_t (*ata_read)(uint8_t ata_dev_index, uint16_t sectors_count, uint64_t base_sector, uint8_t* buf);
	uint8_t (*ata_write)(uint8_t ata_dev_index, uint16_t sectors_count, uint64_t base_sector, uint8_t* buf);
	void (*ata_reset)(uint8_t ata_dev_index);
};

typedef struct IDE_DEVICE {
	uint8_t drive;		/* drive number (0xA0/0xB0) -> primary/slave */
	uint16_t channel;	/* IDE slot. 0x1f0-0x1f7/0x170-0x177 */
	uint16_t cfg_reg;	/* configuration register */
	uint8_t type;		/* ATA/ATAPI (1/0) */
	struct ide_dev_ops *dev_ops;
} ide_device;

/********************************************************************************/
/*                        external functions                                    */
/********************************************************************************/

void sys_ata_init();				/* initialization of ATA driver*/
uint8_t sys_ata_read_sectors(uint8_t index, uint16_t sectors, uint64_t base_addr, uint8_t *buf);	/* read multiple sectors */
uint8_t sys_ata_write_sectors(uint8_t index, uint16_t sectors, uint64_t base_addr, uint8_t *buf);	/* write multiple sectors */

#endif	/* ATA_HEADER */

