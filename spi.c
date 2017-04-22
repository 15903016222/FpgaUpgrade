#include "spi.h"

int fd_spi, fd_cs;

void delay (int m)
{
    int i, j;
    for (i = 0; i < m; ++i)
    {
        for (j = 0; j < 10000; ++j)
        {
            ;
        }
    }
}

int  spi_open (const char *path)
{
    if (NULL == path)
    {
        perror ("spi_open");
        return -1;
    }

    fd_spi = open (path, O_RDWR);
    if (fd_spi < 0)
    {
        return -1;
    }

    return fd_spi;
}

int  spi_set_mode (uint8_t mode)
{
    int res;

    if ((res = (ioctl(fd_spi, SPI_IOC_RD_MODE, &mode))) < 0)
    {
        return -1;
    }
    if ((res = (ioctl(fd_spi, SPI_IOC_WR_MODE, &mode))) < 0) {
        return -1;
    }

    return res;
}

int  spi_set_bits (uint8_t bits)
{
    int res;

    if ((res = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits)) < 0)
    {
        return -1;
    }
    if ((res = ioctl(fd_spi, SPI_IOC_RD_BITS_PER_WORD, &bits)) < 0)
    {
        return -1;
    }

    return res;
}

int  spi_set_speed (uint32_t speed)
{
    int res;

    if ((res = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed)) < 0)
    {
        return -1;
    }
    if ((res = ioctl(fd_spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed)) < 0)
    {
        return -1;
    }

    return res;
}

void spi_close (void)
{
    close (fd_spi);
}

int spi_cs_open (const char *path)
{
    if (NULL == path)
    {
        return -1;
    }

    fd_cs = open (path, O_RDWR);
    if (fd_cs < 0)
    {
        return -1;
    }

    return fd_cs;
}

int spi_cs_high (void)
{
    if (fd_cs < 0)
    {
        return -1;
    }

    int val, res;

    if ((res = ioctl(fd_cs, GPIO21_HIGH, &val)) < 0)
    {
        return -1;
    }

    return res;
}

int spi_cs_low(void)
{
    if (fd_cs < 0)
    {
        return -1;
    }

    int val, res;

    spi_cs_high();
    delay (2);
    if ((res = ioctl(fd_cs, GPIO21_LOW, &val)) < 0)
    {
        return -1;
    }

    return res;
}

void spi_cs_close (void)
{
    close (fd_cs);
}

int  spi_rdid (char *id, size_t size)
{
    if (fd_spi < 0 || NULL == id || size < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = RDID;

    spi_cs_low ();
    if ((res = (write (fd_spi, &cmd, sizeof (cmd)))) < 0)
    {
        return -1;
    }
    if ((res = (read (fd_spi, id, size))) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_rdsr (char *status)
{
    if (fd_spi < 0 || NULL == status)
    {
        return -1;
    }

    int res;
    uint8_t cmd = RDSR;

    spi_cs_low ();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    if ((res = read (fd_spi, status, 1)) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_wrsr (uint8_t reg)
{
    if (fd_spi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = WRSR;
    uint8_t status = reg;

    spi_cs_low();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    if ((res = write (fd_spi, &status, sizeof (status))) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_wren (void)
{
    if (fd_spi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = WREN;

    spi_cs_low();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_se (unsigned int address)
{
    if (fd_spi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = SE;

    spi_cs_low();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    uint8_t addr;
    // H8
    addr = (address & 0x00ffffff) >> 16;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }
    // M8
    addr = (address & 0x00ffffff) >> 8;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }
    // L8
    addr = address & 0x00ffffff;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_be (void)
{
    if (fd_spi < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = BE;

    spi_cs_low();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_read (unsigned int address, char *data, size_t size)
{
    if (fd_spi < 0 || address < 0)
    {
        return -1;
    }
    if (NULL == data || size < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = READ;
    uint8_t addr;

    spi_cs_low();
    if ((write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    // H8
    addr = (address & 0x00ffffff) >> 16;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }
    // M8
    addr = (address & 0x0000ffff) >> 8;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }
    // L8
    addr = address & 0x000000ff;
    if ((res = write (fd_spi, &addr, 1)) < 0)
    {
        return -1;
    }

    if ((res = (read (fd_spi, data, size))) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_pp (unsigned int address, char *data, size_t size)
{
    if (fd_spi < 0 || address < 0)
    {
        return -1;
    }
    if (NULL == data || size < 0)
    {
        return -1;
    }

    int res;
    uint8_t cmd = PP;
    uint8_t addr;

    spi_cs_low();
    if ((res = write (fd_spi, &cmd, sizeof (cmd))) < 0)
    {
        return -1;
    }

    // H8
    addr = address >> 16;
    if ((res = write (fd_spi, &addr, sizeof (addr))) < 0)
    {
        return -1;
    }

    // M8
    addr = address >> 8;
    if ((res = write (fd_spi, &addr, sizeof (addr))) < 0)
    {
        return -1;
    }

    // L8
    addr = address;
    if ((res = write (fd_spi, &addr, sizeof (addr))) < 0)
    {
        return -1;
    }

    if ((res = write (fd_spi, data, size)) < 0)
    {
        return -1;
    }
    spi_cs_high();

    return res;
}

int  spi_write (unsigned int address, char *data, size_t size)
{
    int cnt = 0;
    int res = 0;
    int tmp = size;
    unsigned int addr = address;

    while (tmp / 256)
    {
        res = spi_pp (address + cnt, data + cnt, 256);
        if (res < 0)
        {
            return -1;
        }
        tmp -= res;
        cnt += res;
    }

    while (tmp % 256)
    {
        res = spi_pp (address + cnt, data + cnt, (tmp % 256));
        if (res < 0)
        {
            return -1;
        }
        tmp -= res;
        cnt += res;
    }

    return cnt;
}

int  spi_is_busy (void)
{
    char status;

    if (spi_rdsr(&status) < 0)
    {
        return -1;
    }

    return status & 0x0;
}

