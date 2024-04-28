#include <linux/module.h>
#include <linux/platform_device.h>
#include "gpio_platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "[%s]: " fmt, __func__

void pcdev_release(struct device *dev) {
    pr_info("Device released\n");
}

struct pcdev_platform_data pcdev_pdata[] = {
    [0] = {.size = 512, .permission = RDWR, .serial_number = "LED_1"},
    [1] = {.size = 512, .permission = RDWR, .serial_number = "LED_2"},
    [2] = {.size = 1024, .permission = RDONLY, .serial_number = "LED_3"},
    [3] = {.size = 2048, .permission = WRONLY, .serial_number = "LED_4"}
};

struct platform_device platform_pcdev_1 = {
    .name = "LED-Ax",
    .id = 0,
    .dev = {
        .platform_data = &pcdev_pdata[0],
        .release = pcdev_release
    }
};

struct platform_device platform_pcdev_2 = {
    .name = "LED-Bx",
    .id = 1,
    .dev = {
        .platform_data = &pcdev_pdata[1],
        .release = pcdev_release
    }
};

struct platform_device platform_pcdev_3 = {
    .name = "LED-Cx",
    .id = 2,
    .dev = {
        .platform_data = &pcdev_pdata[2],
        .release = pcdev_release
    }
};

struct platform_device platform_pcdev_4 = {
    .name = "LED-Dx",
    .id = 3,
    .dev = {
        .platform_data = &pcdev_pdata[3],
        .release = pcdev_release
    }
};

struct platform_device *platform_pcdevs[] = {
    &platform_pcdev_1,
    &platform_pcdev_2,
    &platform_pcdev_3,
    &platform_pcdev_4
};

static int __init pcdev_platform_init(void) {
    platform_add_devices(platform_pcdevs, ARRAY_SIZE(platform_pcdevs));

    pr_info("Platform device setup module loaded\n");

    return 0;
}

static void __exit pcdev_platform_exit(void) {
    platform_device_unregister(&platform_pcdev_1);
    platform_device_unregister(&platform_pcdev_2);
    platform_device_unregister(&platform_pcdev_3);
    platform_device_unregister(&platform_pcdev_4);

    pr_info("Platform device setup module unloaded\n");
}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nahass");
MODULE_DESCRIPTION("Register GPIO devices with the GPIO driver");