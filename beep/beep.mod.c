#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x64cd2ee2, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xde436b34, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x42e7982c, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x38faf0a7, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x8b972ecd, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x6a67508d, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x7d8699b, __VMLINUX_SYMBOL_STR(of_find_node_opts_by_path) },
	{ 0x6ae5b6e8, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x8044ba6a, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xa3fefda0, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x2e5e2fe1, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x716b78dd, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x4ea3f103, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

