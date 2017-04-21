#ifndef __SPI_H
#define __SPI_H

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <stdio.h>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define WREN    0x06
#define WRDI    0x04
#define RDID    0x9f
#define RDSR    0x05
#define WRSR    0x01
#define READ    0x03
#define PP      0x02
#define SE      0xd8
#define BE      0xc7
#define RES     0xab

#define GPIO99_LOW      0x6004
#define GPIO99_HIGH		0x6005
#define GPIO21_LOW      0x6006
#define GPIO21_HIGH     0x6007

extern void delay (int m);
extern void pabort (const char *s);
extern int  spi_rdid (int fd, char *id, size_t size);
extern int  spi_rdsr (int fd, char *status);
extern int  spi_wrsr (int fd, uint8_t reg);\
extern int  spi_wren (int fd);
extern int  spi_se (int fd, unsigned int address);
extern int  spi_be (int fd);
extern int  spi_read (int fd, unsigned int address, char *data, size_t size);
extern int  spi_pp (int fd, unsigned int address, char *data, size_t size);
extern int  spi_cs_low (int fd);
extern int  spi_cs_high (int fd);

#endif //__SPI_H
