#include "spi.h"

void delay (int m) {
    int i, j;
    for (i = 0; i < m; ++i) {
        for (j = 0; j < 10000; ++j){
            ;
        }
    }
}

void pabort (const char *s) {
    perror(s);
    abort();
}
int spi_cs_high (int fd) {
    if (fd < 0) {
        pabort("spi_cs_low");
    }

    int val;

    if (ioctl(fd, GPIO21_HIGH, &val) < 0) {
        close (fd);
        return -1;
    }

    return 0;
}

int spi_cs_low(int fd) {
    if (fd < 0) {
        pabort("spi_cs_low");
    }

    int val;

    spi_cs_high(fd);
    delay (2);
    if (ioctl(fd, GPIO21_LOW, &val) < 0) {
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_rdid (int fd, char *id, size_t size) {
    if (fd < 0 || NULL == id || size < 0) {
        pabort("spi_rdid");
    }

    uint8_t cmd = RDID;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror("Write RDID");
        close (fd);
        return -1;
    }
    if ((read (fd, id, size)) < 0) {
        perror ("read id");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_rdsr (int fd, char *status) {
    if (fd < 0 || NULL == status) {
        pabort("spi_rdsr");
    }

    uint8_t cmd = RDSR;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write RDSR");
        close (fd);
        return -1;
    }

    if ((read (fd, status, 1)) < 0) {
        perror ("read RDSR");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_wrsr (int fd, uint8_t reg) {
    if (fd < 0) {
        pabort("spi_wrsr");
    }

    uint8_t cmd = WRSR;
    uint8_t status = reg;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write WRSR");
        close (fd);
        return -1;
    }

    if ((write (fd, &status, sizeof (status))) < 0) {
        perror ("Write WRSR");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_wren (int fd) {
    if (fd < 0) {
        pabort("spi_wren");
    }

    uint8_t cmd = WREN;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write WREN");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_se (int fd, unsigned int address) {
    if (fd < 0) {
        pabort("spi_se");
    }

    uint8_t cmd = SE;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write SE");
        close (fd);
        return -1;
    }

    uint8_t addr;
    // H8
    addr = (address & 0x00ffffff) >> 16;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write H8 addr");
        close (fd);
        return -1;
    }
    // M8
    addr = (address & 0x00ffffff) >> 8;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write M8 addr");
        close (fd);
        return -1;
    }
    // L8
    addr = address & 0x00ffffff;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write L8 addr");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_be (int fd) {
    if (fd < 0) {
        pabort("spi_se");
    }

    uint8_t cmd = BE;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write BE");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_read (int fd, unsigned int address, char *data, size_t size) {
    if (fd < 0 || address < 0) {
        pabort("spi_read");
    }
    if (NULL == data || size < 0) {
        pabort("spi_read");
    }

    uint8_t cmd = READ;
    uint8_t addr;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write READ");
        close (fd);
        return -1;
    }

    // H8
    addr = (address & 0x00ffffff) >> 16;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write H8 addr");
        close (fd);
        return -1;
    }
    // M8
    addr = (address & 0x0000ffff) >> 8;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write M8 addr");
        close (fd);
        return -1;
    }
    // L8
    addr = address & 0x000000ff;
    if ((write (fd, &addr, 1)) < 0) {
        perror ("Write L8 addr");
        close (fd);
        return -1;
    }

    if ((read (fd, data, size)) < 0) {
        perror ("read READ");
        close (fd);
        return -1;
    }

    return 0;
}

int  spi_pp (int fd, unsigned int address, char *data, size_t size) {
    if (fd < 0 || address < 0) {
        pabort("spi_pp");
    }
    if (NULL == data || size < 0) {
        pabort("spi_pp");
    }

    uint8_t cmd = PP;
    uint8_t addr;

    if ((write (fd, &cmd, sizeof (cmd))) < 0) {
        perror ("Write PP");
        close (fd);
        return -1;
    }

    // H8
    addr = address >> 16;
    if ((write (fd, &addr, sizeof (addr))) < 0) {
        perror ("Write H8 addr");
        close (fd);
        return -1;
    }

    // M8
    addr = address >> 8;
    if ((write (fd, &addr, sizeof (addr))) < 0) {
        perror ("Write M8 addr");
        close (fd);
        return -1;
    }

    // L8
    addr = address;
    if ((write (fd, &addr, sizeof (addr))) < 0) {
        perror ("Write L8 addr");
        close (fd);
        return -1;
    }

    if ((write (fd, data, size)) < 0) {
        perror ("Write data");
        close (fd);
        return -1;
    }

    return 0;
}


