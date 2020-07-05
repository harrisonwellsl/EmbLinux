#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harrison");

static char* whom = "world";
static int howmany = 1;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static int __init hello_init(void) {
    printk("hello world\n");
    return 0;
}

static void __exit hello_exit(void) {
    printk("goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);