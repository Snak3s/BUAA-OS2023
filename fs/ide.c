/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_ID, sizeof(uint32_t));
		temp = begin + off;
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_OFFSET, sizeof(uint32_t));
		temp = DEV_DISK_OPERATION_READ;
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_START_OPERATION, sizeof(uint32_t));
		syscall_read_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_STATUS, sizeof(uint32_t));
		panic_on(temp == 0);
		syscall_read_dev(dst + off, DEV_DISK_ADDRESS | DEV_DISK_BUFFER, BY2SECT);
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_ID, sizeof(uint32_t));
		temp = begin + off;
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_OFFSET, sizeof(uint32_t));
		syscall_write_dev(src + off, DEV_DISK_ADDRESS | DEV_DISK_BUFFER, BY2SECT);
		temp = DEV_DISK_OPERATION_WRITE;
		syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_START_OPERATION, sizeof(uint32_t));
		syscall_read_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_STATUS, sizeof(uint32_t));
		panic_on(temp == 0);
	}
}

#define NSSD 32
#define SSD_W 1
#define SSD_UW 0

int ssd_map[NSSD];
int ssd_bitmap[NSSD];
int ssd_erase_cnt[NSSD];

void ssd_init() {
	for (int i = 0; i < NSSD; i++) {
		ssd_map[i] = -1;
		ssd_bitmap[i] = SSD_W;
		ssd_erase_cnt[i] = 0;
	}
}

int ssd_read(u_int logic_no, void *dst) {
	if (ssd_map[logic_no] == -1) {
		return -1;
	}
	ide_read(0, ssd_map[logic_no], dst, 1);
	return 0;
}

void ssd_write(u_int logic_no, void *src) {
	static char zero[BY2SECT];
	if (ssd_map[logic_no] != -1) {
		ide_write(0, ssd_map[logic_no], zero, 1);
		ssd_bitmap[ssd_map[logic_no]] = SSD_W;
		ssd_erase_cnt[ssd_map[logic_no]]++;
		ssd_map[logic_no] = -1;
	}
	int sec_no = -1;
	for (int i = 0; i < NSSD; i++) {
		if (ssd_bitmap[i] == SSD_UW) {
			continue;
		}
		if (sec_no == -1 || ssd_erase_cnt[i] < ssd_erase_cnt[sec_no]) {
			sec_no = i;
		}
	}
	if (ssd_erase_cnt[sec_no] >= 5) {
		int uw_sec_no = -1;
		for (int i = 0; i < NSSD; i++) {
			if (ssd_bitmap[i] == SSD_W) {
				continue;
			}
			if (uw_sec_no == -1 || ssd_erase_cnt[i] < ssd_erase_cnt[uw_sec_no]) {
				uw_sec_no = i;
			}
		}
		static char buf[BY2SECT];
		ide_read(0, uw_sec_no, buf, 1);
		ide_write(0, sec_no, buf, 1);
		ssd_bitmap[sec_no] = SSD_UW;
		ssd_bitmap[uw_sec_no] = SSD_W;
		ide_write(0, uw_sec_no, zero, 1);
		ssd_erase_cnt[uw_sec_no]++;
		for (int i = 0; i < NSSD; i++) {
			if (ssd_map[i] == uw_sec_no) {
				ssd_map[i] = sec_no;
			}
		}
		sec_no = uw_sec_no;
	}
	ssd_map[logic_no] = sec_no;
	ide_write(0, sec_no, src, 1);
	ssd_bitmap[ssd_map[logic_no]] = SSD_UW;
}

void ssd_erase(u_int logic_no) {
	static char zero[BY2SECT];
	if (ssd_map[logic_no] == -1) {
		return;
	}
	ide_write(0, ssd_map[logic_no], zero, 1);
	ssd_erase_cnt[ssd_map[logic_no]]++;
	ssd_bitmap[ssd_map[logic_no]] = SSD_W;
	ssd_map[logic_no] = -1;
}
