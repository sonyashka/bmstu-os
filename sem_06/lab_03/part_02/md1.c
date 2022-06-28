// Вызываемый модуль ядра Linux
#include <linux/init.h>	  // ​Макросы __init и ​__exit
#include <linux/module.h> // MODULE_LICENSE, MODULE_AUTHOR

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sophia");

char *md1_data = "AAA";

extern char *md1_proc(void)
{
	return md1_data;
}

static char *md1_local(void)
{
	return md1_data;
}

extern char *md1_noexport(void)
{
	return md1_data;
}

EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);

static int __init md_init(void)
{
	printk(KERN_INFO "+ module md1 start!\n");
	return 0;
}

static void __exit md_exit(void)
{
	printk(KERN_INFO "+ module md1 unloaded!\n");
}

module_init(md_init);
module_exit(md_exit);