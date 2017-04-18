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
    static uint32_t speed = 5000000;

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

    printf ("gpio99 set 1 ... \n");
    ioctl(fd_tt, GPIO99_HIGH, &val);
    close (fd_tt);
    printf ("gpio99 set 1 over ... \n");

    uint8_t cmd;

    /* read id */

while (1) {
    cmd = RDID;
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
    printf ("ID: tmp = %.2x \n", tmp[0]);
}

/*    // 写使能
    cmd = WREN;   // RDID
    printf ("WREN: cmd = %.2x ... ...\n", cmd);

    if ((size = write (fd_mtd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write enable");
        close (fd_mtd);
        return -1;
    }

    // 发送pp
    cmd = PP;
    printf ("PP: cmd = %.2x ... ...\n", cmd);

    if ((size = write (fd_mtd, &cmd, sizeof (cmd))) < 0) {
        perror ("Spi PP");
        close (fd_mtd);
        return -1;
    }

    // 发送地址
    int addr = 0;
    printf ("write addr \n");

    if ((size = write (fd_mtd, &addr, sizeof (addr))) < 0) {
        perror ("Spi data addr");
        close (fd_mtd);
        return -1;
    }

    // 发送数据
int i = 0;
for (i = 0; i < 65536; ++i) {
    unsigned int tmp[256] = {0,};
    printf ("write tmp[0] = %d to spi \n", tmp[0]);

    if ((size = write (fd_mtd, &tmp, sizeof (tmp))) < 0) {
        perror ("Spi tmp");
        close (fd_mtd);
        return -1;
    }
    delay (10000);
}
    printf ("Data transfer is over \n");
*/
    return 0;
}

