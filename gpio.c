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
            iounmap ((void *)data);
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
            iounmap ((void *)data);
            printk ("gpio99 is set 0 over ... \n");
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
    printk ("char device gpio register successed. \n");

    return 0;
}

void __exit omap_gpio_exit(void)
{
	misc_deregister (&gpio_misc);
    printk ("char device gpio unregister. \n");

    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan gpio module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(omap_gpio_init);
module_exit(omap_gpio_exit);
