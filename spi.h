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
#include <termios.h>
#include <time.h>

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

#define GPIO99_VOLTAGE_SELECT_PIN_LOW      0x6004
#define GPIO99_VOLTAGE_SELECT_PIN_HIGH		0x6005
#define GPIO21_CS_LOW      0x6006
#define GPIO21_CS_HIGH     0x6007

extern int fd_spi, fd_cs;

extern int  spi_open (const char *path);
extern int  spi_set_mode (uint8_t mode);
extern int  spi_set_bits (uint8_t bits);
extern int  spi_set_speed (uint32_t speed);
extern void spi_close (void);

extern int  spi_rdid (char *id, size_t size);
extern int  spi_rdsr ( char *status);
extern int  spi_wrsr (uint8_t reg);\
extern int  spi_wren (void);
extern int  spi_se (unsigned int address);
extern int  spi_be (void);
extern int  spi_read (unsigned int address, char *data, size_t size);
extern int  spi_pp (unsigned int address, char *data, size_t size);

extern int  spi_write (unsigned int address, char *data, size_t size);
extern int  spi_is_busy (void);

extern int  spi_cs_open (const char *path);
extern int  spi_cs_low (void);
extern int  spi_cs_high (void);
extern void spi_cs_close (void);

#endif //__SPI_H
