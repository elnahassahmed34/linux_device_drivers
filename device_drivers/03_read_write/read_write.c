#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Registers a device nr and implements some call back functions");

static char buffer[255]
static int buffer_pointer;

static dev_t my_device_nr ;
static struct class *my_class ;
static struct cdev my_device ;

static ssize_t driver_read(struct file *File , char *user_buffer ,size_t , loff_t *off){
	int to_copy , not copied ,delta ;
	to_copy = min(count , buffer_pointer);
	not_copied = copy_to_user(user_buffer , buffer , to_copy);
	delta = to_copy - not_copied ;
	return delta ;
}


static ssize_t driver_write(struct file *File , const char *user_buffer ,size_t , loff_t *off){
	int to_copy , not copied ,delta ;
	to_copy = min(count , sizeof(buffer));
	not_copied = copy_from_user(buffer , user_buffer , to_copy);
	buffer_pointer = to_copy ;
	delta = to_copy - not_copied ;
	return delta ;
}


static int driver_open(struct inode * device_file , struct file * instance){
	printk("dev_nr - open was called! \n");
	return 0 ;
}

static int driver_close(struct inode * device_file , struct file * instance){
	printk("dev_nr - close was called! \n");
	return 0 ;
}

static struct file_operations fops = {
	.owner = THIS_MODULE , 
	.open = driver_open ,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

#define MYMAJOR 90
#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MYModuleClass"

static int __init my_init(void) {
	int retval ;
	printk("Hello, Kernel!\n");

	if(alloc_chrdev_region(&my_device_nr , 0 , 1 , DRIVER_NAME) < 0){
		printk("device not allocated\n");
		return -1
	}
	printk("right allocation");

	if(my_class = class_create(THIS_MODULE , DRIVER_CLASS)==NULL)  {
		goto ClassError ;
	}

	device_create(&my_class , my_device_nr , 1);

	cdev_init(&my_device , &fops);
	cdev_add(&my_device , my_device_nr , 1);

ClassError:
	unregister_chrdev(my_device_nr , DRIVER_NAME);
	return 0;
}


static void __exit my_exit(void) {
	printk("Goodbye, Kernel\n");
	cdev_del(&my_device);
	device_destroy(my_class , my_device_nr);
	class_destroy(myclass);
	unregister_chrdev(MYMAJOR , "my_dev_nr");
}

module_init(my_init);
module_exit(my_exit);


