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

/**
 * 模块入口函数
 */

static int __init dtsof_init(void) {
    int ret = 0;

    struct device_node* bl_nd;/* 节点 */
    /* 1：找到backlight的节点，路径是/backlight */
    bl_nd = of_find_node_by_path("/backlight");
    if (bl_nd == NULL) {
        printk(KERNEL_DEBUG, "No /backlight node.\n");
        ret = -EINVAL;
        goto fail_findnd;
    }

    /* 2：获取属性 */
    struct property* comppro;
    comppro = of_find_property(bl_nd, "compatible", NULL);
    if (comppro == NULL) {
        printk(KERNEL_DEBUG, "No /backlight node.\n");
        ret = -EINVAL;
        goto fail_findpro;
    } else {
        printk("compatible = %s\r\n", (char*)comppro->value);
    }


    char name[10][20];
    ret = of_property_read_string(bl_nd, "status", name);
    if (ret < 0) {
        goto fail_rs;
    } else {
        printk("status = %s\r\n", name[0]);
    }


    /* 3：获取数字属性值 */
    u32 def_value;
    ret = of_property_read_u32(bl_nd, "default-brightness-level", &def_value);
    if (ret < 0) {
        goto fail_read32;
    } else {
        printk("default-brightness-level = %ud", def_value);
    }

    /* 4：获取数组类型的属性 */
    u32 elementsize = 0;
    elementsize = of_property_count_elems_of_size(bl_nd, "brightness-levels", sizeof(u32));
    if (elementsize < 0) {
        ret = -EINVAL;
        goto fail_readele;
    } else {
        printk("brightness-levels elems size = %ud", elementsize);
    }

    /* 内存申请 */
    u32* brival;
    brival = kmolloc(elementsize * sizeof(u32), GFP_KERNEL);
    if (!brival) {
        ret = -EINVAL;
        goto fail_mem;
    }

    /* 读取数组 */
    ret = of_property_read_u32_array(bl_nd, "brightness-levels", brival, elementsize);
    if (ret < 0) {
        goto fail_read32array;
    } else {
        for (int i = 0; i < elementsize; i++) {
            /* code */
            printk("brightness-levels[%d] = %d\r\n", i, brival[i]);
        }
    }
    kfree(brival);

    return 0;
    
fail_read32array:
    kfree(brival);
fail_mem;
fail_read32:
fail_rs:
fail_findpro:
fail_findnd:
    return ret;
}


/**
 * 模块出口函数
*/
static void __exit dtsof__exit(void) {
}

module_init(dtsof_init);
module_exit(dtsof_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("harrison")