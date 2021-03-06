#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>


#define BUF "mpouzol"
#define BUF_LEN 8
#define MAX_SIZE 8096

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
	char buffer[BUF_LEN];
	

	if (len != BUF_LEN || copy_from_user(buffer, buf, BUF_LEN)  != BUF_LEN)
	 	return (-1);
	
	if (strncmp(buffer, BUF, BUF_LEN) == 0)
		return (BUF_LEN);
	else
		return (-1);
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

static ssize_t jiffies_read(struct file *f, char *buffer, size_t length, loff_t *offset)
{
	unsigned long time_stamp = jiffies;
    char number[10];
    ssize_t ret = 0;

    snprintf(number, 10,"%ld", time_stamp);
   	ret = simple_read_from_buffer(buffer, length, offset, number, 10);
    return (10);
}

static ssize_t foo_write(struct file *file, const char  *buffer, size_t len, loff_t *offset)
{
	ssize_t res = 0;

	mutex_lock(&foo_mutex);
	memset(str, 0, MAX_SIZE);
    res = simple_write_to_buffer(str, MAX_SIZE, offset, buffer, len);
    mutex_unlock(&foo_mutex);
	return (res);
}


static ssize_t foo_read(struct file *file, char  *buffer, size_t len, loff_t *offset)
{
	ssize_t ret = 0;
	mutex_lock(&foo_mutex);
	ret = simple_read_from_buffer(buffer, len, offset, str, MAX_SIZE);
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
    debugfs_remove(dent);
    pr_info("dire is deleted\n");
}

module_init(misc_init)
module_exit(misc_exit)
