#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h>
#include<linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include<linux/kthread.h>
#include<linux/delay.h>
#include<linux/jiffies.h>
#include<linux/timer.h>
#include<linux/proc_fs.h>

#define TIMEOUT 5000 //millisecond
#define TAILLE 100

#define DRIVER_AUTHOR "Nassim&Adrian"
#define DRIVER_DESC "Hello world Module"
#define DRIVER_LICENSE "GPL"
#define BUF_LEN 64

static char *buffer;
static int param;
static int major = 254;
struct timer_list mytimer;
char message[];

module_param(param, int, 0);
MODULE_PARM_DESC(param, "Un paramètre de ce module");

static int my_open_function(struct inode *inode, struct file *file);
static int my_release_function(struct inode *inode, struct file *file);
static ssize_t my_read_function(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t my_write_function(struct file *file, char *buf, size_t count, loff_t *ppos);
void timer_callback(struct timer_list *timer);

static int proc_open(struct inode *inode, struct file *file);
static int proc_release(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *file, char __user * buffer,size_t count, loff_t * ppos);
static ssize_t proc_write(struct file * file, const char __user * buffer,size_t count, loff_t * ppos);









void timer_callback(struct timer_list *timer)
{
	printk(KERN_INFO"in Timer callback function ");

	// Re-enable the timer which will make this as periodic timer

	mod_timer(&mytimer,msecs_to_jiffies(TIMEOUT));

}

static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open  = proc_open,
  .read  = proc_read,
};

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_read_function,
        .write          = my_write_function,

};


static ssize_t my_read_function(struct file *file, char *buf, size_t count, loff_t *ppos) {
        int lus = 0;
        printk(KERN_DEBUG "read: demande lecture de %d octets\n", count);
        /* Check for overflow */
        if (count <= BUF_LEN - (int)*ppos)
        lus = count;
        else lus = BUF_LEN - (int)*ppos;
        if(lus)
        copy_to_user(buf, (int *)buffer + (int)*ppos, lus);
        *ppos += lus;
        printk(KERN_DEBUG "read: %d octets reellement lus\n", lus);
        printk(KERN_DEBUG "read: position=%d\n", (size_t)*ppos);
        return lus;
}

static ssize_t my_write_function(struct file *file, char *buf, size_t count, loff_t *ppos)
{
            int ecrits = 0, i = 0;
            printk(KERN_DEBUG "write: demande ecriture de %d octets\n", count);
            if (count <= BUF_LEN - (int)*ppos) /* Check for overflow */
            ecrits = count;
            else ecrits = BUF_LEN - (int)*ppos;
            if(ecrits)
            copy_from_user((int *)buffer + (int)*ppos, buf, ecrits);
            *ppos += ecrits;
            printk(KERN_DEBUG "write: %d octets reellement ecrits\n", ecrits);
            printk(KERN_DEBUG "write: position=%d\n", (int)*ppos);
            printk(KERN_DEBUG "write: contenu du buffer\n");
            for(i=0;i<BUF_LEN;i++)
            printk(KERN_DEBUG " %d", buffer[i]);
            printk(KERN_DEBUG "\n");
            return ecrits;
}


static ssize_t proc_write(struct file * file, const char __user * buffer,size_t count, loff_t * ppos)
{
            unsigned int size_of_message=0;
            int len = count;
            if (len > TAILLE) len = TAILLE;
            printk(KERN_INFO "Recieving new messag\n");
            if (copy_from_user(message, buffer, count)) {
            return -EFAULT;
            }
            message[count] = '\0';
            size_of_message = strlen(message);
            printk(KERN_INFO "New message : %s\n", message);
            return count;
}
static ssize_t proc_read(struct file *file, char __user * buffer,size_t count, loff_t * ppos)
{
	char message[100];
	int errno=0;
	int copy;
	if (count > TAILLE) count=TAILLE;
	if ((copy=copy_to_user(buffer, message, strlen(message))))
		errno = -EFAULT;
	printk(KERN_INFO "message read, %d, %p\n", copy, buffer);
	return count-copy;
}



static int __init  hello_init(void)
{

    int ret;

    //ret = register_chrdev(major, "mydriver", &fops);

    if(ret < 0)
    {
        printk(KERN_WARNING "Probleme sur la majeur\n");
        return ret;
    }
    proc_create("myProc", 0, NULL,&proc_fops);
    setup_timer(&mytimer,timer_callback,0);
    mod_timer(&mytimer,jiffies + msecs_to_jiffies(TIMEOUT));

    printk(KERN_DEBUG "mydriver chargé avec succès\n");
	printk(KERN_DEBUG "le paramètre est=%d\n", param);

	return 0;
}
static void __exit  hello_exit(void)
{
//unregister_chrdev(major, "mydriver");
printk(KERN_DEBUG "mydriver déchargé avec succès\n");
remove_proc_entry("myProc", NULL);
del_timer(&mytimer);


}



module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
