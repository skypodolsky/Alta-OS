#include <sys/ata.h>
#include <sys/mem.h>
#include <sys/debug.h>
#include <sys/stdlib.h>

volatile ide_device sys_ide_device[ATA_MAX_IDE_NUM];

/********************************************************************************/
/*                        static functions declaration                          */
/********************************************************************************/
static void ata_scan();
static void ide_dev_init(uint8_t index, uint8_t drive, uint16_t channel, uint16_t cfg_reg);

static uint8_t ata_verify(uint8_t drive);

static void ata_20ms_delay(uint8_t index);
static void ata_400ns_delay(uint8_t index);

static void ata_reset_drive(uint8_t drive);
static uint8_t ata_read(uint8_t index, uint64_t lba_addr, uint8_t *buf);
static uint8_t ata_write(uint8_t index, uint64_t lba_addr, uint8_t *buf);
static uint8_t ata_read_sectors(uint8_t index, uint16_t sectors, uint64_t lba_addr, uint8_t *buf);
static uint8_t ata_write_sectors(uint8_t index, uint16_t sectors, uint64_t lba_addr, uint8_t *buf);

/*******************************************************************************/
/*                           External ATA functions                            */
/*******************************************************************************/

/*******************************************************************************
 * Function 'sys_ata_init':                                                    *
 * performs initialization of ATA drives                                       *
 *                                                                             *
 * Parameters: none                                                            *
 *                                                                             *
 * Return value: none                                                          *
 ******************************************************************************/
void sys_ata_init() {
	ide_dev_init(0, 0xA0, 0x1f0, 0x3f6);
	ide_dev_init(1, 0xB0, 0x1f0, 0x3f6);
	ide_dev_init(2, 0xA0, 0x170, 0x376);
	ide_dev_init(3, 0xB0, 0x170, 0x376);

	ata_scan();
}

/*******************************************************************************
 * Function 'sys_ata_read_sectors':                                            *
 * reads multiple sectors by the base LBA address                              *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint16_t count [IN] - amount of sectors to read                 *
 *           - uint64_t lba_addr [IN] - LBA address of sector to read          *
 *           - uint8_t* buf [OUT] - buffer to write                            *
 *                                                                             *
 * Return value: uint8_t ATA read result                                       *
 *******************************************************************************/
uint8_t sys_ata_read_sectors(uint8_t index, uint16_t count, uint64_t base_lba, uint8_t *buf) {
	if (sys_ide_device[index].dev_ops->ata_read != NULL)
		return sys_ide_device[index].dev_ops->ata_read(index, count, base_lba, buf);
	else
		return ATA_NOHNDL;
}

/*******************************************************************************
 * Function 'sys_ata_write_sectors':                                           *
 * writes multiple sectors by the base LBA address                             *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint16_t count [IN] - amount of sectors to write                *
 *           - uint64_t lba_addr [IN] - LBA address of sector to write         *
 *           - uint8_t *buf [IN] - buffer to write                             *
 *                                                                             *
 * Return value: uint8_t ATA write result                                      *
 *******************************************************************************/
uint8_t sys_ata_write_sectors(uint8_t index, uint16_t count, uint64_t base_lba, uint8_t *buf) {
	if (sys_ide_device[index].dev_ops->ata_write != NULL)
		return sys_ide_device[index].dev_ops->ata_write(index, count, base_lba, buf);
	else
		return ATA_NOHNDL;
}

/*******************************************************************************
 * Function 'sys_ata_reset':                                                   *
 * resets drive (master & slave) by index number                               *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *                                                                             *
 * Return value: none                                                          *
 *******************************************************************************/
void sys_ata_reset(uint8_t index) {
	if (sys_ide_device[index].type == IDE_TYPE_ATA)
		if (sys_ide_device[index].dev_ops->ata_reset != NULL)
			sys_ide_device[index].dev_ops->ata_reset(index);
}

/*******************************************************************************/
/*                           Static ATA functions                              */
/*******************************************************************************/

/*******************************************************************************
 * Function 'ide_dev_init':                                                    *
 * performs primary initialization of ATA drives                               *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint8_t drive [IN] - primary/slave                              *
 *           - uint8_t channel [IN] - command register                         *
 *           - uint16_t cfg_reg [IN] - config register                         *
 *                                                                             *
 * Return value: none                                                          *
 ******************************************************************************/
