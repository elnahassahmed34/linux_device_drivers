

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include "gpio_platform.h"
#include <linux/uaccess.h>
#include <linux/gpio.h>

#undef pr_fmt
#define pr_fmt(fmt) "[%s]: " fmt, __func__

#define CLASS_NAME      "gpio_class"
#define DEV_NAME        "led"
#define NO_OF_DEVICES   4

enum pcdev_name {
    LED_A_CONF,
    LED_B_CONF,
    LED_C_CONF,
    LED_D_CONF,
};

struct device_configure {
    int configure_num1;
    int configure_num2;
};

struct device_configure pcdev_configure[] = {
    [LED_A_CONF] = {.configure_num1 = 40, .configure_num2 = 254 },
    [LED_B_CONF] = {.configure_num1 = 50, .configure_num2 = 253 },
    [LED_C_CONF] = {.configure_num1 = 60, .configure_num2 = 252 },
    [LED_D_CONF] = {.configure_num1 = 70, .configure_num2 = 251 },
};

struct platform_device_id pcdevs_ids[] = {
    [0] = {
        .name = "LED-Ax",
        .driver_data = LED_A_CONF
    },
    [1] = {
        .name = "LED-Bx",
        .driver_data = LED_B_CONF
    },
    [2] = {
        .name = "LED-Cx",
        .driver_data = LED_C_CONF
    },
    [3] = {
        .name = "LED-Dx",
        .driver_data = LED_D_CONF
    },
    {}
};

struct pcdev_private_data {
    struct pcdev_platform_data pdata;
    dev_t dev_num;
    char *buffer;
    struct cdev cdev;
};

struct pcdrv_private_data {
    int total_device;
    dev_t device_number_base;
    struct class *class_pcd;
    struct device *device_pcd;
};
struct pcdrv_private_data pcdrv_data;

int pcd_release(struct inode *inode, struct file *filp);
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);

int pcd_platform_driver_probe(struct platform_device *pdev);
int pcd_platform_driver_remove(struct platform_device *pdev);

struct file_operations pcd_fops = {
    .open = pcd_open,
    .write = pcd_write,
    .read = pcd_read,
    .release = pcd_release,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE
};

struct platform_driver pcd_platform_driver = {
    .probe = pcd_platform_driver_probe,
    .remove = pcd_platform_driver_remove,
    .id_table = pcdevs_ids,
    .driver = {
        .name = "pseudo-char-device"
    }
};



ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
    char tmp[3] = " \n";
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;


    
    int x = MINOR(pcdev_data->dev_num) ;
    tmp[0] = gpio_get_value(x) + '0';


    copy_to_user(buff, tmp , count);   

    return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {

    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
    
    copy_from_user(pcdev_data->buffer, buff, count);
    char x = pcdev_data->buffer[0] ;
    switch(x) {
		case '0':
			gpio_set_value(MINOR(pcdev_data->dev_num), 0);
			break;
		case '1':
			gpio_set_value(MINOR(pcdev_data->dev_num), 1);
			break;
		default:
			printk("Invalid Input!\n");
			break;
	}
    
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp) {
    pr_info("Released successful\n");
    return 0;
}

int pcd_platform_driver_probe(struct platform_device *pdev) {

    int ret;
    struct pcdev_private_data *dev_data;
    struct pcdev_platform_data *pdata;

    pr_info("Device was detected\n");

    pdata = (struct pcdev_platform_data *)dev_get_platdata(&pdev->dev);

    dev_data = devm_kzalloc(&pdev->dev, sizeof(*dev_data), GFP_KERNEL);

    dev_set_drvdata(&pdev->dev, dev_data);

    dev_data->pdata.size = pdata->size;
    dev_data->pdata.permission = pdata->permission;
    dev_data->pdata.serial_number = pdata->serial_number;

    dev_data->buffer = devm_kzalloc(&pdev->dev, dev_data->pdata.size, GFP_KERNEL);

    dev_data->dev_num = pcdrv_data.device_number_base + pdev->id;

    cdev_init(&dev_data->cdev, &pcd_fops);
    dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->cdev, dev_data->dev_num, 1);

    pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, dev_data->dev_num, NULL, "led-%d", pdev->id);

    pcdrv_data.total_device++;

    int x = pdev->id;

	gpio_request(x,"rpi-gpio-4");
	gpio_direction_output(x,0);


    pr_info("Probe was successful\n");
    return 0;
}

int pcd_platform_driver_remove(struct platform_device *pdev) {

    struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);

    device_destroy(pcdrv_data.class_pcd, dev_data->dev_num);
    cdev_del(&dev_data->cdev);
    pcdrv_data.total_device--;

    int x = pdev->id;

	gpio_free(x,"rpi-gpio-4");


    pr_info("Device was removed");
    return 0;
}


static int __init char_platform_driver_init(void) {

    int ret;

    ret = alloc_chrdev_region(&pcdrv_data.device_number_base, 0, NO_OF_DEVICES, DEV_NAME);

    pcdrv_data.class_pcd = class_create(CLASS_NAME);

    ret = platform_driver_register(&pcd_platform_driver);

    pr_info("Platform driver module loaded\n");

    return 0;

    return ret;

}

static void __exit char_platform_driver_exit(void) {
    platform_driver_unregister(&pcd_platform_driver);
    class_destroy(pcdrv_data.class_pcd);
    unregister_chrdev_region(pcdrv_data.device_number_base, NO_OF_DEVICES);

    pr_info("Platform driver module unloaded\n");
}

module_init(char_platform_driver_init);
module_exit(char_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nahass");
MODULE_DESCRIPTION("GPIO Platform driver & GPIO platform deivce");
MODULE_INFO(board,"Beaglebone Black rev.c");