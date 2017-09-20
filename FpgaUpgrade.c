#include "spi.h"
// linux 读取spi flash代码linux SPI flash代码，linux读写spi flash
#define DEF_CHECK_SIZE  (4096)
#define DEF_SIZE        (DEF_CHECK_SIZE * 32)

#define DEF_SOFTWARE_SIZE (16 * 1024 * 1024)// our flash must be 16M byts to Fit FPGA!
#define DEF_SOFTWARE_OFFSET 0xa2

#define TTY_DEVICE "/dev/ttyS1"

int gFdKey = -1;//mcu file descriptior
int gFdTT=-1;
int gFdFile = -1;
int gHoleFileSize = -1;

int time_elapse(struct timeval t) {
    
    struct timeval now;
    int elapse = 0;
    gettimeofday(&now, NULL);
    elapse = 1000000 * (now.tv_sec - t.tv_sec) + now.tv_usec - t.tv_usec;
    return elapse;
}

//vol18Or33: 1,3.3v    0:1.8v
int set_IO_converter(int fd, int vol18Or33)
{
	int val = 1;
	if( vol18Or33 == 1 ){
		ioctl(fd, GPIO99_VOLTAGE_SELECT_PIN_HIGH, &val);
	}else{
		ioctl(fd, GPIO99_VOLTAGE_SELECT_PIN_LOW, &val);
	}
	printf ("[%s]:%d, set GPIO 99: %d!\n", __func__, __LINE__, vol18Or33);
	
    usleep(10000);
    
    return 	vol18Or33;
}

/** gFdFile: file fd
 *  sizeYouWish: the size you wish
 *  return: ok, return zero, if large then sizeYouWish: return 1, 
 *  else if less then size YouWish return -1
 */
 
int check_file_size(int fd_file, int sizeYouWish)////(0x1000000 + 0xa2)
{

    if (lseek (fd_file, 0, SEEK_END) < sizeYouWish){
		printf ("[%s]:%d, check file size Error! size should be more lagrger!\n", __func__, __LINE__);
		return -1;
    }else if (lseek (fd_file, 0, SEEK_END) > sizeYouWish){
		printf ("[%s]:%d, check file size Error! size should be less then this!\n", __func__, __LINE__);
		return 1;
	}
	return 0;
}


