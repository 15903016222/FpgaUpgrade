#include "spi.h"

static int fd_mtd, fd_tt;

int spi_mode (void) {
    int res, val;
    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 500000;

    fd_mtd = spi_open("/dev/spidev3.1");
    if (fd_mtd < 0)
    {
        perror ("spi_open");
        return -1;
    }

    res = spi_set_mode(mode);
    if (res < 0)
    {
        perror ("spi_set_mode");
        return -1;
    }

    res = spi_set_bits(bits);
    if (res < 0)
    {
        perror ("spi_set_bits");
        return -1;
    }

    res = spi_set_speed(speed);
    if (res < 0)
    {
        perror ("spi_set_speed");
        return -1;
    }

    fd_tt = spi_cs_open("/dev/gpiodrv");
    if (fd_tt < 0)
    {
        perror ("spi_cs_open");
        return -1;
    }

    ioctl(fd_tt, GPIO99_HIGH, &val);

    return 0;
}

int main (int argc, char *argv[]) {
    int i, j;

    int fd_file;
    size_t size = 0;
    size_t tmp = 0;
    char buff[256] = {0};

    spi_mode();

/*
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
*/

    // RDID
    char id[3] = {0,};
    spi_rdid (id, sizeof (id));
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);

    // RDSR

    unsigned char status;
    spi_rdsr(&status);
    printf ("RDSR: status = %.2x \n", status);


    // SE
//    spi_se(0x300);

    // BE
    spi_be();

    // RDSR
    while (1) {

        if (!spi_is_busy()) {
            break;
        }
        else {
            printf ("RDSR: status is busy \n");
            sleep (5);
            continue;
        }
    }

    // PP
/*    char write_data[256] = {0};

    for (i = 0; i < 256; ++i) {
        write_data[i] = i % 256;
    }
    printf ("write_data: \n");
    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 32; ++j) {
            printf ("%.2x ", write_data[i * 16 + j]);
        }
        printf ("\n");
    }

    spi_pp(0x200, write_data, 256);
*/

    // BP
    char write_data[1024 + 256] = {0};

    for (i = 0; i < 1024 + 256; ++i) {
        write_data[i] = i % 256;
    }
    printf ("write_data: \n");
    for (i = 0; i < 40; ++i) {
        for (j = 0; j < 32; ++j) {
            printf ("%.2x ", write_data[i * 32 + j]);
        }
        printf ("\n");
        if (i == 32)
        {
            printf ("\n");
        }
    }

    spi_write(0x0, write_data, 1024 + 128);

    // RDSR
    while (1) {

        if (!spi_is_busy()) {
            break;
        }
        else {
            printf ("RDSR: status is busy \n");
            sleep (5);
            continue;
        }
    }

    // READ
    char read_data[2048] = {0};
    spi_read(0x0, read_data, sizeof (read_data));
    printf ("READ: \n");
    for (i = 0; i < 64; ++i) {
        for (j = 0; j < 32; ++j) {
            printf ("%.2x ", read_data[32 * i + j]);
        }
        printf ("\n");
        if (i == 31)
        {
            printf ("\n");
        }
    }

    return 0;
}
