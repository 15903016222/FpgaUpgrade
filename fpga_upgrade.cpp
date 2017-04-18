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

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define GPIO99_LOW      0x6004
#define GPIO99_HIGH		0x6005

#define SOFT_WARE_SIZE  (1024)

int main (int argc, char *argv[]) {
    int fd_mtd, fd_tt, fd_new_soft;
    int size, ret;
    int val = 0x01;
    char data[SOFT_WARE_SIZE] = {0};
    char buff[SOFT_WARE_SIZE] = {0};

    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 500000;


    if ((fd_mtd = open ("/dev/spidev3.1", O_RDWR)) == -1) {
        perror ("/dev/spidev3.1");
        return -1;
    }

    /*
     * spi mode
     */
//    val = (val & 0xfc) | 0x00;
    if ((ioctl(fd_mtd, SPI_IOC_RD_MODE, &mode)) == -1) {
        printf("can't get spi rd_mode \n");
        close (fd_mtd);
        return -1;
    }
//    val = (val & 0xfc) | 0x00;
    if ((ioctl(fd_mtd, SPI_IOC_WR_MODE, &mode)) == -1) {
        printf("can't set spi wr_mode \n");
        close (fd_mtd);
        return -1;
    }

    /*
     * bits per word
     */
/*    ret = ioctl(fd_mtd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        printf("can't set bits per word \n");
        close (fd_mtd);
        return -1;
    }

    ret = ioctl(fd_mtd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1) {
        printf ("can't get bits per word \n");
        close (fd_mtd);
        return -1;
    }
*/
    /*
     * max speed hz
     */
/*    ret = ioctl(fd_mtd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        printf ("can't set max speed hz \n");
        close (fd_mtd);
        return -1;
    }

    ret = ioctl(fd_mtd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        printf ("can't get max speed hz \n");
        close (fd_mtd);
        return -1;
    }
*/

/*
    if ((fd_tt = open ("/dev/tt", O_RDWR)) == -1) {
        perror ("open /dev/tt");
        close (fd_mtd);
    }
    ioctl(fd_tt, GPIO99_HIGH, &val);

    // open the software
    // check the usb exited or not
    // 挂载U盘 检查

*/
    if ((fd_tt = open("/dev/gpiodrv", O_RDWR)) == -1 )
    {
        perror("/dev/gpiodrv");
        close (fd_mtd);
        return -1;
    }

    printf ("gpio99 set 1 ... \n");
    ioctl(fd_tt, GPIO99_HIGH, &val);
    close (fd_tt);
    printf ("gpio99 set 1 over ... \n");

    memset (data, 8, SOFT_WARE_SIZE);
    printf ("写入 data = %d ... ...\n", data[512]);

    int i = 0;
    while (1) {
        for (i = 0; i < 128; i++) {
            if ((size = write (fd_mtd, data, SOFT_WARE_SIZE)) < 0) {
                printf ("can't write to spi \n");
                close (fd_mtd);
                return -1;
            }
            printf("write size = %d \n", size);
            lseek (fd_mtd, (i + 1) * 1024, SEEK_SET);
        }
        lseek (fd_mtd, 0, SEEK_SET);
        i = 0;
    }

    return 0;
}