static void ide_dev_init(uint8_t index, uint8_t drive, uint16_t channel, uint16_t cfg_reg) {
	sys_ide_device[index].drive = drive;
	sys_ide_device[index].channel = channel;
	sys_ide_device[index].cfg_reg = cfg_reg;
	sys_ide_device[index].type = IDE_TYPE_INVALID;		/* invalid by default */
	sys_ide_device[index].dev_ops = kvalloc(sizeof(struct ide_dev_ops));	/* TODO: add error state handler */

	sys_ide_device[index].dev_ops->ata_read = NULL;
	sys_ide_device[index].dev_ops->ata_write = NULL;
	sys_ide_device[index].dev_ops->ata_reset = NULL;
}

/*******************************************************************************
 * Function 'ata_scan':                                                        *
 * scans all ATA drives on both of buses and fills sys_ide_device type field   *
 *                                                                             *
 * Parameters: none                                                            *
 *                                                                             *
 * Return value: none                                                          *
 ******************************************************************************/
static void ata_scan() {
	uint8_t i = 0;

	for (i = 0; i < ATA_MAX_IDE_NUM; i++) {
		outportb(sys_ide_device[i].cfg_reg, 2);

		sys_ide_device[i].type = ata_verify(i);
		if (sys_ide_device[i].type != 0xFF)
		printf(KERN_INFO"IDE device found at drive %x, channel %x; Type = %x\n", sys_ide_device[i].drive, sys_ide_device[i].channel, sys_ide_device[i].type);

		/* operations */
		sys_ide_device[i].dev_ops->ata_read = ata_read_sectors;
		sys_ide_device[i].dev_ops->ata_write = ata_write_sectors;
		sys_ide_device[i].dev_ops->ata_reset = ata_reset_drive;
	}
}

/*******************************************************************************
 * Function 'ata_verify':                                                      *
 * checks whether drive is attached on bus                                     *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t drive [IN] - index of drive                             *
 *                                                                             *
 * Return value: uint8_t type of ATA device                                    *
 ******************************************************************************/
 static uint8_t ata_verify(uint8_t drive) {
	volatile uint8_t status = 0;			/* status register */
	uint8_t isATAPI_flg = 0;				/* is it ATA or ATAPI drive */
	uint16_t try_cnt = 0;					/* count of attempts */

	uint16_t dbyte;
	uint16_t i;

	/* nIEN bit enable */
	// outportb(sys_ide_device[drive].cfg_reg, 2);

	/* device select */
	outportb(sys_ide_device[drive].channel + ATA_DRIVE, sys_ide_device[drive].drive);
	ata_400ns_delay(drive);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[drive].channel + ATA_STATUS, status);
		if (status == 0) 
			return IDE_TYPE_INVALID;

		if (try_cnt == ATA_MAX_OPERATIONS)
			return IDE_TYPE_INVALID;	/* sorry, but you are busy too long */
		try_cnt++;
	} while (status & ATA_BSY);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[drive].channel + ATA_STATUS, status);
		if (status == 0) 
			return IDE_TYPE_INVALID;

		if (try_cnt == ATA_MAX_OPERATIONS)
			return IDE_TYPE_INVALID;

		try_cnt++;
	} while (!(status & ATA_DRDY));

	/* OK, now we know, that device DOES exist. Let's check it for ATA or ATAPI. */

	/* Let's try ATA controller */
	outportb(sys_ide_device[drive].channel + ATA_CMD, 0xEC);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[drive].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS) {	/* sorry, you was busy too long. Now I will assume you are ATAPI */
			isATAPI_flg = 1;
			break;	/* you don't have 0xEC command? Let's try 0xA1 for ATAPI */
		}
		try_cnt++;
	} while (!(status & ATA_DRQ));

	if (!isATAPI_flg)
		goto ata_identify_read;

	if (isATAPI_flg) {
		outportb(sys_ide_device[drive].channel + ATA_CMD, 0xA1);

		do {
			inportb(sys_ide_device[drive].channel + ATA_STATUS, status);
		} while (!(status & ATA_DRQ));
	}

ata_identify_read:

	for (i = 0; i < 256; i++)
		inportw(sys_ide_device[drive].channel + ATA_DATA, dbyte);

		if (isATAPI_flg)
			return IDE_TYPE_ATAPI;
		else
			return IDE_TYPE_ATA;

}

/*******************************************************************************
 * Function 'ata_400ns_delay':                                                 *
 * waits between drive operations, ~400 ns according to specification          *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *                                                                             *
 * Return value: none                                                          *
 ******************************************************************************/
static void ata_400ns_delay(uint8_t index) {
	uint8_t status;

	inportb(sys_ide_device[index].channel + ATA_STATUS, status);
	inportb(sys_ide_device[index].channel + ATA_STATUS, status);
	inportb(sys_ide_device[index].channel + ATA_STATUS, status);
	inportb(sys_ide_device[index].channel + ATA_STATUS, status);	/* ~ 400 ns according to specification */
}

