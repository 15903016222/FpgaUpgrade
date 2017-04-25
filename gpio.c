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
#define GPIO21_LOW      0x6006
#define GPIO21_HIGH     0x6007

#define GPIO109_HIGH    0x6008
#define GPIO109_LOW     0x6009
#define GPIO109_HIGH_PULL 0x6010
#define GPIO109_LOW_PULL  0x6011

#define GPIO21_HIGH_PULL 0x6012
#define GPIO21_LOW_PULL  0x6013

#define CONTROL_PADCONF_CAM_D9  0x48002128
#define GPIO4_OE                0x49054034
#define GPIO4_DATAOUT           0x4905403c

#define CONTROL_PADCONF_ETK_D6  0x480025e8
#define GPIO1_OE                0x48310034
#define GPIO1_DATAOUT           0x4831003c

static long gpio_ioctl (struct file* file, unsigned int cmd, unsigned long arg)
{
    int val = 0;
    unsigned int data;

    switch (cmd) {
        case GPIO109_HIGH:
 //           printk ("set gpio109 led on \n");
            data = (unsigned int)ioremap (CONTROL_PADCONF_CAM_D9, 4);
            *(unsigned int *)data &= 0xffff;
            *(unsigned int *)data |= 1 << 18;
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_OE, 4);
            *((unsigned int *)data) &= ~(1 << 13);
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_DATAOUT, 4);
            *((unsigned int *)data) |= 1 << 13;
            iounmap ((void *)data);
            break;

        case GPIO109_LOW:
            data = (unsigned int)ioremap (GPIO4_DATAOUT, 4);
            *((unsigned int *)data) &= ~(1 << 13);
            iounmap ((void *)data);
            break;

        case GPIO109_HIGH_PULL:
            data = (unsigned int)ioremap (CONTROL_PADCONF_CAM_D9, 4);
            *(unsigned int *)data &= 0xffff;
            *((unsigned int *)data) |= 1 << 18 | 1 << 19 | 1 << 20;
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_OE, 4);
            *((unsigned int *)data) |= (1 << 13);
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_DATAOUT, 4);
            *((unsigned int *)data) &= ~(1 << 13);
            iounmap ((void *)data);
            break;

        case GPIO109_LOW_PULL:
            data = (unsigned int)ioremap (CONTROL_PADCONF_CAM_D9, 4);
            *((unsigned int *)data) &= ~(1 << 18 | 1 << 19 | 1 << 20);
            break;

        case GPIO99_HIGH:
//            printk ("set gpio99 1 \n");
            data = (unsigned int)ioremap (0x48002114, 4);
            *(unsigned int *)data = 1 << 18 | 1 << 19 | 1 << 20;
            iounmap ((void *)data);
            data = 0;
//            printk ("gpio99 is set 1 over ... \n");
            break;

        case GPIO99_LOW:
//            printk ("set gpio99 0 \n");
            data = (unsigned int)ioremap (0x48002114, 4);
            *(unsigned int *)data &= ~(1 << 18 | 1 << 19 | 1 << 20);
            iounmap ((void *)data);
            data = 0;
//            printk ("gpio99 is set 0 over ... \n");
            break;

        case GPIO21_HIGH:
 //           printk ("set gpio21 1 \n");
            data = (unsigned int)ioremap (CONTROL_PADCONF_ETK_D6, 4);
            *(unsigned int *)data &= 0xffff;
            *(unsigned int *)data |= 1 << 18;
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO1_OE, 4);
            *((unsigned int *)data) &= ~(1 << 21);
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO1_DATAOUT, 4);
            *((unsigned int *)data) |= 1 << 21;
            iounmap ((void *)data);
            break;

        case GPIO21_LOW:
//            printk ("set gpio21 0 \n");
            data = (unsigned int)ioremap (GPIO1_DATAOUT, 4);
            *((unsigned int *)data) &= ~(1 << 21);
            iounmap ((void *)data);
            break;

        case GPIO21_HIGH_PULL:
            data = (unsigned int)ioremap (CONTROL_PADCONF_ETK_D6, 4);
            *(unsigned int *)data &= 0xffff;
            *((unsigned int *)data) |= 1 << 18 | 1 << 19 | 1 << 20;
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_OE, 4);
            *((unsigned int *)data) |= (1 << 21);
            iounmap ((void *)data);
            data = (unsigned int)ioremap (GPIO4_DATAOUT, 4);
            *((unsigned int *)data) &= ~(1 << 21);
            iounmap ((void *)data);
            break;

        case GPIO21_LOW_PULL:
            data = (unsigned int)ioremap (CONTROL_PADCONF_ETK_D6, 4);
            *((unsigned int *)data) &= ~(1 << 18 | 1 << 19 | 1 << 20);
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
