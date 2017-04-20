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
#define GPIO21_LOW      0x6006
#define GPIO21_HIGH     0x6007

#define GPIO109_HIGH    0x6008
#define GPIO109_LOW     0x6009
#define GPIO109_HIGH_PULL 0x6010
#define GPIO109_LOW_PULL  0x6011

#define SOFT_WARE_SIZE  (1024)

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

void delay (int m) {
    int i = 0;
    for (i = 0; i < m; ++i) {
        ;
    }
}

static void pabort(const char *s)
{
    perror(s);
    abort();
}

int main (int argc, char *argv[]) {
    int fd_mtd, fd_tt, fd_new_soft;
    int size, ret;
    int val = 0x01;
    char data[SOFT_WARE_SIZE] = {0};
    char buff[SOFT_WARE_SIZE] = {0};

    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 300000;

    if ((fd_mtd = open ("/dev/spidev3.1", O_RDWR)) == -1) {
        perror ("/dev/spidev3.1");
        return -1;
    }

    /*
     * spi mode
     */
    if ((ioctl(fd_mtd, SPI_IOC_RD_MODE, &mode)) == -1) {
        printf("can't get spi rd_mode \n");
        close (fd_mtd);
        return -1;
    }
    if ((ioctl(fd_mtd, SPI_IOC_WR_MODE, &mode)) == -1) {
        printf("can't set spi wr_mode \n");
        close (fd_mtd);
        return -1;
    }

    /*
     * bits per word
     */
    ret = ioctl(fd_mtd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd_mtd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd_mtd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fd_mtd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");


    if ((fd_tt = open("/dev/gpiodrv", O_RDWR)) == -1 )
    {
        perror("/dev/gpiodrv");
        close (fd_mtd);
        return -1;
    }

    if (!strncmp("no", argv[1], 2)) {
        printf ("No pull up/down ... \n");
        while (1) {
            printf ("Please input enter to set GPIO109 HIGH \n");
            getchar();
            ioctl(fd_tt, GPIO109_HIGH, &val);
            printf ("Please input enter to set GPIO109 LOW \n");
            getchar();
            ioctl(fd_tt, GPIO109_LOW, &val);
        }
    }
    else if (!strncmp("pull", argv[1], 4)) {
        printf ("Pull up/down ... \n");
        while (1) {
            printf ("Please input enter to set GPIO109 HIGH \n");
            getchar();
            ioctl(fd_tt, GPIO109_HIGH_PULL, &val);
            printf ("Please input enter to set GPIO109 LOW \n");
            getchar();
            ioctl(fd_tt, GPIO109_LOW_PULL, &val);
        }
    }


    //printf ("gpio99 set 1 ... \n");
    //ioctl(fd_tt, GPIO99_HIGH, &val);
    //printf ("gpio99 set 1 over ... \n");
/*
    uint8_t cmd;

while (1) {
    cmd = RDID;
    printf ("set gpio21 1 \n");
    getchar();
    ioctl(fd_tt, GPIO21_HIGH, &val);
    printf ("set gpio21 0 \n");
    getchar();
    ioctl(fd_tt, GPIO21_LOW, &val);

    printf ("RDID: cmd = %.2x \n", cmd);
    if ((size = write (fd_mtd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write RDID");
        close (fd_mtd);
        return -1;
    }

    uint8_t tmp[3] = {0,};
    if ((size = read (fd_mtd, &tmp, sizeof (tmp))) < 0) {
        perror ("read id");
        close (fd_mtd);
        return -1;
    }

}
*/
    return 0;
}

