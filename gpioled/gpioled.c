#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#define     GPIOLED_CNT     1
#define     GPIOLED_NAME    "gpioled"

/* GPIOLED设备结构体 */
struct gpioled_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class* class;
    struct device* device;
};

struct gpioled_dev gpioled; /* led */

static const struct file_operations led_fops = {
    .owner      =   THIS_MODULE,
    .write      =   led_write,
    .open       =   led_open,
    .release    =   led_release,
}

/* 驱动入口函数 */
static int __init led_init(void) {

    /* 注册字符设备驱动 */
    gpioled.major = 0;
    if (gpioled.major) {    /* 给定主设备号 */
        gpioled.major = MKDEV(gpioled.major, 0);
        register_chrdev_region(gpioled.devid, GPIOLED_CNT, GPIOLED_NAME);
    } else {                /* 没给定设备号 */
        alloc_chrdev_region(&gpioled.devid, 0, GPIOLED_CNT, GPIOLED_NAME);
        gpioled.major = MAJOR(gpioled.devid);
        gpioled.minor = MINOR(gpioled.devid);
    }

    printk("gpioled major = %d, minor = %d\r\n", gpioled.major, gpioled.minor);

    /* 初始化cdev */
    gpioled.cdev.owner = THIS_MODULE;
    cdev_init(&gpioled.cdev, &led_fops);

    /* 添加cdev */
    cdev_add(&gpioled.cdev, gpioled.devid, GPIOLED_CNT);

    /* 创建类 */
    gpioled.class = class_create(THIS_MODULE, GPIOLED_NAME);
    if (IS_ERR(gpioled.class)) {
        return PTR_ERR(gpioled.class);
    }

    /* 创建设备 */
    gpioled.device = device_create(gpioled.class, NULL, gpioled.devid, NULL, GPIOLED_NAME);
    if (IS_ERR(gpioled.class)) {
        return PTR_ERR(gpioled.device);
    }

    /*  */

    return 0;
}


/* 驱动出口函数 */
static void __exit led_exit(void) {
    /* 注销字符设备驱动 */
    cdev_del(&gpioled.cdev);
    /* 释放设备号 */
    unregister_chrdev_region(gpioled.devid, GPIOLED_CNT);

    /* 摧毁设备 */
    device_destroy(gpioled.class, gpioled.devid);

    /* 摧毁类 */
    class_destroy(gpioled.class);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("harrison");