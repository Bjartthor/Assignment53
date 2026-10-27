#include <linux/module.h>
#include <linux/fs.h>

#define DEVICE_MAJOR 0 ///< Requested device node major number or 0 for dynamic allocation
#define DEVICE_NAME "mydev1" ///< Name in /proc/devices. The node in /dev is created separately with mknod

static int majorNumber; ///< Stores the device number -- determined automatically
static int numberOpens; ///< Counts the number of times the device is opened
static char *name = "world";

module_param(name, charp, 0444); ///< 0444: readable in /sys/module/mydev/parameters/name
MODULE_PARM_DESC(name, "The name to display in the kernel log");

static int mydev_open(struct inode *inodep, struct file *filep)
{
  numberOpens++;
  pr_info("mydev: Device has been opened %d time(s)\n", numberOpens);
  return 0;
}

static ssize_t mydev_write(struct file *filep, const char __user *buf,
                                              size_t count, loff_t *ppos)
{
  pr_info("mydev: Accepting %zu bytes from the user\n", count);
  return count; ///< Report all bytes as consumed
}

static int mydev_release(struct inode *inodep, struct file *filep)
{
  pr_info("mydev: Device successfully closed\n");
  return 0;
}

static const struct file_operations fops = {
  .owner = THIS_MODULE, ///< Blocks rmmod while the device is open
  .open = mydev_open,
  .write = mydev_write,
  .release = mydev_release,
};

static int __init mydev_init(void)
{
  pr_info("mydev: Hello %s from the RPi LKM!\n", name);
  majorNumber = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
  if (majorNumber < 0) {
    pr_alert("mydev: Failed to register a major number\n");
    return majorNumber;
  }
  pr_info("mydev: Device registered correctly with major number %d\n", majorNumber);
  return 0;
}

static void __exit mydev_exit(void)
{
  unregister_chrdev(majorNumber, DEVICE_NAME);
  pr_info("mydev: Goodbye %s from the RPi LKM!\n", name);
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bjartthor");
MODULE_DESCRIPTION("A simple Linux LKM that accepts characters (bytes) from the user.");
MODULE_VERSION("0.1");