/*******************************************************************************
 * Function 'ata_20ms_delay':                                                  *
 * waits between drive operations, ~20 microseconds                            *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *                                                                             *
 * Return value: none                                                          *
 ******************************************************************************/
static void ata_20ms_delay(uint8_t index) {

	uint8_t i;
	for (i = 0; i < 5; i++)
		ata_400ns_delay(index);
}

/*******************************************************************************
 * Function 'ata_read':                                                        *
 * reads one sector by the base LBA address                                    *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint64_t lba_addr [IN] - LBA address of sector to read          *
 *           - uint8_t *buf [OUT] - buffer to save read results                *
 *                                                                             *
 * Return value: uint8_t ATA read result                                       *
 *******************************************************************************/
static uint8_t ata_read(uint8_t index, uint64_t lba_addr, uint8_t *buf) {

	uint8_t status = 0;
	uint16_t try_cnt = 0;

	/* Enable nIEN bit */
	// outportb(sys_ide_device[index].cfg_reg, 6);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;/* FIXME: <- */

		try_cnt++;
	} while (status & ATA_BSY);

	//outportb(sys_ide_device[index].channel + ATA_ERROR, 0x00);
	//outportw(sys_ide_device[index].channel + ATA_SECTOR, 0x00);
	//outportb(sys_ide_device[index].channel + ATA_SECTOR, 0x00);

	outportb(sys_ide_device[index].channel + ATA_DRIVE, (((sys_ide_device[index].drive & 0x1F) >> 4) == 1) ? 0x50 : 0x40);

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;
		try_cnt++;
	} while (!(status & ATA_DRDY));

	outportb(sys_ide_device[index].channel + ATA_SECTOR, 0);//((sectors & 0xFF00) >> 8));
	outportb(sys_ide_device[index].channel + ATA_LBA_L, (uint8_t)(lba_addr & 0xFF000000) >> 24);
	outportb(sys_ide_device[index].channel + ATA_LBA_M, (uint8_t)((uint64_t)(lba_addr & 0xFF00000000) >> 32));
	outportb(sys_ide_device[index].channel + ATA_LBA_H, (uint8_t)((uint64_t)(lba_addr & 0xFF0000000000) >> 40));

	outportb(sys_ide_device[index].channel + ATA_SECTOR, 1);//(sectors & 0xFF));

	outportb(sys_ide_device[index].channel + ATA_LBA_L, (uint8_t)(lba_addr & 0x000000FF));
	outportb(sys_ide_device[index].channel + ATA_LBA_M, (uint8_t)((lba_addr & 0x0000FF00) >> 8));
	outportb(sys_ide_device[index].channel + ATA_LBA_H, (uint8_t)((lba_addr & 0x00FF0000) >> 16));

	outportb(sys_ide_device[index].channel + ATA_CMD, 0x24);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;
		try_cnt++;
	} while (!(status & ATA_DRQ));

	/* Transfering bytes */

	uint16_t dbyte;	/* double byte to read */
	uint16_t wordCount;	/* word */
	uint8_t error;	/* error */

	for (wordCount = 0; wordCount < 256; wordCount++) {
		inportw(sys_ide_device[index].channel + ATA_DATA, dbyte);
		buf[(wordCount * 2)] = (uint8_t)(dbyte & 0xFF);
		buf[(wordCount * 2) + 1] = (uint8_t)(dbyte >> 8);
	}

	ata_20ms_delay(index);

	inportb(sys_ide_device[index].channel + ATA_ERROR, error);
	if (error)
		return error;

	/* Disable nIEN bit */
	// outportb(sys_ide_device[index].cfg_reg, 0);
	return ATA_SUCCESS;
}

/*******************************************************************************
 * Function 'ata_write':                                                       *
 * writes one sector by the base LBA address                                   *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint64_t lba_addr [IN] - LBA address of sector to write         *
 *           - uint8_t *buf [IN] - buffer to write                             *
 *                                                                             *
 * Return value: uint8_t ATA write result                                      *
 *******************************************************************************/
