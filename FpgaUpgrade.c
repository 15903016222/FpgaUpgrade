#include "spi.h"

#define CHECK_SIZE  (4096)
#define SIZE        (CHECK_SIZE * 32)

#define SOFTWARE_SIZE (16 * 1024 * 1024)
#define SOFTWARE_OFFSET 0xa2

int spi_setup (void)
{
    int fd_mtd, fd_tt;
    int res, val;
    static uint8_t mode;
    static uint8_t bits = 8;
//    static uint32_t speed = 500000;

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

/*    res = spi_set_speed(speed);
    if (res < 0)
    {
        printf ("spi_set_speed is failed. \n");
        spi_close ();
        return -1;
    }
*/
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
            printf ("spi is busy ... \n");
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

	time_t rawtime;
	struct tm *timeinfo;

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

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	printf ("system time : %s \n", asctime (timeinfo));

    printf ("Start upgrading fpga ... \n");

    spi_wait_ready();
    // BE
    spi_be();
//    printf ("file size is %d \n", size);
    spi_wait_ready();
//    printf ("spi_be is over ... \n");

    tmp = SOFTWARE_SIZE;
    lseek (fd_file, SOFTWARE_OFFSET, SEEK_SET);

	char *buff = malloc (SOFTWARE_SIZE);
    if (NULL == buff)
    {
		perror ("malloc");
        goto err2;
    }
	memset (buff, 0, SOFTWARE_SIZE);
    tmp = 0;
	while (tmp != SOFTWARE_SIZE)
    {
        res = read (fd_file, buff + tmp, SOFTWARE_SIZE);
        if (res < 0)
        {
             goto err1;
        }
        tmp += res;
        printf ("read file tmp = %x \n", tmp);
    }

	// byte convert
    for (i = 0; i < SOFTWARE_SIZE; i++)
    {
        buff[i] = convert (buff[i]);
    }
	
	// write to spi flash
	addr = 0;    
	while (addr < SOFTWARE_SIZE)
    {
        spi_wait_ready();
        if ((SOFTWARE_SIZE - addr) < SIZE)
        {
            res = spi_write(addr, buff + addr, SOFTWARE_SIZE - addr);
            if (res < 0)
            {
                 goto err1;
            }
        }
        else 
        {
            res = spi_write(addr, buff + addr, SIZE);
            if (res < 0)
            {
                 goto err1;
            }
        }

        addr += res;
//        printf ("write flash addr = %x\n", addr);
    }
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	printf ("system time : %s \n", asctime (timeinfo));
    
    printf ("Start check ... \n");
	char *buff1 = malloc (SOFTWARE_SIZE);
    if (NULL == buff1)
    {
         perror ("malloc");
         goto err1;
    }
	memset (buff1, 0, SOFTWARE_SIZE);

	addr = 0;
    while (addr < SOFTWARE_SIZE)
    {
        if ((SOFTWARE_SIZE - addr) < CHECK_SIZE)
        {
            res = spi_read(addr, buff1 + addr, SOFTWARE_SIZE - addr);
            if (res < 0)
            {
                 goto err;
            } 
        }
        else 
        {
	        res = spi_read(addr, buff1 + addr, CHECK_SIZE);
            if (res < 0)
            {
                goto err;
            }
        }
        addr += res;
//        printf ("write flash addr = %x\n", addr);
    }

    if (memcmp (buff, buff1, SOFTWARE_SIZE))
    {
         printf ("Check failed ... \n");
         goto err;
    }

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	printf ("system time : %s \n", asctime (timeinfo));

    printf ("Ok! \n");
	free (buff1);
	free (buff);
    close (fd_file);
    spi_close();
    spi_cs_close();
    return 0;

err:
	free (buff1);
err1:
	free (buff);
err2:
    close (fd_file);
    spi_close();
    spi_cs_close();
	return -1;
}
