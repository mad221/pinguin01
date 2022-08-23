#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpouzol");
MODULE_DESCRIPTION("Useless module");

char str[PAGE_SIZE];
char tmp[PAGE_SIZE];

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);



static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read  = myfd_read,
	.write = myfd_write,
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "reverse",
	.fops  = &myfd_fops,
};

static int __init myfd_init(void)
{
    if ( misc_register(&myfd_device))
        printk("cannot register misc modules");
    else
        printk("register on  misc modules");
    return 0;
}

static void __exit myfd_cleanup(void) 
{
    misc_deregister(&myfd_device);
    printk("deregister on  misc modules");
}


ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
    int t = 0;
    int i = 0;
 
    for (t = strlen(str) - 1; t != -1; t--, i++)
    {
        tmp[i] = str[t];
    }
    tmp[i] = 0x0;
    return copy_to_user(user, tmp, i);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs)
{
    ssize_t res = copy_from_user(str, user, size) + 1;
    str[size + 1] = 0x0;
    return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);