#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mpouzol");
MODULE_DESCRIPTION("Fortytwo misc device driver.");

static int misc_open(struct inode *inode, struct file *file) {
	return 0;
}

static int misc_close(struct inode *inodep, struct file *filp) {
	return 0;
}

static ssize_t misc_write(struct file *file, const char __user *buf,
			  size_t len, loff_t *pppos)
{
	if (strncmp("mpouzol", buf, 5) == 0) {
		return 6;
	}
	return -EINVAL;
}

static ssize_t misc_read(struct file *filep, char *buf, size_t len, 
			 loff_t *offset)
{
	char *login = "mpouzol";
	ssize_t bytes = len < (5-(*offset)) ? len : (5-(*offset));
	if (copy_to_user(buf, login, bytes)) {
		return -EFAULT;
	}
	(*offset) += bytes;
	return bytes;
}

static const struct file_operations misc_fops = {
	.owner	  =  THIS_MODULE,
	.open     =  misc_open,
	.write    =  misc_write,
	.read     =  misc_read,
	.release  =  misc_close,
	.llseek   =  no_llseek,
};

struct miscdevice misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "fortytwo",
	.fops  = &misc_fops
};

static int __init misc_init(void)
{
	if (misc_register(&misc_device))
		printk(KERN_ERR "Unable register misc device");
	return 0;
}
module_init(misc_init);

static void __exit misc_exit(void)
{
	misc_deregister(&misc_device);
}
module_exit(misc_exit);