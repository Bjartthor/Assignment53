#include <linux/module.h>
#include <linux/fs.h>

#define DEVICE_MAJOR 0          // 0 = let the kernel pick a free number
#define DEVICE_NAME "mydev1"    // name shown in /proc/devices

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bjartthor");
MODULE_DESCRIPTION("A simple Linux LKM that accepts a single parameter");
MODULE_VERSION("0.1");

static char *name = "world";
static int majorNumber;

module_param(name, charp, 0444);
MODULE_PARM_DESC(name, "The name to display in the systemd journal");

static ssize_t mydev_write(struct file *filep, const char __user *buf,
                           size_t count, loff_t *ppos)
{
    pr_info("mydev: Accepting %zu bytes from the user\n", count);
    return count; // Report all bytes as consumed
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = mydev_write,
};

static int __init mydev_init(void) {
    printk(KERN_INFO "mydev: Hello %s from the RPi LKM!\n", name);
    majorNumber = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        pr_alert("mydev: Failed to register a major number\n");
        return majorNumber;
    }
    pr_info("mydev: Device registered correctly with major number %d\n", majorNumber);
    return 0;
}

static void __exit mydev_exit(void) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "mydev: Goodbye %s from the RPi LKM!\n", name);
}

module_init(mydev_init);
module_exit(mydev_exit);