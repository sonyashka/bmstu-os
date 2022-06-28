#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <linux/lockdep.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sophia");

#define IRQ_NUM 1
#define IRQ_NAME "keyboard"
#define WQ_NAME "keyboard_wq"
#define ITER_COUNT 100000000

#define KBD_DATA_REG 0x60
#define KBD_SCANCODE_MASK 0x7f
#define KBD_STATUS_MASK 0x80

static char *ascii_map[] = {
    "[ESC]", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "bs", "[Tab]", "Q",
    "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "[Enter]", "[CTRL]", "A", "S", "D",
    "F", "G", "H", "J", "K", "L", ";", "\'", "`", "[LShift]", "\\", "Z", "X", "C", "V", "B", "N", "M",
    ",", ".", "/", "[RShift]", "[PrtSc]", "[Alt]", " ", "[Caps]", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "[Num]", "[Scroll]", "[Home(7)]", "[Up(8)]", "[PgUp(9)]", "-",
    "[Left(4)]", "[Center(5)]", "[Right(6)]", "+", "[End(1)]", "[Down(2)]", "[PgDn(3)]", "[Ins]", "[Del]"};

static struct workqueue_struct *my_wq;

typedef struct
{
  struct work_struct work;
  int work_num;
} my_work_t;

static my_work_t *work_1;
static my_work_t *work_2;

char scancode;
int status;
char *key;

void my_work_1(struct work_struct *work)
{
  printk(KERN_INFO "workqueue: work1 %s is pressed", key);
}

void my_work_2(struct work_struct *work)
{
  msleep(100);
  printk(KERN_INFO "workqueue: work2 exit from sleep");
}

irqreturn_t my_irq_handler(int irq_num, void *dev_id)
{
  scancode = inb(KBD_DATA_REG);
  status = scancode & KBD_STATUS_MASK;
  if (!(status))
  {
    key = ascii_map[(scancode & KBD_SCANCODE_MASK) - 1];
  }

  if (irq_num == IRQ_NUM)
  {
    if (work_1)
      queue_work(my_wq, (struct work_struct *)work_1);
    if (work_2)
      queue_work(my_wq, (struct work_struct *)work_2);
    return IRQ_HANDLED;
  }
  return IRQ_NONE;
}

static int __init my_module_init(void)
{
  if (request_irq(IRQ_NUM, my_irq_handler, IRQF_SHARED, IRQ_NAME, my_irq_handler))      
  {
    printk(KERN_ERR "workqueue: request_irq error\n");
    return -ENOMEM;
  }
  printk(KERN_INFO "workqueue: IRQ handler registered\n");

  work_1 = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
  work_2 = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);

  if (work_1)
  {
    INIT_WORK((struct work_struct *)work_1, my_work_1);
    work_1->work_num = 1;
  }
  else
  {
    printk(KERN_ERR "workqueue: kalloc error\n");
    return -ENOMEM;
  }

  if (work_2)
  {
    INIT_WORK((struct work_struct *)work_2, my_work_2);
    work_2->work_num = 2;
  }
  else
  {
    printk(KERN_ERR "workqueue: kalloc error\n");
  }

  my_wq = create_workqueue(WQ_NAME);
  if (!my_wq)
  {
    free_irq(IRQ_NUM, my_irq_handler);
    printk(KERN_ERR "workqueue: create_workqueue error\n");
    return -ENOMEM;
  }
  printk(KERN_INFO "workqueue: created\n");

  printk(KERN_INFO "workqueue: module loaded!\n");
  return 0;
}

static void __exit my_module_exit(void)
{
  free_irq(IRQ_NUM, my_irq_handler);

  flush_workqueue(my_wq);
  destroy_workqueue(my_wq);

  if (work_1)
    kfree(work_1);

  if (work_2)
    kfree(work_2);

  printk(KERN_INFO "workqueue: module unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);