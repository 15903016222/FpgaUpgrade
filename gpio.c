//#include <linux/config.h>
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/device.h>

#include <linux/io.h>

#include <plat/gpmc.h>
#include <linux/timer.h>

#include <linux/workqueue.h>

#include <linux/gpio.h>

#define GPIO99_LOW      0x6004
#define GPIO99_HIGH     0x6005
/*
#define GPIO100_LOW     0x6006
#define GPIO100_HIGH    0x6007
#define GPIO101_LOW     0x6008
#define GPIO101_HIGH    0x6009
#define GPIO170_LOW     0x6010
#define GPIO170_HIGH    0x6011
#define GPIO170_INPUT   0x6012
#define GPIO184_LOW     0x6013
#define GPIO184_HIGH    0x6014
#define GPIO185_LOW     0x6015
#define GPIO185_HIGH    0x6016
#define GPIO158_LOW     0x6017
#define GPIO158_HIGH    0x6018

#define GPIO109_LOW     0x8888
#define GPIO109_HIGH    0x8889

#define GPIO_READ       0x1000
*/

static long gpio_ioctl (struct file* file, unsigned int cmd, unsigned long arg)
{
    int val = 0;
    unsigned int data;

    switch (cmd) {
        case GPIO99_HIGH:
            printk ("set gpio99 1 \n");
            data = (unsigned int)ioremap (0x48002114, 4);
            *(unsigned int *)data |= 1 << 19 | 1 << 20 | 1 << 24;
            gpio_request (99, "spi_en");
            gpio_direction_output (99, 0);
            gpio_set_value (99, 1);
            gpio_free (99);
            iounremap (void *data);
            printk ("gpio99 is set 1 over ... \n");
            break;

        case GPIO99_LOW:
            printk ("set gpio99 0 \n");
            data = (unsigned int)ioremap (0x48002114, 4);
            *(unsigned int *)data |= 1 << 19 | 1 << 20 | 1 << 24;
            gpio_request (99, "spi_en");
            gpio_direction_output (99, 0);
            gpio_set_value (99, 0);
            gpio_free (99);
            iounremap (void *data);
            printk ("gpio99 is set 0 over ... \n");
            break;
/*
        case GPIO100_HIGH:
            printk ("set gpio100 1 \n");
            gpio_set_value (100, 1);
            break;

        case GPIO100_LOW:
            printk ("set gpio100 0 \n");
            gpio_set_value (100, 0);
            break;

        case GPIO101_HIGH:
            printk ("set gpio101 1 \n");
            gpio_set_value (101, 1);
            break;

        case GPIO101_LOW:
            printk ("set gpio101 0 \n");
            gpio_set_value (101, 0);
            break;

        case GPIO109_HIGH:
            printk ("set gpio109 1 \n");
            gpio_set_value (109, 1);
            break;

        case GPIO109_LOW:
            printk ("set gpio109 0 \n");
            gpio_set_value (109, 0);
            break;

        case GPIO170_HIGH:
            printk ("set gpio170 1 \n");
            gpio_set_value (170, 1);
            break;

        case GPIO170_LOW:
            printk ("set gpio170 0 \n");
            gpio_set_value (170, 0);
            break;

        case GPIO170_INPUT:
            printk ("set gpio170 input \n");
            val = 9;
            gpio_direction_input (170);
            val = gpio_get_value (170);
            printk ("gpio170 input val = %d", val);
            break;

        case GPIO184_HIGH:
            printk ("set gpio184 1 \n");
            gpio_set_value (184, 1);
            break;

        case GPIO184_LOW:
            printk ("set gpio184 0 \n");
            gpio_set_value (184, 0);
            break;

        case GPIO185_HIGH:
            printk ("set gpio185 1 \n");
            gpio_set_value (185, 1);
            break;

        case GPIO185_LOW:
            printk ("set gpio185 0 \n");
            gpio_set_value (185, 0);
            break;

        case GPIO158_HIGH:
            printk ("set gpio158 1 \n");
            gpio_set_value (158, 1);
            break;

        case GPIO158_LOW:
            printk ("set gpio158 0 \n");
            gpio_set_value (158, 0);
            break;

        case GPIO_READ:
            printk ("\n");
            val = 9;
            val = gpio_get_value (99);
            printk ("gpio99 val = %d \n", val);
            val = 9;
            val = gpio_get_value (100);
            printk ("gpio100 val = %d \n", val);
            val = 9;
            val = gpio_get_value (101);
            printk ("gpio101 val = %d \n", val);
            val = 9;
            val = gpio_get_value (109);
            printk ("gpio109 val = %d \n", val);
            val = 9;
            val = gpio_get_value (170);
            printk ("gpio170 val = %d \n", val);
            val = 9;
            val = gpio_get_value (184);
            printk ("gpio184 val = %d \n", val);
            val = 9;
            val = gpio_get_value (158);
            printk ("gpio158 val = %d \n", val);
            val = 9;
            break;
*/
        default:
            break;
    }

	return 0; 
}

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
	.unlocked_ioctl = gpio_ioctl,
};

static struct miscdevice gpio_misc = {
    .minor = MISC_DYNAMIC_MINOR,
	.name  = "gpiodrv",
	.fops  = &gpio_fops,
};

int __init omap_gpio_init(void)
{
/*    unsigned int data;

    printk ("gpio99 is setting ... \n");
    data = (unsigned int)ioremap (0x48002114, 4);
    *(unsigned int *)data |= 1 << 19 | 1 << 20 | 1 << 24;
    gpio_request (99, "spi_en");
    gpio_direction_output (99, 0);
    gpio_set_value (99, 1);
    printk ("gpio99 is set over ... \n");

    gpio_request (100, "cam_d1");
    gpio_direction_output (100, 0);
    gpio_request (101, "cam_d2");
    gpio_direction_output (101, 0);

    printk ("gpio109 is setting ... \n");
    data = (unsigned int)ioremap (0x48002128, 4);
    *(unsigned int *)data |= 1 << 19 | 1 << 20 | 1 << 24;
    gpio_request (109, "led");
    gpio_direction_output (109, 0);
    gpio_set_value (109, 1);
    printk ("gpio109 is set over ... \n");

    gpio_request (170, "test");
    gpio_direction_output (170, 0);
    gpio_request (184, "i2c_scl");
    gpio_direction_output (184, 0);
    gpio_request (185, "i2c_scd");
    gpio_direction_output (185, 0);
    gpio_request (158, "spi4_miso");
//    gpio_direction_output (158, 0);
    gpio_direction_input (158);
*/
	misc_register (&gpio_misc);
    printk ("char device gpio register successed. \n");

    return 0;
}

void __exit omap_gpio_exit(void)
{
/*    gpio_free (99);
    gpio_free (100);
    gpio_free (101);
    gpio_free (109);
    gpio_free (170);
    gpio_free (184);
    gpio_free (185);
    gpio_free (158);
*/
	misc_deregister (&gpio_misc);
    printk ("char device gpio unregister. \n");

    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan gpio module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(omap_gpio_init);
module_exit(omap_gpio_exit);
