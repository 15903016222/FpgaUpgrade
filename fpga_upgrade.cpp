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


























































/*
#include "spi.h"

#define GPIO99_HIGH  0x8000

using namespace std;
using namespace DplFpga;

int i, j, val;
char data[1024];
char buff[1024];
int fd_gpio;

class Spi spi;

int main (int argc, char *argv[]) {


    // 打开spidev设备
    if (!spi.open("/dev/spidev3.1")) {
        printf ("Open spi device fialed.\n");
        return -1;
    }

    // 设置 spi mode
    if (!spi.set_mode(spi.MODE0)) {
        printf ("Set spi mode failed.\n");
        return -1;
    }
    if (!spi.set_speed(500 * 1000)) {
        printf ("Set spi speed failed. \n");
        return -1;
    }
    if (!spi.set_lsb_first(false)) {
        printf ("Set spi lsb first failed. \n");
        return -1;
    }
    if (!spi.set_bits_per_word(8)) {
        printf ("Set spi bits per word failed. \n");
        return -1;
    }

    // 设置gpio的值
    if (fd_gpio = open ("/dev/tt", O_RDWR) == -1) {
        perror ("/dev/tt");
    }

    ioctl(fd_gpio, GPIO99_HIGH, &val);

    // 写入spi数据
    printf ("正在发送数据 ... \n");

    val = 'A';

    while (1) {
        if (val == 'Z') {
            printf ("over ... ... \n");
            val = 'A';
        }

        memset (data, val++, 1024);
        printf ("data is : %c \n", data[0]);

        if (!spi.write(data, 1024)) {
            printf ("Write data to spi failed. \n");
            return -1;
        }
        if (!spi.read(buff, 1024)) {
            printf ("Read data from spi failed. \n");
            return -1;
        }
        for (i = 0; i < 32; ++i) {
            printf ("buff[%d] = %d \n", i * 32, buff[i * 32]);
        }
        printf ("\n");
    }

    // 关闭 spidev设备
    spi.close();

    return 0;
}
*/

