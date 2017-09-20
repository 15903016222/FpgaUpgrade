#include "spi.h"

#define SIZE 4096
#define SOFTWARE_SIZE 0x01000000
#define SOFTWARE_OFFSET 0xa2

int spi_setup (void)
{
    int fd_mtd, fd_tt;
    int res, val;
    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 500000;

    fd_mtd = spi_open("/dev/spidev3.1");
    if (fd_mtd < 0)
    {
        printf ("spi_open is failed.\n");
        return -1;
    }

    res = spi_set_mode(mode);
    if (res < 0)
    {
        printf ("spi_set_mode is failed.\n");
        spi_close ();
        return -1;
    }

    res = spi_set_bits(bits);
    if (res < 0)
    {
        printf ("spi_set_bits is failed. \n");
        spi_close ();
        return -1;
    }

    res = spi_set_speed(speed);
    if (res < 0)
    {
        printf ("spi_set_speed is failed. \n");
        spi_close ();
        return -1;
    }

    fd_tt = spi_cs_open("/dev/tt");
    if (fd_tt < 0)
    {
        printf ("open \"/dev/tt\" is failed. \n");
        return -1;
    }

    ioctl(fd_tt, GPIO99_HIGH, &val);

    return 0;
}

void spi_wait_ready (void)
{
    while (1)
    {

        if (!spi_is_busy())
        {
            break;
        }
        else
        {
//            printf ("spi is busy ... \n");
            usleep (100000);
            continue;
        }
    }

    return ;
}

unsigned char convert (unsigned char num)
{
    unsigned char a = 0;
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        a = ((num >> i) & 0x01) | a << 1;
    }

    return a;
}

int main (int argc, char *argv[]) {
    int i;
    int res;
    int fd_file;
    size_t size = 0;
    size_t tmp = 0;
    char buff[256] = {0};

    if (argc < 2)
    {
        printf ("Usage: %s <FilePath> \n", argv[0]);
        return -1;
    }

    spi_setup();

    // RDID
    char id[3] = {0,};
    spi_rdid (id, sizeof (id));
//    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);
    if (id[0] != 0x20) {
        printf ("Can not find SPI FLASH. \n");
        return -1;
    }

    // RDSR
    unsigned char status;
    spi_rdsr(&status);
    spi_wait_ready();

    // READ
    fd_file = open(argv[1], O_RDWR | O_CREAT);
    if (fd_file < 0)
    {
        perror ("open fd_file");
        return -1;
    }

    unsigned char read_data[SIZE] = {0};
    unsigned int addr = 0;
    size = SOFTWARE_SIZE;

    while (size / SIZE)
    {
        memset (read_data, 0, SIZE);
        res = spi_read(addr, read_data, sizeof (read_data));

        for (i = 0; i < SIZE; i++)
        {
            read_data[i] = convert (read_data[i]);
        }

        write (fd_file, read_data, res);

        size -= res;
        addr += res;
        if (addr % res)
        {
            printf ("size/SIZE no read SIZE ... \n");
            return -1;
        }
//        printf ("size = %d addr = %d \n", size, addr);
    }
    while (size % SIZE)
    {
        memset (read_data, 0, SIZE);
        res = spi_read(addr, read_data, size % SIZE);

        for (i = 0; i < SIZE; i++)
        {
            read_data[i] = convert (read_data[i]);
        }

        write (fd_file, read_data, res);

        size -= res;
        addr += res;
//        printf ("size = %d addr = %d \n", size, addr);
    }
    printf ("Read from spi ,write to file over ... \n");
    close (fd_file);
    spi_close ();
    spi_cs_close ();

    return 0;
}