static uint8_t ata_write(uint8_t index, uint64_t lba_addr, uint8_t *buf) {

	uint8_t status = 0;
	uint16_t try_cnt = 0;

	// outportb(sys_ide_device[index].cfg_reg, 2);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;
		try_cnt++;
	} while (status & ATA_BSY);

	outportb(sys_ide_device[index].channel + ATA_DRIVE, (((sys_ide_device[index].drive & 0x1F) >> 4) == 1) ? 0x50 : 0x40);

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;
		try_cnt++;
	} while (!(status & ATA_DRDY));

	outportb(sys_ide_device[index].channel + ATA_SECTOR, 0);//((sectors & 0xFF00) >> 8));

	outportb(sys_ide_device[index].channel + ATA_LBA_L, (uint8_t)(lba_addr & 0xFF000000) >> 24);
	outportb(sys_ide_device[index].channel + ATA_LBA_M, (uint8_t)((uint64_t)(lba_addr & 0xFF00000000) >> 32));
	outportb(sys_ide_device[index].channel + ATA_LBA_H, (uint8_t)((uint64_t)(lba_addr & 0xFF0000000000) >> 40));

	outportb(sys_ide_device[index].channel + ATA_SECTOR, 1);//sectors & 0xFF);

	outportb(sys_ide_device[index].channel + ATA_LBA_L, (uint8_t)(lba_addr & 0x000000FF));
	outportb(sys_ide_device[index].channel + ATA_LBA_M, (uint8_t)((lba_addr & 0x0000FF00) >> 8));
	outportb(sys_ide_device[index].channel + ATA_LBA_H, (uint8_t)((lba_addr & 0x00FF0000) >> 16));

	outportb(sys_ide_device[index].channel + ATA_CMD, 0x34);

	try_cnt = 0;

	do {
		inportb(sys_ide_device[index].channel + ATA_STATUS, status);
		if (try_cnt == ATA_MAX_OPERATIONS)
			return ATA_NEXIST;
		try_cnt++;
	} while (!(status & ATA_DRQ));

	/* Transfering bytes */

	uint16_t dbyte;	/* double byte to write */
	uint16_t wordCount;	/* word */
	uint8_t error;	/* error */

	for (wordCount = 0; wordCount < 256; wordCount++) {
		dbyte = (*(buf + (wordCount * 2) + 1) << 8) | *(buf + (wordCount * 2));
		outportw(sys_ide_device[index].channel + ATA_DATA, dbyte);
		ata_400ns_delay(index);
	}

	ata_400ns_delay(index);

	inportb(sys_ide_device[index].channel + ATA_ERROR, error);
	if (error)
		return error;

	// outportb(sys_ide_device[index].cfg_reg, 0);

	return ATA_SUCCESS;
}

/*******************************************************************************
 * Function 'ata_read_sectors':                                                *
 * reads multiple sectors by the base LBA address                              *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint16_t count [IN] - amount of sectors to read                 *
 *           - uint64_t lba_addr [IN] - LBA address of sector to read          *
 *           - uint8_t *buf [OUT] - buffer to save read results                *
 *                                                                             *
 * Return value: uint8_t ATA read result                                       *
 *******************************************************************************/
static uint8_t ata_read_sectors(uint8_t index, uint16_t count, uint64_t src, uint8_t *buf) {
	uint32_t offset = 0;
	uint16_t i = 0;

	for (i = 0; i < count; i++) {
		uint8_t res = ata_read(0, src + i, buf + offset);
		ata_20ms_delay(index);
		if (res)
			return res;

		offset += 512;
	}

	return ATA_SUCCESS;
}

/*******************************************************************************
 * Function 'ata_write_sectors':                                               *
 * writes multiple sectors by the base LBA address                             *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *           - uint16_t count [IN] - amount of sectors to write                *
 *           - uint64_t lba_addr [IN] - LBA address of sector to write         *
 *           - uint8_t *buf [IN] - buffer to write                             *
 *                                                                             *
 * Return value: uint8_t ATA write result                                      *
 *******************************************************************************/
static uint8_t ata_write_sectors(uint8_t index, uint16_t count, uint64_t src, uint8_t * buf) {
	uint16_t offset = 0;
	uint16_t i = 0;

	for (i = 0; i < count; i++) {
		uint8_t res = ata_write(0, src + i, buf + offset);
		ata_20ms_delay(index);
		if (res)
			return res;

		offset += 512;
	}

	return ATA_SUCCESS;
}

/*******************************************************************************
 * Function 'ata_reset_drive':                                                 *
 * resets drive (master & slave) by index                                      *
 *                                                                             *
 * Parameters:                                                                 *
 *           - uint8_t index [IN] - index of drive                             *
 *                                                                             *
 * Return value: none                                                          *
 *******************************************************************************/
static void ata_reset_drive(uint8_t index) {
	outportb(sys_ide_device[index].cfg_reg, 4);
}