int spi_setup_and_open_ttdev (void)
{
    int fd_mtd;
    int res, val;
    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 6000000;
	mode = SPI_MODE_0;
	
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

//   res = spi_set_speed(speed);
//    if (res < 0)
//    {
//        printf ("spi_set_speed is failed. \n");
//        spi_close ();
//        return -1;
//    }

    gFdTT = spi_cs_open("/dev/tt");// open io ctl
    if (gFdTT <= 0)
    {
        printf ("open \"/dev/tt\" is failed. \n");
        return -1;
    }
	ioctl(gFdTT, GPIO99_VOLTAGE_SELECT_PIN_HIGH, &val);
	set_IO_converter(gFdTT, 1);
	ioctl(gFdTT, GPIO99_VOLTAGE_SELECT_PIN_HIGH, &val);
	printf ("[%s]:%d setup SPI ok, gFDTT:%d \n", __func__, __LINE__, gFdTT);
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
    printf ("[%s]:%d lseeking file \n", __func__, __LINE__);
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
            //printf ("spi is busy ... \n");
            usleep (10000);
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
/*int fd: mcu file fd
 * baud: 115200, 8 bit, one stop bit
 * 
 */
void init_serial(int *fd)
{
	struct termios newtermios;
	
	printf ("[%s]:%d, begin to open tty1\n", __func__, __LINE__);
	
	*fd = open(TTY_DEVICE, O_RDWR | O_NOCTTY );
	if (*fd < 0) {
		perror(TTY_DEVICE); 
		return ;
	}
	
	printf ("[%s]:%d, termios struct initiliza\n", __func__, __LINE__);
	bzero(&newtermios, sizeof(newtermios));
	tcgetattr(*fd, &newtermios);
	newtermios.c_cc[VMIN] = 1;
	newtermios.c_iflag = 0;
	newtermios.c_oflag = 0;
	newtermios.c_lflag = 0; 
	newtermios.c_cflag	= B115200 | CS8 | PARENB | CLOCAL | CREAD;
	
	printf ("[%s]:%d, finished initialize terios, 115200, 8bit\n", __func__, __LINE__);
	if (tcsetattr(*fd, TCSANOW, &newtermios)) {
		perror("tcsetattr   error");  
		exit(1);  
	}
	return ;
}

int my_memcmp(const char *s1, const char *s2, int len)
{
	int i=0;
	do
	{
		if(*(s1+i) != *(s2+i))
		{
			printf("[%s]%d: not equal addr: %d", __func__, __LINE__, i);
			break;
		}
		
	}while((++i)<len);
	
	if(i == len)
	{
		return 0;
	}
	else
	{
		return i;
	};
}

void read_spi_flash_ID(char *id)
{
    spi_rdid (id, 3);
    printf ("ID:id = %.2x %.2x %.2x \n", id[0], id[1], id[2]);
    if (id[0] != 0x20)
    {
        printf ("[%s]%d: Can not find SPI FLASH. \n", __func__, __LINE__);
        return;
    }
    return;
}

void set_mcu_bus(int serial_fd)
{
	char writeData = 0xaa ;
    write(serial_fd, &writeData , 1) ;
    usleep(10000);
    printf ("[%s]:%d, serial OK, 0xaa sent, begin to setup SPI, FD:%d\n", __func__, __LINE__, serial_fd);
}

/* int size: buffer size
 * 
 * 
 */
char *get_buffer_and_init(int size)
{
	//size -= SOFTWARE_OFFSET;// hole file to the end
	size = DEF_SOFTWARE_SIZE;//我们的FPGA只能用 16 M bytes， jic文件的原因
	printf("new size(useful data) is:%d\n", size); //out FPGA size must be 16M Bytes!
	char *buff = malloc (size);//申请size大小的内存用于存放问及内容
    if (NULL == buff)
    {
		perror ("malloc");
        return NULL;
    }
	memset (buff, 0, size);//初始化申请的内容
	return buff;
}

/* wirte buffer data to spi flash
 *  SIZE MUST BE 16Mb
 * buff: buffer to be writen
 * size: the size to be writen
 * return: zero is ok, none zero error
 */
int write_all_spi_flash(char *buff, int size)
{
	int res = -1;
	int addr = 0;    
	while (addr < size)
    {
        spi_wait_ready();
        if ((size - addr) < DEF_SIZE)
        {
            res = spi_write(addr, buff + addr, size - addr);
            if (res < 0)
            {
				printf ("[%s]:%d, Write Error 1, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }
        else 
        {
            res = spi_write(addr, buff + addr, DEF_SIZE);
            if (res < 0)
            {
				printf ("[%s]:%d, Write Error2, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }

        addr += res;
        printf ("Write flash addr = %x H, res: %d\n", addr, res);
	}
	return 0;
}

int read_all_spi_flash(char *dst_buff, int size)
{
	int addr = 0;
	int res = -1;
	//read spi flash data
    while (addr < size)
    {
        if ((size - addr) < DEF_CHECK_SIZE)
        {
            res = spi_read(addr, dst_buff + addr, size - addr);
            if (res < 0)
            {
				printf ("[%s]:%d, Read Error 1, res: %d !\n", __func__, __LINE__, res);
                return addr;
            } 
        }
        else
        {
	        res = spi_read(addr, dst_buff + addr, DEF_CHECK_SIZE);
            if (res < 0)
            {
				printf ("[%s]:%d, Read Error 4K Bytes, res: %d !\n", __func__, __LINE__, res);
                return addr;
            }
        }
        addr += res;
        spi_wait_ready();
        printf ("(Read!) flash addr = %x H\n", addr);
    }
    return 0;
}


int memcpy_by_block(char *buff, char *buff1, int size)
{
	///////////////////// linux比较内存函数， liux spi flash 操作 ////////////////////////
	int addr = 0;
	int tmpAddr= 0 ;
	int i=0;
	int cnt = 0;
	int res = -1;
	while (addr < size){
		if(tmpAddr == addr){
			if(++cnt > 5){
				return addr;
				}
			}
		if ((size - addr) < DEF_CHECK_SIZE){
			if (res = my_memcmp (buff+addr, buff1+addr, size - addr))//buff is orgin file, buff1 read form flash;
				{
				printf ("[%s]:%d, check Error 1*! buff and buff1 not equal, addr is:%x H(%d), res is:%d!\n", __func__, __LINE__, addr, addr, res);
				tmpAddr = addr;
				spi_wait_ready();
				if(0 < spi_write(addr, buff + addr, size - addr)){//try to write flash again 3 times
						printf ("[%s]:%d, check Error 1_*! rewrite ok, addr is:%x H(%d)\n", __func__, __LINE__, addr, addr);
						spi_wait_ready();
						spi_read(addr, buff1 + addr, size - addr);
						continue;
					}else{
						usleep(10000);
						spi_wait_ready();
						if(0 < spi_write(addr, buff + addr, size - addr)){
							spi_wait_ready();
							spi_read(addr, buff1 + addr, size - addr);
							continue;
						}else{
							usleep(10000);
							spi_wait_ready();
							if(0 < spi_write(addr, buff + addr, size - addr)){
								spi_wait_ready();
								spi_read(addr, buff1 + addr, size - addr);
								continue;
								}
							}
					}
					
				return addr;
				}
			
			}else{
				
			if(res = my_memcmp (buff+addr, buff1+addr, DEF_CHECK_SIZE))//check size is 4K bytes
				{
				tmpAddr = addr;
				printf ("[%s]:%d, check Error 2**! buff and buff1 not equal, addr is:%x H(%d), res is:%d!\n", __func__, __LINE__, addr, addr, res);
				spi_wait_ready();
				if(0 < spi_write(addr, buff + addr, DEF_CHECK_SIZE)){//try to write flash again 3 times
						printf ("[%s]:%d, check Error 2**! rewrite ok, addr is:%x H(%d)\n", __func__, __LINE__, addr, addr);
						spi_wait_ready();
						spi_read(addr, buff1 + addr, DEF_CHECK_SIZE);
						continue;
					}else{
						usleep(10000);
						spi_wait_ready();
						if(0 < spi_write(addr, buff + addr, DEF_CHECK_SIZE)){
							printf ("[%s]:%d, check Error 2__**! rewrite (2) ok, addr is:%x H(%d)\n", __func__, __LINE__, addr, addr);
							spi_wait_ready();
							spi_read(addr, buff1 + addr, DEF_CHECK_SIZE);
							continue;
						}else{
							usleep(10000);
							spi_wait_ready();
							if(0 < spi_write(addr, buff + addr, DEF_CHECK_SIZE)){
								spi_wait_ready();
								spi_read(addr, buff1 + addr, DEF_CHECK_SIZE);
								continue;
								}
							}
					}
				return addr;//after 3 times writen, no success, error!
				}
		}
		
		if(res != 0){//when mem is ok, return zero!
			i++;
			if(i>8){
				i=0;
				printf ("[%s]:%d, check Error 3***! error cnt overflow, res is:%d!\n", __func__, __LINE__, res);
				return addr;
			}
		}else{
			i=0;
			addr+=DEF_CHECK_SIZE;
		}
         printf ("(check!) flash addr = %x H\n", addr);
	}
	return 0;
}

/* fd_file: file descriptor
 * dst_buf: the dest buffer to read from file;
 * size: the size to read
 * 
 * 
 */
int read_file_to_buff(int fd_file, char * dst_buf, int size)
{
	int addr = 0;
	int res = -1;
	
	while (addr != size)//begin to read file from USB
    {
        res = read (fd_file, dst_buf + addr, size);
        if (res <= 0)
        {
			printf ("xxx>> read file ERROR! addr: %x H, Actuly read file size = %d Bytes\n", addr, res);
            return addr;
        }
        addr += res;
        printf ("read file final addr(size): %x H(%d), Actuly read file size = %d Bytes\n", addr, addr, res);
    }
    return 0;
}   

//main形式参数的用法main参数用法main函数形参argc argv用法
int main (int argc, char *argv[])
{
	int i;
	
    if (argc < 2){
        printf ("Usage: %s <FilePath> \n", argv[0]);
        return -1;
    }
	
	init_serial(&gFdKey); if(gFdKey <= 0){return -1;}
	printf("**Serial Port open, gFdKey OK!: %d.\n", gFdKey);
	
	set_mcu_bus(gFdKey);// let spi bus switch!
	
	set_IO_converter(gFdTT, 1);
	
    spi_setup_and_open_ttdev();//open /dev/tt, open spi and config;
    
    gFdFile = file_operate (argv[1], &gHoleFileSize);//open file
    if (gFdFile < 0) { printf ("file_operate is failed. \n");return -1;}
    
    printf ("[%s]:%d, gFdFile OK. gFdFile:%d \n", __func__, __LINE__, gFdFile);
	
	//if(check_file_size(gFdFile, (0x1000000 + 0xa2)) != 0) { return -1;}
	
	printf ("[%s]:%d, begin to read id\n", __func__, __LINE__);
    // RDID, funny , must write lake that!!! 只能这样写,
    char id[3] = {0,};
    read_spi_flash_ID(id);

    spi_wait_ready();
    
    // BE, block erase 擦除所有的flash
    spi_be(); printf ("Erasing Flsah!! Hole file size is %d \n", gHoleFileSize);
    
    spi_wait_ready();//wait for erase ok
	
	// malloc buffer for file
	char *buff = get_buffer_and_init(DEF_SOFTWARE_SIZE);

    lseek(gFdFile, DEF_SOFTWARE_OFFSET, SEEK_SET);
	
	if(read_file_to_buff(gFdFile, buff, DEF_SOFTWARE_SIZE) != 0) { goto error_label; }

    for (i = 0; i < DEF_SOFTWARE_SIZE; i++){
        buff[i] = convert (buff[i]);
        }
        
	printf ("[%s]:%d, step... \n", __func__, __LINE__);
	
	// write hole spi flash
    write_all_spi_flash(buff, DEF_SOFTWARE_SIZE);
    
    printf ("Start check ... \n");

	char *buff1 = get_buffer_and_init(DEF_SOFTWARE_SIZE);
	
	printf ("[%s]:%d, Start check ... \n", __func__, __LINE__);
	
	if( read_all_spi_flash(buff1, DEF_SOFTWARE_SIZE) != 0 ){ goto error_label; }
	
	if( memcpy_by_block(buff, buff1, DEF_SOFTWARE_SIZE) != 0) { goto error_label; }

//------------------- fininsed spi operation, deal with result  --------------------//	
	set_IO_converter(gFdTT, 0); 
    printf ("[%s]:%d, Main loop ! All OK .. \n", __func__, __LINE__);

	if(buff1)
		free (buff1);
	if(buff)
		free (buff);
    close (gFdFile);
    close (gFdTT);
    spi_close();
    spi_cs_close();
    return 0;

error_label:
	set_IO_converter(gFdTT, 0);
    printf ("[%s]:%d, Main loop ! ERROR LABEL .. \n", __func__, __LINE__);
    
	if(buff1)
		free (buff1);
	if(buff)
		free (buff);
		
    close (gFdFile);
    close (gFdTT);
    spi_close();
    spi_cs_close();
	return -1;
}
