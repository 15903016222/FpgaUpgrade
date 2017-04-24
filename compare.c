#include "spi.h"

#define SIZE 4096

int spi_mode (void) {
    int fd_mtd, fd_tt;
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
    int res;
    int fd_file;
    size_t size = 0;
    size_t tmp = 0;
    char buff[256] = {0};

    spi_mode();

    // RDID
     char id[3] = {0,};
    spi_rdid (id, sizeof (id));
    if (id[0] != 0x20) {
        printf ("No found SPI FLASH. \n");
        return -1;
    }
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);

    // READ
    fd_file = open(argv[1], O_RDWR);
    if (fd_file < 0)
    {
        perror ("open file");
        return -1;
    }
    size = lseek (fd_file, 0, SEEK_END);
    lseek (fd_file, 0, SEEK_SET);

    char read_data[SIZE] = {0};
    char file_data[SIZE] = {0};
    unsigned int addr = 0;
    while (size / SIZE)
    {
        memset (read_data, 0, SIZE);
        memset (file_data, 0, SIZE);
        res = spi_read(addr, read_data, sizeof (read_data));

        lseek (fd_file, addr, SEEK_SET);
        read (fd_file, file_data, res);
        if (memcmp(file_data, read_data, SIZE))
        {
            int i, j;
            printf ("spi_data: \n");
            for (i = 0; i < 128; i++)
            {
                for (j = 0; j < 32; j++)
                {
                    printf ("%.2x ", read_data[32 * i + j]);
                }
                printf ("\n");
                if (i % 32 == 31)
                {
                    printf ("\n");
                }
            }
            printf ("file_data: \n");
            for (i = 0; i < 128; i++)
            {
                for (j = 0; j < 32; j++)
                {
                    printf ("%.2x ", file_data[32 * i + j]);
                }
                printf ("\n");
                if (i % 32 == 31)
                {
                    printf ("\n");
                }
            }

            printf ("addr = 0x%08x size/SIZE Not same ... \n", addr);
            close (fd_file);
            spi_close ();
            spi_cs_close();
            return -1;
        }
        size -= res;
        addr += res;
        if (addr % res)
        {
            printf ("size/SIZE no read SIZE ... \n");
            close (fd_file);
            spi_close ();
            spi_cs_close();
            return -1;
        }
    }
    while (size % SIZE)
    {
        memset (read_data, 0, SIZE);
        memset (file_data, 0, SIZE);
        res = spi_read(addr, read_data, size % SIZE);

        lseek (fd_file, addr, SEEK_SET);
        read (fd_file, file_data, res);
        if (memcmp(file_data, read_data, SIZE))
        {
            printf ("addr = 0x%08x size%SIZE Not same ... \n", addr);
            close (fd_file);
            spi_close ();
            spi_cs_close();
            return -1;
        }
        size -= res;
        addr += res;
    }
    printf ("Same ... \n");
    close (fd_file);
    spi_close ();
    spi_cs_close();

    return 0;
}
