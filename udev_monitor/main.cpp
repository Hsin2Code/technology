#include <cstdio>
#include <sys/select.h>
#include <iostream>
#include "libudev.h"

static void print_device(struct udev_device *dev)
{
    // 获取事件动作
    const char *action = udev_device_get_action(dev);
    if(! action) {
        action = "exists";
    }
    // 获取设备vid
    const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");
    if(! vendor)
        vendor = "0000";
    // 获取设备pid
    const char *product = udev_device_get_sysattr_value(dev, "idProduct");
    if(! product)
        product = "0000";

    const char *sysname = udev_device_get_sysname(dev); // 系统名
    const char *subsystem = udev_device_get_subsystem(dev); // 设备类型
    const char *devtype = udev_device_get_devtype(dev); // 获取设备类型
    const char *driver = udev_device_get_driver(dev);   // 驱动
    const char *syspath = udev_device_get_syspath(dev); // 系统绝对路径
    const char *devpath = udev_device_get_devpath(dev); // 设备路径
    const char *devnode = udev_device_get_devnode(dev); // 设备节点

    // 打印数据，进行分析
    printf("%s %s %s %s %s %s:%s\n %s %s\n",
           action, sysname, subsystem, devtype, driver, vendor, product,
           devnode, syspath);
}

static void enumerate_devices(struct udev *udev)
{
    // 创建一个枚举器用于扫描已连接的设备
    struct udev_enumerate *enumerate = NULL;
    enumerate = udev_enumerate_new(udev);
    // 在“block”子系统中创建设备列表
//    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);
//    udev_enumerate_scan_subsystems(enumerate);

    // 返回一个存储了设备所有属性信息的链表
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *dev_list_entry;
    // 遍历每一个枚举条目信息。
    udev_list_entry_foreach(dev_list_entry, devices) {
        // 获取名称
        const char *path  = udev_list_entry_get_name(dev_list_entry);
        // 通过设备名称创建一个udev设备的映射
        struct udev_device *dev = udev_device_new_from_syspath(udev, path);

        print_device(dev);
        // 释放设备映射
        udev_device_unref(dev);
    }
    // 释放枚举对象
    udev_enumerate_unref(enumerate);
}


static void monitor_devices(struct udev *udev)
{
    struct udev_monitor *mon = NULL;
    // 创建新的udev监视器并连接到指定的事件源（udev or kernel）
    mon = udev_monitor_new_from_netlink(udev, "udev");
    // 过滤器 如：block、hidraw
//    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
    // 启动监听
    udev_monitor_enable_receiving(mon);
    // 获取文件描述符
    int fd = udev_monitor_get_fd(mon);
    // 使用select触发事件
    // TODO: linux epoll, osx kqueue 实现更好些
    fd_set fds;
    while(1) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        int ret = select(fd + 1, &fds, NULL, NULL, NULL);
        if(ret < 0) break;
        if(FD_ISSET(fd, &fds)) {
            // 获取udev设备映射
            struct udev_device *dev = udev_monitor_receive_device(mon);

            print_device(dev);
            // 释放设备映射
            udev_device_unref(dev);
        }
    }
    udev_monitor_unref(mon);
}


int udevadm()
{
    struct udev *udev = NULL;
    // 创建udev对象
    udev = udev_new();
    if(udev == NULL) {
        std::cout << "Can't create udev" << "\n";
        return -1;
    }
    // NOTE: 先创建监控事件之后进行枚举，防止另一种情况导致的事件丢失。
    // 枚举已存在设备
    enumerate_devices(udev);
    // 监听热插拔事件
    monitor_devices(udev);
    // 销毁对象
    udev_unref(udev);
    return 0;
}

int main(int argc, char *argv[])
{
    udevadm();
    return 0;
}
