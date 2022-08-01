#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#define MYMAJOR	200
#define MYNAME	"chardev_test_byHc"

//DEVICE ID
dev_t devid;
struct cdev cdev;

static int chardev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "chardev open\n");
	return 0;
}

static ssize_t chardev_read(struct file *file, char __user *buf,
	size_t size, loff_t *ppos)
{
	return 0;
}

static const struct file_operations chardev_fops = {
	.open = chardev_open,
	.read = chardev_read,
};

static int __init mymodule_init(void)
{
	int ret = 0;
	printk(KERN_INFO "chrdev_init helloworld init\n");
	cdev_init(&cdev,&chardev_fops);

	alloc_chrdev_region(&devid,2,255,MYNAME);
	printk(KERN_INFO "MAJOR Number is %d\n",MAJOR(devid));
	printk(KERN_INFO "MINOR Number is %d\n",MINOR(devid));
	cdev_add(&cdev,devid,255);
	return 0;
}

static void __exit mymodule_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
	cdev_del(&cdev);
	unregister_chrdev_region(devid,255);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL"); // 描述模块的许可证
MODULE_AUTHOR("Hanc"); // 描述模块的作者
MODULE_DESCRIPTION("module test"); // 描述模块的介绍信息
MODULE_ALIAS("alias xxx");	// 描述模块的别名信息

