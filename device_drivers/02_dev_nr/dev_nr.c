#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Registers a device nr and implements some call back functions");


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
	.release = driver_close
};

#define MYMAJOR 90

static int __init my_init(void) {
	int retval ;
	printk("Hello, Kernel!\n");
	retval = register_chrdev(MYMAJOR , "my_dev_nr" , &fops);

	if (retval ==0){
		printk("dev_nr - regi %d" , MYMAJOR );
	}
	else{
		printk("does't registered");
	}

	return 0;
}


static void __exit my_exit(void) {
	printk("Goodbye, Kernel\n");
	unregister_chrdev(MYMAJOR , "my_dev_nr");
}

module_init(my_init);
module_exit(my_exit);


