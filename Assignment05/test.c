#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define BUF "mpouzol"
#define BUF_LEN 8 

MODULE_DESCRIPTION("Misc Driver");
MODULE_AUTHOR("mpouzol");
MODULE_LICENSE("GPL");

static int open(struct inode *inode, struct file *file)
{
    return 0;
}

static int close(struct inode *inodep, struct file *filp)
{
    return 0;
}

static ssize_t write(struct file *f, const char *buf, size_t len, loff_t *offset)
{
	char buffer[BUF_LEN];

        if (len != BUF_LEN) 
        {
            return (-EFAULT);
        }

        if (copy_from_user(buffer, buf, BUF_LEN) != 0)
        {
            return (-EFAULT);
        }
        
        if (strncmp(buffer, BUF, BUF_LEN) == 0)
        {
            return (BUF_LEN);
        }
        else
        {
            return (-EFAULT);
        }
}

static ssize_t read(struct file *f, char *buffer, size_t length, loff_t *offset)
{
	int res;
	char *read_from = BUF + *offset;
	size_t read_num = length < (BUF_LEN - *offset) ? length : (BUF_LEN - *offset);

	if (read_num == 0)
		return (0);

	if ((res = copy_to_user(buffer, read_from, read_num)) == read_num)
		return (-1);
	else
	{
		*offset = BUF_LEN - res;
		return (read_num - res);
	}
}


static const struct file_operations fops = {
    .owner			= THIS_MODULE,
    .open			= open,
    .release        = close,
    .write			= write,
    .read			= read,
};

struct miscdevice device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "fortytwo",
    .fops = &fops,
};

static int __init misc_init(void)
{
    int error;

    error = misc_register(&device);
    if (error) {
        pr_err("Unable to register Example Driver\n");
        return error;
    }

    pr_info("Registered  Driver fortytwo\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&device);
    pr_info("Deregister Driver fortytwo\n");
}

module_init(misc_init)
module_exit(misc_exit)
