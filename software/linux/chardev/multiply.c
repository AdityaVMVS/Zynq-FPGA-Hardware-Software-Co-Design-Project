#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "multiplier"

#define MULTIPLIER_BASEADDR  0x43C00000
#define MULTIPLIER_SPAN      0x10000

static int Major;
static void __iomem *multiplier_ptr;

/* Function prototypes */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

/* File operations */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* INIT */
static int __init multiplier_init(void)
{
    multiplier_ptr = ioremap(MULTIPLIER_BASEADDR, MULTIPLIER_SPAN);
    if (!multiplier_ptr) {
        printk(KERN_ALERT "Failed to map multiplier peripheral\n");
        return -ENOMEM;
    }

    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering device failed: %d\n", Major);
        iounmap(multiplier_ptr);
        return Major;
    }

    printk(KERN_INFO "Registered device with major number %d\n", Major);
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);

    return 0;
}

/* EXIT */
static void __exit multiplier_exit(void)
{
    unregister_chrdev(Major, DEVICE_NAME);
    iounmap(multiplier_ptr);
    printk(KERN_INFO "Multiplier driver removed\n");
}

/* OPEN */
static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Multiplier device opened\n");
    return 0;
}

/* CLOSE */
static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Multiplier device closed\n");
    return 0;
}

/* READ: bytes 0–11 */
static ssize_t device_read(struct file *filp, char __user *buffer,
                           size_t length, loff_t *offset)
{
    int i;
    unsigned char value;

    if (length > 12)
        length = 12;

    for (i = 0; i < length; i++) {
        value = ioread8(multiplier_ptr + i);
        if (put_user(value, buffer + i))
            return -EFAULT;
    }

    return i;
}

/* WRITE: bytes 0–7 */
static ssize_t device_write(struct file *file, const char __user *buffer,
                            size_t length, loff_t *offset)
{
    int i;
    unsigned char value;

    if (length > 8)
        length = 8;

    for (i = 0; i < length; i++) {
        if (get_user(value, buffer + i))
            return -EFAULT;

        iowrite8(value, multiplier_ptr + i);
    }

    return i;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya");
MODULE_DESCRIPTION("Multiplier Character Device Driver");

module_init(multiplier_init);
module_exit(multiplier_exit);
