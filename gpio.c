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
#define GPIO100_LOW     0x6006
#define GPIO100_HIGH    0x6007
#define GPIO101_LOW     0x6008
#define GPIO101_HIGH    0x6009
#define GPIO170_LOW     0x6010
#define GPIO170_HIGH    0x6011
#define GPIO170_INPUT   0x6012

#define GPIO109_LOW     0x8888
#define GPIO109_HIGH    0x8889

#define GPIO_READ       0x1000


static long gpio_ioctl (struct file* file, unsigned int cmd, unsigned long arg)
{
    int val = 0;
    switch (cmd) {
        case GPIO99_HIGH:
            printk ("set gpio99 1 \n");
            gpio_set_value (99, 1);
            break;

        case GPIO99_LOW:
            printk ("set gpio99 0 \n");
            gpio_set_value (99, 0);
            break;

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
            break;
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
    gpio_request (99, "spi_en");
    gpio_direction_output (99, 0);
    gpio_request (100, "cam_d1");
    gpio_direction_output (100, 0);
    gpio_request (101, "cam_d2");
    gpio_direction_output (101, 0);
    gpio_request (109, "led");
    gpio_direction_output (109, 0);
    gpio_request (170, "test");
    gpio_direction_output (170, 0);

	misc_register (&gpio_misc);
    printk ("char device gpio register successed. \n");

    return 0;
}

void __exit omap_gpio_exit(void)
{
    gpio_free (99);
    gpio_free (100);
    gpio_free (101);
    gpio_free (109);
    gpio_free (170);

	misc_deregister (&gpio_misc);
    printk ("char device gpio unregister. \n");

    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan gpio module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(omap_gpio_init);
module_exit(omap_gpio_exit);
