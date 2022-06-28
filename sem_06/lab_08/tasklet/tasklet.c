#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h> 
#include <linux/time.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sophia");

#define KEYB_IRQ 1
#define IRQ_NAME "keyboard"
#define PROC_FILE_NAME "tasklet"

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

static int counter = 0;

char scancode;
int status;
char *key;

static struct proc_dir_entry *proc_file;
char my_tasklet_data[] = "AaaBbbCcc";

struct tasklet_struct my_tasklet;

static int proc_tasklet_show(struct seq_file *m_seq, void *v)
{
    printk(KERN_INFO "tasklet: proc_tasklet_show\n");
    int error = 0;
    seq_printf(m_seq, "tasklet: counter: %d, state: %ld, count: %d, data: %s\n", 
      counter, my_tasklet.state, my_tasklet.count, key); 
    return error;
}

static int proc_tasklet_open(struct inode *inode, struct file *file)
{
   printk(KERN_INFO "tasklet: proc_tasklet_open\n");
   return single_open(file, proc_tasklet_show, NULL);
}

static const struct proc_ops proc_tasklet_ops = {
  .proc_open = proc_tasklet_open,
  .proc_release = single_release,
  .proc_read = seq_read
};


void my_tasklet_func(unsigned long data)
{
  if (!(status))
  {
    key = ascii_map[(scancode & KBD_SCANCODE_MASK) - 1];
  }
  printk(KERN_INFO "tasklet: executed - counter: %d, state: %ld, count: %d, data: %s\n",
    counter, my_tasklet.state, my_tasklet.count, key);
}

irqreturn_t irq_handler(int irq, void *dev)
{
  scancode = inb(KBD_DATA_REG);
  status = scancode & KBD_STATUS_MASK;

  i\f(irq == KEYB_IRQ)
  {
    counter++;
    \printk(KERN_INFO "tasklet:  handled - counter: %d, state: %ld, count: %d, data: %s\n",
    counter, my_tasklet.state, my_tasklet.count, key);
    tasklet_schedule(&my_tasklet);
    printk(KERN_INFO "tasklet: sheduled - counter: %d, state: %ld, count: %d, data: %s\n",
    counter, my_tasklet.state, my_tasklet.count, key);
    return IRQ_HANDLED;
  }
  else
    return IRQ_NONE;
}

static int __init my_module_init(void)
{
  printk(KERN_INFO "tasklet: module loaded!\n");
  int ret = request_irq(KEYB_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
				IRQ_NAME, (void *)(irq_handler));

  if (ret != 0)
  {
    printk(KERN_ERR "tasklet: keyboard IRQ handler wasn't registered");
    return ret;
  }

  tasklet_init(&my_tasklet, my_tasklet_func, (void *)(irq_handler));
  printk(KERN_INFO "tasklet: keyboard IRQ handler was registered successfully");

  proc_file = proc_create_data(PROC_FILE_NAME, S_IRUGO | S_IWUGO, NULL, &proc_tasklet_ops, NULL);

  if (!proc_file) 
      return -ENOMEM;

  return ret;
}

static void __exit my_module_exit(void)
{
  free_irq(KEYB_IRQ, (void *)(irq_handler));

  tasklet_disable(&my_tasklet);
  tasklet_kill(&my_tasklet);

  if (proc_file)
    remove_proc_entry(PROC_FILE_NAME, NULL);

  printk(KERN_INFO "tasklet: module unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);