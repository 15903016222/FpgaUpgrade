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

#define GPIO109_LOW     0x8888
#define GPIO109_HIGH    0x8889


static long gpio_ioctl (struct file* file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case GPIO99_HIGH:
            printk ("set gpio99 1 \n");
            gpio_request (99, "spi_en");
            gpio_direction_output (99, 1);
            gpio_set_value (99, 1);
            break;

        case GPIO100_HIGH:
            printk ("set gpio100 1 \n");
            gpio_request (100, "CAM_D1");
            gpio_direction_output (100, 1);
            gpio_set_value (100, 1);
            break;

        case GPIO101_HIGH:
            printk ("set gpio101 1 \n");
            gpio_request (101, "CAM_D2");
            gpio_direction_output (101, 1);
            gpio_set_value (101, 1);
            break;

        case GPIO109_HIGH:
            printk ("set gpio109 1 \n");
            gpio_request (109, "CAM_D2");
            gpio_direction_output (109, 1);
            gpio_set_value (109, 1);
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
	misc_register (&gpio_misc);
    
    return 0;
}

void __exit omap_gpio_exit(void)
{
	misc_deregister (&gpio_misc);
    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan gpio module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(omap_gpio_init);
module_exit(omap_gpio_exit);
