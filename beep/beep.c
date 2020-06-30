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
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#define     BEEPLED_CNT     1
#define     BEEPLED_NAME    "beep"

#define     BEEPON           0
#define     BEEPOFF          1

/* GPIOLED设备结构体 */
struct beep_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class* class;
    struct device* device;
    struct device_node* nd;
    int beep_gpio;
};

struct beep_dev beep; /* beep */

static int beep_open(struct inode* inode, struct file* flip) {
    flip->private_data = &beep;
    return 0;
}

static int beep_release(struct inode* inode, struct file* flip) {
    struct beep_dev* dev = (struct beep_dev*)flip->private_data;
    return 0;
}

static ssize_t beep_write(struct file* flip, const char __user* buf, size_t count, loff_t* ppos) {

    return 0;
}

static const struct file_operations beep_fops = {
    .owner      =   THIS_MODULE,
    .write      =   beep_write,
    .open       =   beep_open,
    .release    =   beep_release,
};

/* 驱动入口函数 */
static int __init beep_init(void) {

    int ret = 0;

    /* 注册字符设备驱动 */
    beep.major = 0;
    if (beep.major) {    /* 给定主设备号 */
        beep.major = MKDEV(beep.major, 0);
        ret = register_chrdev_region(beep.devid, BEEPLED_CNT, BEEPLED_NAME);
    } else {                /* 没给定设备号 */
        ret = alloc_chrdev_region(&beep.devid, 0, BEEPLED_CNT, BEEPLED_NAME);
        beep.major = MAJOR(beep.devid);
        beep.minor = MINOR(beep.devid);
    }

    if (ret < 0) {
        goto fail_devid;
    }

    printk("beep major = %d, minor = %d\r\n", beep.major, beep.minor);

    /* 初始化cdev */
    beep.cdev.owner = THIS_MODULE;
    cdev_init(&beep.cdev, &beep_fops);

    /* 添加cdev */
    ret = cdev_add(&beep.cdev, beep.devid, BEEPLED_CNT);
    if (ret) {
        goto fail_cdevadd;
    }

    /* 创建类 */
    beep.class = class_create(THIS_MODULE, BEEPLED_NAME);
    if (IS_ERR(beep.class)) {
        ret = PTR_ERR(beep.class);
        goto fail_class;
    }

    /* 创建设备 */
    beep.device = device_create(beep.class, NULL, beep.devid, NULL, BEEPLED_NAME);
    if (IS_ERR(beep.class)) {
        ret = PTR_ERR(beep.device);
        goto fail_device;
    }

    /* 初始化BEEP */
    beep.nd = of_find_node_by_path("/beep");
    if (NULL == beep.nd) {
        ret = -EINVAL;
        goto fail_nd;
    }

    beep.beep_gpio = of_get_named_gpio(beep.nd, "beep-gpios", 0);
    if (beep.beep_gpio < 0) {
        ret = -EINVAL;
        goto fail_nd;
    }

    ret = gpio_request(beep.beep_gpio, "beep-gpio");
    if (ret) {
        printk("Can't request beep gpio\r\n");
        goto fail_nd;
    }

    ret = gpio_direction_output(beep.beep_gpio, 0);
    if (ret < 0) {
        goto fail_set;
    }

    gpio_set_value(beep.beep_gpio, 0);

    return 0;

fail_set:
    gpio_free(beep.beep_gpio);
fail_nd:
    device_destroy(beep.class, beep.devid);
fail_device:
    class_destroy(beep.class);
fail_class:
    cdev_del(&beep.cdev);
fail_cdevadd:
    unregister_chrdev_region(beep.devid, BEEPLED_CNT);
fail_devid:
    return ret;
}


/* 驱动出口函数 */
static void __exit beep_exit(void) {

    /* 关闭设备 */
    gpio_set_value(beep.beep_gpio, 1);

    /* 注销字符设备驱动 */
    cdev_del(&beep.cdev);
    /* 释放设备号 */
    unregister_chrdev_region(beep.devid, BEEPLED_CNT);

    /* 摧毁设备 */
    device_destroy(beep.class, beep.devid);

    /* 摧毁类 */
    class_destroy(beep.class);

    /* 释放IO */
    gpio_free(beep.beep_gpio);
}

module_init(beep_init);
module_exit(beep_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("harrison");