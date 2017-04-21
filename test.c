#include "spi.h"

static int fd_mtd, fd_tt;

int spi_mode (void) {
    int ret, val;
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

 //   printf ("gpio99 set 1 ... \n");
    ioctl(fd_tt, GPIO99_HIGH, &val);
 //   printf ("gpio99 set 1 over ... \n");

    return 0;
}

int main (int argc, char *argv[]) {
    int i, j;

    int fd_file;
    size_t size = 0;
    size_t tmp = 0;
    char buff[256] = {0};

    spi_mode();

    // RDID
    spi_cs_low (fd_tt);
    char id[3] = {0,};
    spi_rdid (fd_mtd, id, sizeof (id));
    spi_cs_high(fd_tt);
    if (id[0] != 0x20) {
        printf ("No found SPI FLASH. \n");
        return -1;
    }
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);

    // BE
    spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_be(fd_mtd);
    spi_cs_high(fd_tt);

    // RDSR
    while (1) {
        spi_cs_low (fd_tt);
        unsigned char status;
        spi_rdsr(fd_mtd, &status);
        spi_cs_high(fd_tt);
            printf ("RDSR: status = %.2x \n", status);
        if (!(status | 0x0)) {
            break;
        }
        else {
            sleep (5);
            continue;
        }
    }

    // file operation
    fd_file = open (argv[1], O_RDONLY);
    if (fd_file < 0) {
        perror ("open");
        return -1;
    }
    size = lseek(fd_file, 0, SEEK_END);
    if (size < 0) {
        return -1;
    }
    lseek(fd_file, 0, SEEK_SET);

    printf ("file: size = %d \n", size);

    tmp = size;
    int count = 0;
    while (tmp / 256) {
        memset (buff, 0, 256);
        if (read (fd_file, buff, 256) < 0) {
            perror ("read");
            return -1;
        }

        // RDSR
        while (1) {
            spi_cs_low (fd_tt);
            unsigned char status;
            spi_rdsr(fd_mtd, &status);
            spi_cs_high(fd_tt);
//            printf ("RDSR: status = %.2x \n", status);
            if (!(status | 0x0)) {
                break;
            }
            else {
                sleep (5);
                continue;
            }
        }

        // PP
        spi_cs_low(fd_tt);
        spi_wren(fd_mtd);
        spi_cs_high(fd_tt);
        spi_cs_low(fd_tt);
        spi_pp(fd_mtd, 256 * count, buff, 256);
        spi_cs_high(fd_tt);

        ++count;
        tmp -= 256;
    }

    tmp = size % 256;
    if (tmp <= 0) {
        return 0;
    }
    memset (buff, 0, 256);
    if (read (fd_file, buff, tmp) < 0) {
        perror ("read");
        return -1;
    }

    // RDSR
    while (1) {
        spi_cs_low (fd_tt);
        unsigned char status;
        spi_rdsr(fd_mtd, &status);
        spi_cs_high(fd_tt);
//            printf ("RDSR: status = %.2x \n", status);
        if (!(status | 0x0)) {
            break;
        }
        else {
            sleep (5);
            continue;
        }
    }

    // PP
    spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_pp(fd_mtd, 256 * count, buff, 256);
    spi_cs_high(fd_tt);

    // RDSR
    while (1) {
        spi_cs_low (fd_tt);
        unsigned char status;
        spi_rdsr(fd_mtd, &status);
        spi_cs_high(fd_tt);
//            printf ("RDSR: status = %.2x \n", status);
        if (!(status | 0x0)) {
            break;
        }
        else {
            sleep (5);
            continue;
        }
    }


    // RDID
/*    spi_cs_low (fd_tt);
    char id[3] = {0,};
    spi_rdid (fd_mtd, id, sizeof (id));
    spi_cs_high(fd_tt);
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);

    // RDSR
    spi_cs_low (fd_tt);
    char status;
    spi_rdsr(fd_mtd, &status);
    spi_cs_high(fd_tt);
    printf ("RDSR: status = %.2x \n", status);
*/
    // SE
/*    spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_se(fd_mtd, 0x300);
    spi_cs_high(fd_tt);
*/
    // BE
 /*   spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_be(fd_mtd);
    spi_cs_high(fd_tt);
*/

    // WRSR
/*    spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_wrsr(fd_mtd, 0x03);
    spi_cs_high(fd_tt);
*/

    // PP
 /*   char write_data[256] = {0};

    for (i = 0; i < 256; ++i) {
        write_data[i] = i % 256;
    }
    printf ("write_data: \n");
    for (i = 0; i < 16; ++i) {
        for (j = 0; j < 16; ++j) {
            printf ("%.2x ", write_data[i * 16 + j]);
        }
        printf ("\n");
    }

    spi_cs_low(fd_tt);
    spi_wren(fd_mtd);
    spi_cs_high(fd_tt);
    spi_cs_low(fd_tt);
    spi_pp(fd_mtd, 0x000, write_data, 256);
    spi_cs_high(fd_tt);
*/
    // RDSR
/*    while (1) {
        spi_cs_low (fd_tt);
        unsigned char status;
        spi_rdsr(fd_mtd, &status);
        spi_cs_high(fd_tt);
        printf ("RDSR: status = %.2x \n", status);
        if (!(status | 0x0)) {
            break;
        }
        else {
            sleep (5);
            continue;
        }
    }
    printf ("RDSR is OVER ... \n");
*/
    // READ
 /*   spi_cs_low (fd_tt);
    char read_data[1024] = {0};
    spi_read(fd_mtd, 0x0, read_data, sizeof (read_data));
    spi_cs_high(fd_tt);
    printf ("READ: \n");
    for (i = 0; i < 32; ++i) {
        for (j = 0; j < 32; ++j) {
            printf ("%.2x ", read_data[32 * i + j]);
        }
        printf ("\n");
    }
*/
    return 0;
}
