#include <asm/uaccess.h>
#include <linux/init.h>  // ​Макросы __init и ​__exit
#include <linux/init_task.h>
#include <linux/module.h>   // MODULE_LICENSE, MODULE_AUTHOR
#include <linux/proc_fs.h>  // proc_create
#include <linux/sched.h>
#include <linux/seq_file.h> /* for seq_file */
#include <linux/string.h>
#include <linux/version.h>  // KERNEL_VERSION
#include <linux/vmalloc.h>

MODULE_AUTHOR("Sofiya");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fortune Cookie/Sequences Kernel Module");

#define _BUF_SIZE PAGE_SIZE

static char *_cookie_pot;  // буфер

static unsigned read_inx;
static unsigned write_inx;

static struct proc_dir_entry *proc_entry, *proc_dir, *sym_link;
char tmp[256];

static void *fortune_seq_start(struct seq_file *s, loff_t *pos) {
  printk(KERN_INFO "Module: seq_start\n");
  static unsigned long counter = 0;

  if (*pos == 0) {
    return &counter;
  } else {
    *pos = 0;
    return NULL;
  }
}

static void *fortune_seq_next(struct seq_file *s, void *v, loff_t *pos) {
  printk(KERN_INFO "Module: seq_next\n");
  (*pos)++;
  return NULL;
}

static void fortune_seq_stop(struct seq_file *s, void *v) {
  printk(KERN_INFO "Module: seq_stop\n");
}

static int fortune_seq_show(struct seq_file *s, void *v) {
  printk(KERN_INFO "Module: seq_show\n");
  int len = 0;

  if (read_inx >= write_inx) {
    read_inx = 0;
  }

  len = sprintf(tmp, "%s\n", &_cookie_pot[read_inx]);
  seq_printf(s, "%s", tmp);

  read_inx += len;
  return 0;
}

static struct seq_operations fortune_seq_ops = {
  .start = fortune_seq_start,
  .next = fortune_seq_next,
  .stop = fortune_seq_stop,
  .show = fortune_seq_show
};

static int fortune_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Module: вызвана fortune_open\n");
  return seq_open(file, &fortune_seq_ops);
};


ssize_t fortune_write(struct file *filp, const char __user *buf, size_t count,
                      loff_t *offp) {
  int space_left = (_BUF_SIZE - write_inx) + 1;

  if (space_left < count) {
    printk(KERN_INFO "Module: Буфер полон!");
    return -ENOSPC;
  }
  if (copy_from_user(&_cookie_pot[write_inx], buf, count)) {
    return -EFAULT;
  }

  write_inx += count;
  _cookie_pot[write_inx - 1] = '\0';

  printk(KERN_INFO "Module: write");
  return count;
}

static struct proc_ops fortune_file_ops = {
  .proc_open = fortune_open,
  .proc_read = seq_read,
  .proc_write = fortune_write,
  .proc_lseek = seq_lseek,
  .proc_release = seq_release
};

static int __init fortune_init(void) {
  _cookie_pot = (char *)vmalloc(_BUF_SIZE);
  if (!_cookie_pot) {
    printk(KERN_INFO "Module: Недостаточно памяти!\n");
    return -ENOMEM;
  }
  memset(_cookie_pot, 0, _BUF_SIZE);

  proc_entry =
      proc_create("fortune_seq", S_IRUGO | S_IWUGO, NULL, &fortune_file_ops);
  if (!proc_entry) {
    vfree(_cookie_pot);
    printk(KERN_INFO "Module: Не удалось создать файл в proc!\n");
    return -ENOMEM;
  }

  proc_dir = proc_mkdir("fortune_dir", NULL);
  sym_link = proc_symlink("fortune_symlink", NULL, "/proc/fortune_dir");

  if ((proc_dir == NULL) || (sym_link == NULL)) {
    vfree(_cookie_pot);
    printk(KERN_INFO "Module: Не удалось создать дирректорию или симв. ссылку!\n");
    return -ENOMEM;
  }

  read_inx = 0;
  write_inx = 0;

  printk(KERN_INFO "Module: Модуль загружен в ядро!\n");
  return 0;
}

static void __exit fortune_exit(void) {
  remove_proc_entry("fortune_seq", NULL);
  remove_proc_entry("fortune_symlink", NULL);
  remove_proc_entry("fortune_dir", NULL);

  if (_cookie_pot) {
    vfree(_cookie_pot);
  }

  printk(KERN_INFO "Module: Модуль выгружен из ядра!\n");
}

module_init(fortune_init);
module_exit(fortune_exit);