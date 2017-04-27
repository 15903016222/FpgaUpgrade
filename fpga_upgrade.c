#include "spi.h"

#define CHECK_SIZE  (4096)
#define SIZE        (CHECK_SIZE * 32)

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

int file_operate (const char *path, size_t *size)
{
    int fd;

    if (NULL == path || NULL == size)
    {
        return -1;
    }

    fd = open (path, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }
    *size = lseek(fd, 0, SEEK_END);

    lseek(fd, 0, SEEK_SET);

    return fd;
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
            sleep (1);
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

int main (int argc, char *argv[])
{
    int fd_file;
    int i, res, tmp;
    unsigned int addr = 0;
    size_t size = 0;
    unsigned char buff[SIZE] = {0,};

    if (argc < 2)
    {
        printf ("Usage: %s <FilePath> \n", argv[0]);
        return -1;
    }

    spi_setup();

    fd_file = file_operate (argv[1], &size);
    if (fd_file < 0)
    {
        printf ("file_operate is failed. \n");
        return -1;
    }

    // RDID
    char id[3] = {0,};
    spi_rdid (id, sizeof (id));
//    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);
    if (id[0] != 0x20)
    {
        printf ("Can not find SPI FLASH. \n");
        return -1;
    }

    // RDSR
    unsigned char status;
    spi_rdsr(&status);
    printf ("Start upgrading fpga ... \n");

    spi_wait_ready();
    // BE
    spi_be();
//    printf ("file size is %d \n", size);
    spi_wait_ready();
//    printf ("spi_be is over ... \n");

    tmp = SOFTWARE_SIZE;
    lseek (fd_file, SOFTWARE_OFFSET, SEEK_SET);

    while (tmp / SIZE)
    {
        memset (buff, 0, SIZE);
        // read file
        if ((res = read (fd_file, buff, SIZE)) < 0)
        {
            perror ("read");
            close (fd_file);
            spi_close ();
            spi_cs_close ();
            return -1;
        }

        for (i = 0; i < SIZE; i++)
        {
            buff[i] = convert (buff[i]);
        }

        // write to flash
        spi_wait_ready();
        res = spi_write(addr, buff, res);
        addr += res;
        tmp  -= res;
//        printf ("tmp = %d, addr = %d \n", tmp, addr);
    }
    while (tmp % SIZE != 0)
    {
        memset (buff, 0, SIZE);
        // read file
        if ((res = read (fd_file, buff, tmp % SIZE)) < 0)
        {
            perror ("read");
            close (fd_file);
            spi_close ();
            spi_cs_close ();
            return -1;
        }

        for (i = 0; i < SIZE; i++)
        {
            buff[i] = convert (buff[i]);
        }

        // write to flash
        spi_wait_ready();
        res = spi_write(addr, buff, res);
        addr += res;
        tmp  -= res;
//        printf ("tmp = %d, addr = %d \n", tmp, addr);
    }
    spi_wait_ready();

    // check
    printf ("Start checking fpga ... \n");
    lseek (fd_file, SOFTWARE_OFFSET, SEEK_SET);

    unsigned char read_data[CHECK_SIZE] = {0}; // read flash buff
    unsigned char file_data[CHECK_SIZE] = {0}; // read file  buff

    addr = 0;
    tmp = SOFTWARE_SIZE;
    while (tmp / CHECK_SIZE)
    {
        memset (read_data, 0, CHECK_SIZE);
        memset (file_data, 0, CHECK_SIZE);
        res = spi_read(addr, read_data, sizeof (read_data));

        read (fd_file, file_data, res);

        for (i = 0; i < CHECK_SIZE; i++)
        {
            read_data[i] = convert(read_data[i]);
        }

        if (memcmp(file_data, read_data, CHECK_SIZE))
        {
            printf ("Write error.\n");
            return -1;
        }

        tmp -= res;
        addr += res;
//        printf ("res = %d tmp = %d, addr = %d \n", res, tmp, addr);
    }
    while (tmp % CHECK_SIZE)
    {
        memset (read_data, 0, CHECK_SIZE);
        memset (file_data, 0, CHECK_SIZE);
        res = spi_read(addr, read_data, tmp % CHECK_SIZE);

        read (fd_file, file_data, res);

        for (i = 0; i < CHECK_SIZE; i++)
        {
            read_data[i] = convert(read_data[i]);
        }

        if (memcmp(file_data, read_data, CHECK_SIZE))
        {
            printf ("Write error");
            return -1;
        }
        tmp -= res;
        addr += res;
//        printf ("res = %d tmp = %d, addr = %d \n", res, tmp, addr);
    }
    printf ("Ok! \n");

    close (fd_file);
    spi_close();
    spi_cs_close();

    return 0;
}
