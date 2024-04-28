#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("GPIO driver for led");

static char buffer[255];
static int buffer_pointer;

static dev_t my_device_nr ;
static struct class *my_class ;
static struct cdev my_device ;

static ssize_t driver_read(struct file *File , char *user_buffer ,size_t , loff_t *off){

	int to_copy , not copied ,delta ;
	char tmp[3] = " \n";
	to_copy = min(count , sizeof(tmp));
	printk("value of the button: %d" ,gpio_get_value(17) );
	tmp[0] = gpio_get_value(17) + '0';

	not_copied = copy_to_user(user_buffer , tmp , to_copy);
	delta = to_copy - not_copied ;
	return delta ;
}


static ssize_t driver_write(struct file *File , const char *user_buffer ,size_t , loff_t *off){
	int to_copy , not copied ,delta ;
	char value ;
	to_copy = min(count , sizeof(value));
	not_copied = copy_from_user(value , user_buffer , to_copy);
	switch(value) {
		case '0':
			gpio_set_value(4, 0);
			break;
		case '1':
			gpio_set_value(4, 1);
			break;
		default:
			printk("Invalid Input!\n");
			break;
	}
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
#define DRIVER_NAME "my_gpio_driver"
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

	gpio_request(4,"rpi-gpio-4");
	gpio_direction_output(4,0);

	gpio_request(17,"rpi-gpio-4");
	gpio_direction_output(17,0);

	return 0;
}


static void __exit my_exit(void) {
	gpio_set_value(4,0);
	gpio_free(17);
	gpio_free(4);
	printk("Goodbye, Kernel\n");
	cdev_del(&my_device);
	device_destroy(my_class , my_device_nr);
	class_destroy(myclass);
	unregister_chrdev(MYMAJOR , "my_dev_nr");

	

}

module_init(my_init);
module_exit(my_exit);


