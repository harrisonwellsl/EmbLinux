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

#define     GPIOLED_CNT     1
#define     GPIOLED_NAME    "gpioled"

#define     LEDON           0
#define     LEDOFF          1

/* GPIOLED设备结构体 */
struct gpioled_dev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class* class;
    struct device* device;
    struct device_node* nd;
    int led_gpio;
};

struct gpioled_dev gpioled; /* led */

static int led_open(struct inode* inode, struct file* flip) {
    flip->private_data = &gpioled;
    return 0;
}

static int led_release(struct inode* inode, struct file* flip) {
    struct gpioled_dev* dev = (struct gpioled_dev*)flip->private_data;
    return 0;
}

static ssize_t led_write(struct file* flip, const char __user* buf, size_t count, loff_t* ppos) {
    int ret;
    unsigned char databuf[1];
    struct gpioled_dev* dev = flip->private_data;

    ret  = copy_from_user(databuf, buf, count);
    if (ret < 0) {
        return -EINVAL;
    }

    if (databuf == LEDON) {
        gpio_set_value(gpioled.led_gpio, 0);
    } else if (databuf == LEDOFF) {
        gpio_set_value(gpioled.led_gpio, 1);
    }

    return 0;
}

static const struct file_operations led_fops = {
    .owner      =   THIS_MODULE,
    .write      =   led_write,
    .open       =   led_open,
    .release    =   led_release,
};

/* 驱动入口函数 */
static int __init led_init(void) {

    int ret = 0;

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

    /* 获取设备节点 */
    gpioled.nd = of_find_node_by_path("/gpioled");
    if (gpioled.nd == NULL) {
        ret = -EINVAL;
        goto fail_findnd;
    }

    /* 获取LED所对应的GPIO */
    gpioled.led_gpio = of_get_named_gpio(gpioled.nd, "led-gpios", 0);
    if (gpioled.nd < 0) {
        printk("Can't find a node led-gpios\r\n");
        ret = -EINVAL;
        goto fail_findnd;
    }

    printk("led gpio num = %d\r\n", gpioled.led_gpio);

    /* 申请IO */
    ret = gpio_request(gpioled.led_gpio, "led-gpio");
    if (ret) {
        printk("Failed to request the led gpio");
        ret = -EINVAL;
        goto fail_findnd;
    }
    
    /* 使用IO，设置为输出 */
    ret = gpio_direction_output(gpioled.led_gpio, 1);
    if (ret < 0) {
        goto fail_setoutput;
    }

    /* 设置低电平，点亮LED灯 */
    gpio_set_value(gpioled.led_gpio, 0);

    return 0;

fail_setoutput:
    gpio_free(gpioled.led_gpio);
fail_findnd:
    return ret;
}


/* 驱动出口函数 */
static void __exit led_exit(void) {
    /* 关灯 */
    gpio_set_value(gpioled.led_gpio, 1);

    /* 注销字符设备驱动 */
    cdev_del(&gpioled.cdev);
    /* 释放设备号 */
    unregister_chrdev_region(gpioled.devid, GPIOLED_CNT);

    /* 摧毁设备 */
    device_destroy(gpioled.class, gpioled.devid);

    /* 摧毁类 */
    class_destroy(gpioled.class);

    /* 释放IO */
    gpio_free(gpioled.led_gpio);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("harrison");