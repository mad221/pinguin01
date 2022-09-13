#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>


#define BUF "mpouzol"
#define BUF_LEN 8
#define MAX_SIZE 4096

MODULE_DESCRIPTION("Misc Driver");
MODULE_AUTHOR("mpouzol");
MODULE_LICENSE("GPL");

struct dentry * dent = NULL;
static struct mutex foo_mutex;

char str[MAX_SIZE];

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
	char mybuf[BUF_LEN];

	simple_write_to_buffer(mybuf, BUF_LEN, offset, buf, len);
    
	if ( strncmp(BUF, mybuf, BUF_LEN) == 0)
    {
		return BUF_LEN;
    }

    return -EINVAL;
}

static ssize_t read(struct file *f, char *buffer, size_t length, loff_t *offset)
{
	int res;
	char *read_from = BUF + *offset;
	size_t read_num = length < (BUF_LEN - *offset) ? length : (BUF_LEN - *offset);

	if (read_num == 0) {
		res = 0;
		goto end;
	}

	res = copy_to_user(buffer, read_from, read_num);
	if (res == read_num) {
		res = -EIO;
	} else {
		*offset = BUF_LEN - res;
		res = read_num - res;
	}
end:
	return (res);
}

static ssize_t jiffies_read(struct file *f, char *buffer, size_t length, loff_t *offset)
{
	unsigned long time_stamp = jiffies;
    char number[11];
    ssize_t ret = 0;

	memset(number, *offset, 11);
    
    snprintf(number, 11,"%ld", time_stamp);
   	ret = copy_to_user(buffer, number, 11);

    return simple_read_from_buffer(buffer, length, offset, number, 11);
}

static ssize_t foo_write(struct file *file, const char __user *buf,
			 size_t len, loff_t *offset)
{
	ssize_t res = 0;
	mutex_lock(&foo_mutex);
	memset(str, 0, strlen(str));
	res = simple_write_to_buffer(str, 4096, offset, buf, len);
	mutex_unlock(&foo_mutex);
	return res;
}

static ssize_t foo_read(struct file *filep, char *buf, size_t len,
			loff_t *offset)
{
	ssize_t ret = 0;
	mutex_lock(&foo_mutex);
	ret = simple_read_from_buffer(buf, len, offset, str, MAX_SIZE);
	mutex_unlock(&foo_mutex);
	return ret;
}


static const struct file_operations fops = {
    .owner			= THIS_MODULE,
    .open			= open,
    .release        = close,
    .write			= write,
    .read			= read,
};

static const struct file_operations jiffies_fops = {
    .owner			= THIS_MODULE,
    .open			= open,
    .release        = close,
	.read     =  jiffies_read,
};



static const struct file_operations foo_fops = {
    .owner			= THIS_MODULE,
    .open			= open,
    .release        = close,
    .write			= foo_write,
    .read			= foo_read,
};


struct miscdevice device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "fortytwo",
    .fops = &fops,
};

static int __init misc_init(void)
{
    dent = debugfs_create_dir("fortytwo", NULL);

    if (!dent)
        pr_info("Dir is not create\n");
    if(!debugfs_create_file ("id", 0666, dent, NULL, &fops))
        pr_info("file id  is not create\n");
    if(!debugfs_create_file ("jiffies", 0444, dent, NULL, &jiffies_fops))
        pr_info("file id  is not create\n");
    if(!debugfs_create_file ("foo", 0644, dent, NULL, &foo_fops))
        pr_info("file id  is not create\n");

    return 0;
}

static void __exit misc_exit(void)
{
    debugfs_remove_recursive(dent);
    pr_info("files are deleted\n");
}

module_init(misc_init)
module_exit(misc_exit)
