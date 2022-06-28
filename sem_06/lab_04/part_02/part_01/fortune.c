// https://github.com/soarpenguin/kernel-module/blob/master/proc_Fortune.c
// https://elixir.bootlin.com/linux/v5.4/source/include/linux/fs.h

#include <linux/module.h> // MODULE_LICENSE, MODULE_AUTHOR
#include <linux/kernel.h> // KERN_INFO
#include <linux/init.h>	  // ​Макросы __init и ​__exit
#include <linux/vmalloc.h>

#include <linux/proc_fs.h> // proc_create
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sofiya");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");

#define COOKIE_BUF_SIZE PAGE_SIZE

static char *cookie_buf;
static unsigned read_index;
static unsigned write_index;
static struct proc_dir_entry *proc_entry;

char tmp[256];

static ssize_t fortune_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t fortune_read(struct file *, char __user *, size_t, loff_t *);
int fortune_open(struct inode *, struct file *);
int fortune_release(struct inode *, struct file *);

static struct proc_ops fops =
{
		.proc_read = fortune_read,
		.proc_write = fortune_write,
		.proc_open = fortune_open,
		.proc_release = fortune_release,
};

int fortune_open(struct inode *sp_inode, struct file *sp_file)
{
	printk(KERN_INFO "Module : Вызван fortune_open\n");
	return 0;
}

int fortune_release(struct inode *sp_node, struct file *sp_file)
{
	printk(KERN_INFO "Module : Вызван fortune_release\n");
	return 0;
}

static ssize_t fortune_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int space_available = (COOKIE_BUF_SIZE - write_index) + 1;

	if (space_available < count)
	{
		printk(KERN_INFO "Module : Буфер полон!");
		return -ENOSPC;
	}

	if (copy_from_user(&cookie_buf[write_index], buf, count))
	{
		return -EFAULT;
	}

	write_index += count;
	cookie_buf[write_index - 1] = 0;

	printk(KERN_INFO "Module : Запись в файл");
	return count;
}

static ssize_t fortune_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos)
{
	int len;

	if (*f_pos > 0)
		return 0;

	if (read_index >= write_index)
		read_index = 0;

	len = 0;

	if (write_index > 0)
	{
		len = sprintf(tmp, "%s\n", &cookie_buf[read_index]);
		copy_to_user(buf, tmp, len);
		buf += len;
		read_index += len;
	}

	*f_pos += len;

	printk(KERN_INFO "Module : чтение из файла\n");
	return len;
}

static int __init fortune_init(void)
{
	cookie_buf = (char *)vmalloc(COOKIE_BUF_SIZE);

	if (!cookie_buf)
	{
		printk(KERN_INFO "Module : Недостаточно памяти!\n");
		return -ENOMEM;
	}

	memset(cookie_buf, 0, COOKIE_BUF_SIZE);

	proc_entry = proc_create("fortune", 0666, NULL, &fops);
	if (!proc_entry)
	{
		vfree(cookie_buf);
		printk(KERN_INFO "Module : Не удалось создать файл в proc!\n");
		return -ENOMEM;
	}

	read_index = 0;
	write_index = 0;

	proc_mkdir("my_dir_fortune", NULL);
	proc_symlink("my_symlink_fortune", NULL, "/proc/fortune");

	printk(KERN_INFO "Module : Модуль загружен в ядро!\n");
	return 0;
}

static void __exit fortune_exit(void)
{
	remove_proc_entry("fortune", NULL);
	remove_proc_entry("my_symlink_fortune", NULL);
	remove_proc_entry("my_dir_fortune", NULL);

	if (cookie_buf)
		vfree(cookie_buf);

	printk(KERN_INFO "Module : Модуль выгружен из ядра!\n");
}

module_init(fortune_init);
module_exit(fortune_exit);