#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x8e4729ed, "struct_module" },
	{ 0x1bcd461f, "_spin_lock" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xf17212d3, "create_proc_entry" },
	{ 0x26e96637, "request_irq" },
	{ 0x9eac042a, "__ioremap" },
	{ 0x4c3af445, "__request_region" },
	{ 0x1acd2470, "register_chrdev" },
	{ 0x5feba87d, "dma_alloc_coherent" },
	{ 0x19cacd0, "init_waitqueue_head" },
	{ 0x86cb9d9f, "__mutex_init" },
	{ 0x19070091, "kmem_cache_alloc" },
	{ 0xab978df6, "malloc_sizes" },
	{ 0x94ef8c0d, "pci_enable_device" },
	{ 0x3b531300, "pci_get_subsys" },
	{ 0xdf2126f8, "mutex_unlock" },
	{ 0xd9f7b1a3, "mutex_lock" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x865edc9b, "finish_wait" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0xc8f02aeb, "prepare_to_wait" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x45c077f2, "_spin_lock_irq" },
	{ 0x59968f3c, "__wake_up" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x1e8eafee, "wake_up_process" },
	{ 0x1b7d4074, "printk" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xa22a96d3, "remove_proc_entry" },
	{ 0x37a0cba, "kfree" },
	{ 0x2fd1d81c, "vfree" },
	{ 0xc871cb49, "dma_free_coherent" },
	{ 0x8bb33e7d, "__release_region" },
	{ 0xdc3eaf70, "iomem_resource" },
	{ 0xedc03953, "iounmap" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "09C80E3CD4B3114EF71EA69");
