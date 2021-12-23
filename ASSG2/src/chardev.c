/*
 *  chardev.c: Creates a simple char device with read/write capabilities
 */

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> /* for put_user */

MODULE_LICENSE("GPL");

/*
 *  PrototypesS
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80			  /* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */

static int Major;			/* Major number assigned to our device driver */
static int Device_Open = 0; /* Is device open?
				 * Used to prevent multiple access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;
dev_t devNo;		  // Major and Minor device numbers combined into 32 bits
struct class *pClass; // class_create will set this

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
	struct device *pDev;
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0)
	{
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}

	devNo = MKDEV(Major, 0); // Create a dev_t, 32 bit version of numbers

	// Create /sys/class/chardev in preparation of creating /dev/chardev
	pClass = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(pClass))
	{
		printk(KERN_WARNING "\ncan't create class");
		unregister_chrdev_region(devNo, 1);
		return -1;
	}

	// Create /dev/chardev for this char dev
	if (IS_ERR(pDev = device_create(pClass, NULL, devNo, NULL, DEVICE_NAME)))
	{
		printk(KERN_WARNING "can't create device /dev/chardev\n");
		class_destroy(pClass);
		unregister_chrdev_region(devNo, 1);
		return -1;
	}

	printk(KERN_INFO "Registered char device %s with major %d.\n", DEVICE_NAME, Major);

	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	/*
	 * Unregister the device
	 */
	device_destroy(pClass, devNo); // Remove the /dev/chardev file
	class_destroy(pClass);		   // Remove class /sys/class/chardev
	unregister_chrdev(Major, DEVICE_NAME);
	printk(KERN_INFO "Unregistered char device %s with major %d.\n", DEVICE_NAME, Major);
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	printk(KERN_INFO "File opened %d times\n", ++counter);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open = 0; /* We're now ready for our next caller */

	/*
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module.
	 */
	module_put(THIS_MODULE);
	printk(KERN_INFO "File closed\n");

	return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
						   char *buffer,	  /* buffer to fill with data */
						   size_t length,	  /* length of the buffer     */
						   loff_t *offset)
{
	/*
	 * Number of bytes actually written to the buffer
	 */
	int bytes_read = 0;
	if (length > BUF_LEN - 1)
		length = BUF_LEN - 1;

	/*
	 * If we're at the end of the message,
	 * return 0 signifying end of file
	 */
	if (*msg_Ptr == 0)
	{
		printk(KERN_ALERT "Nothing to read\n");
		return 0;
	}

	/*
	 * Actually put the data into the buffer
	 */
	while (length && *msg_Ptr)
	{

		/*
		 * The buffer is in the user data segment, not the kernel
		 * segment so "*" assignment won't work.  We have to use
		 * put_user which copies data from the kernel data segment to
		 * the user data segment.
		 */
		put_user(*msg_Ptr, buffer++);
		*msg_Ptr = 0;
		msg_Ptr++;
		length--;
		bytes_read++;
	}
	msg_Ptr = msg;
	printk(KERN_INFO "Read %d bytes.\n", bytes_read);
	*offset = 0;

	/*
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	int bytes, errors;
	if (len >= BUF_LEN)
	{
		printk(KERN_ALERT "Sorry, your message is too long.\n");
		return -ENOBUFS;
	}
	bytes = len < BUF_LEN - 1 ? len : BUF_LEN - 1;
	errors = copy_from_user(msg + *off, buff, *off + bytes);
	if (errors != 0)
	{
		printk(KERN_ALERT "Failed to write to device buffer\n");
		return -ENOBUFS;
	}
	*off += bytes;
	printk(KERN_INFO "Current Buffer: %s (%lld bytes)\n", msg, *off);
	return len;
}