/*
 * test spi
 */

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <stdio.h>

#define GPIO99_LOW      0x6004
#define GPIO99_HIGH		0x6005

int main (int argc, char *argv[]) {
    int fd_mtd, fd_gpio;
    int mode, res;
    int val = 0x01;
    char data[1024] = {0};

    if ((fd_mtd = open ("/dev/spidev3.1", O_RDWR)) == -1) {
        perror ("/dev/spidev3.1");
    }
//    val = (val & 0xfc) | 0x00;
    if ((ioctl(fd_mtd, SPI_IOC_RD_MODE, &mode)) == -1) {
        perror("can't get spi rd_mode");
    }
//    val = (val & 0xfc) | 0x00;
    if ((ioctl(fd_mtd, SPI_IOC_WR_MODE, &mode)) == -1) {
        perror("can't set spi wr_mode");
    }
    if( (fd_gpio = open("/dev/gpiodrv", O_RDWR)) == -1 )
    {
        perror("/dev/gpiodrv");
    }

    printf ("gpio158 set 1 ... \n");
    ioctl(fd_gpio, GPIO99_HIGH, &val);
    printf ("gpio158 set 1 over ... \n");

    memset (data, 'A', 1024);
    printf ("开始传输数据 data = %c ... ...\n", data[0]);
    while (1) {
        if ((res = write (fd_mtd, data, 4)) < 0) {
            perror ("can't write to spi \n");
        }
    }

    return 0;
}

