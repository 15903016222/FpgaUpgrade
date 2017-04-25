#include "spi.h"

#define CHECK_SIZE  (4096)
#define SIZE        (CHECK_SIZE * 32)

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
 //           printf ("spi is busy ... \n");
            sleep (1);
            continue;
        }
    }

    return ;
}

int main (int argc, char *argv[])
{
    int fd_file;
    int res, tmp;
    unsigned int addr = 0;
    size_t size = 0;
    char buff[SIZE] = {0,};

    spi_setup();

    fd_file = file_operate (argv[1], &size);
    if (fd_file < 0)
    {
        printf ("file_operate is failed. \n");
        return -1;
    }

    if (size > 0x01000000)
    {
        printf ("File size is too long. \n");
        return -1;
    }

    // RDID
    char id[3] = {0,};
    spi_rdid (id, sizeof (id));
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);

    // RDSR
    unsigned char status;
    spi_rdsr(&status);
    printf ("RDSR: status = %.2x \n", status);

    spi_wait_ready();
    // BE
    spi_be();
//    printf ("file size is %d \n", size);
    spi_wait_ready();
//    printf ("spi_be is over ... \n");

    tmp = size;
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
            return -1;
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
//    printf ("Start check ... \n");
    lseek (fd_file, 0, SEEK_SET);

    char read_data[CHECK_SIZE] = {0};
    char file_data[CHECK_SIZE] = {0};
    addr = 0;
    tmp = size;
    while (tmp / CHECK_SIZE)
    {
        memset (read_data, 0, CHECK_SIZE);
        memset (file_data, 0, CHECK_SIZE);
        res = spi_read(addr, read_data, sizeof (read_data));

        lseek (fd_file, addr, SEEK_SET);
        read (fd_file, file_data, res);
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

        lseek (fd_file, addr, SEEK_SET);
        read (fd_file, file_data, res);
        if (memcmp(file_data, read_data, CHECK_SIZE))
        {
            printf ("Write error");
            return -1;
        }
        tmp -= res;
        addr += res;
//        printf ("res = %d tmp = %d, addr = %d \n", res, tmp, addr);
    }
//    printf ("Ok ... \n");

    close (fd_file);
    spi_close();
    spi_cs_close();

    return 0;
}
