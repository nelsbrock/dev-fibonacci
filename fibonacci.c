#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/printk.h>
#include <linux/minmax.h>
#include <linux/sprintf.h>

#define FIBONACCI_MODULE_NAME "fibonacci"
#define FIBONACCI_MODULE_DESC "Fibonacci series device driver"
#define FIBONACCI_DEVICE_NAME "fibonacci"
#define FIBONACCI_BUF_LEN 64

MODULE_AUTHOR("Niklas Elsbrock");
MODULE_DESCRIPTION(FIBONACCI_MODULE_DESC);
MODULE_LICENSE("GPL");

typedef unsigned long long fib_t;

static void fibonacci_advance(fib_t* fib_current, fib_t* fib_next) {
	fib_t fib_new;

	fib_new = *fib_next;
	*fib_next += *fib_current;
	*fib_current = fib_new;
}

static ssize_t fibonacci_read(
		struct file* file,
		char __user* buf,
		size_t count,
		loff_t* ppos)
{
	char fib_buf[FIBONACCI_BUF_LEN];
	fib_t fib_current = 0;
	fib_t fib_next = 1;

	if (*ppos != 0) {
		pr_warn(FIBONACCI_MODULE_NAME
				": User tried to read from offset %lld, "
				"but reading is only supported from offset 0\n",
				*ppos);
		return -EINVAL;
	}

	while (*ppos < count) {
		int ret = snprintf(fib_buf, sizeof(fib_buf), "%llu\n", fib_current);
		if (ret < 0) {
			pr_err(FIBONACCI_MODULE_NAME ": snprintf failed\n");
			return -EIO;
		}

		size_t len = min(
				(unsigned)ret,
				min(count - *ppos, FIBONACCI_BUF_LEN - 1));
		if (copy_to_user(buf + *ppos, fib_buf, len)) {
			pr_err(FIBONACCI_MODULE_NAME ": copy_to_user failed\n");
			return -EFAULT;
		}

		*ppos += len;

#ifdef CONFIG_FIBONACCI_CHECK_OVERFLOW
		if (fib_current > fib_next)
			break;
#endif

		fibonacci_advance(&fib_current, &fib_next);
	}

	return *ppos;
}

static const struct file_operations fibonacci_fops = {
	.owner = THIS_MODULE,
	.read = fibonacci_read,
};

static struct miscdevice fibonacci_dev = {
	MISC_DYNAMIC_MINOR,
	FIBONACCI_DEVICE_NAME,
	&fibonacci_fops
};

static int __init fibonacci_init(void) {
	int ret;
	ret = misc_register(&fibonacci_dev);
	if (ret)
		pr_err(FIBONACCI_MODULE_NAME ": Unable to register "
				FIBONACCI_DEVICE_NAME " device\n");
	else
		pr_info(FIBONACCI_MODULE_NAME ": " FIBONACCI_MODULE_DESC "\n");
	return ret;
}

static void __exit fibonacci_exit(void) {
	misc_deregister(&fibonacci_dev);
}

module_init(fibonacci_init);
module_exit(fibonacci_exit);
