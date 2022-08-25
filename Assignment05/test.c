#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define BUF "mpouzol"
#define BUF_LEN 7


MODULE_DESCRIPTION("Misc Driver");
MODULE_AUTHOR("mpouzol");
MODULE_LICENSE("GPL");

static struct miscdevice device;

static ssize_t module_read(struct file *f, char *buffer, size_t length, loff_t *offset)
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

static ssize_t module_write(struct file *f, const char *buf, size_t len, loff_t *offset)
{
	char mybuf[BUF_LEN];

	simple_write_to_buffer(mybuf, BUF_LEN, offset, buf, len);
    
	if ( strncmp(BUF, mybuf, BUF_LEN) == 0)
    {
		return BUF_LEN;
    }

    return -EINVAL;
}

static struct file_operations fops = {
  .read = module_read,
  .write = module_write,
};

int init_module(void)
{
	printk(KERN_INFO "Hello world!\n");
	device.minor = MISC_DYNAMIC_MINOR;
	device.name = "fortytwo";
	device.fops = &fops;
	return misc_register(&device);
}

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	misc_deregister(&device);
}

