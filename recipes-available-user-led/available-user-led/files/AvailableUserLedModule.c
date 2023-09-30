/*
==================================================
  Start Section --> File Includes
==================================================
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
/*To register a file.*/
#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/gpio.h>

#include "AvailableUserLed_CONFIG.h"
#include <linux/delay.h>
#include <linux/uaccess.h>

/*
==================================================
  End Section --> File Includes
==================================================
*/

/*
==================================================
  Start Section --> Module props
==================================================
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ahmed Abd-Elfattah");
MODULE_DESCRIPTION("Available User Led module driver.");

/*
==================================================
  End Section --> Module props
==================================================
*/

/*
==================================================
  Start Section --> Seven Segment H.W Module properties
==================================================
*/

static short int Led_Pin;

/*
==================================================
  End Section --> Seven Segment H.W Module properties
==================================================
*/

/*
==================================================
  Start Section --> Module parameter configurations
==================================================
*/
module_param (Led_Pin, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(Led_Pin, "led Pin Number.");
/*
==================================================
  End Section --> Module parameter configurations
==================================================
*/

/*
==================================================
  Start Section --> Init and Exit module function prototypes
==================================================
*/

static int  __init AvailableUserLedModuleInit(void);
static void __exit AvailableUserLedModuleExit(void);

ssize_t    AvailableUserLedModuleDriver_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off);
static int AvailableUserLedModuleDriver_open(struct inode *devicefile, struct file *instance);
static int AvailableUserLedModuleDriver_close(struct inode *devicefile, struct file *instance);

/*
==================================================
  End Section --> Init and Exit module function prototypes
==================================================
*/


/*
==================================================
  Start Section --> Assign init & exit function to this kernel-module 
==================================================
*/

module_init(AvailableUserLedModuleInit);
module_exit(AvailableUserLedModuleExit);

/*
==================================================
  End Section --> Assign init & exit function to this kernel-module
==================================================
*/


/*
==================================================
  Start Section --> Module global vals.
==================================================
*/

dev_t device_number;
struct cdev CharacterDevice;
struct class *myClass;
struct device *myDevice;

struct file_operations FileOperations={
    .owner = THIS_MODULE,
    .write = AvailableUserLedModuleDriver_write,
    .open = AvailableUserLedModuleDriver_open,
    .release = AvailableUserLedModuleDriver_close,
};


/*
==================================================
  End Section --> Module global vals.
==================================================
*/

/*
==================================================
  Start Section --> 
==================================================
*/
static int __init AvailableUserLedModuleInit(void)
{   
    int returnValue;
    /*1- Allocate device number*/
    /*Reserve major number.*/
    returnValue = alloc_chrdev_region(&device_number, 0, TOTAL_NUMBER_OF_DEVICES, "AvailableUserLedDeviceDriverNum");
    if (0 != returnValue){
        /*Failed to reserve major number*/
        return -1;
    }
    /*
    *2- Define wheter device is character or block or network.
    * by init character device
    */
    cdev_init(&CharacterDevice, &FileOperations);
    /*Third argument represents number of devices (counter of devices)*/
    returnValue = cdev_add(&CharacterDevice, device_number, TOTAL_NUMBER_OF_DEVICES);
    if (0 != returnValue){
        /*In case of failure delete the reserved number*/
        goto ERROR_CHARACTER_DEVICE;
    }
    
    /*
    3- Generate File ( class"RPM, Direction, ..." - device)
    Classes are located in /sys/test_class
    You can add attributes and change them at run-time.
    */
    if (NULL == (myClass=class_create(THIS_MODULE, "AvailableUserLed"))){
        /*Failed to create Device driver class.*/
        goto ERROR_CLASS;
    }

    /* 4- Second Parameter is the parent file*/
    myDevice = device_create(myClass, NULL, device_number, NULL, "AvailableUserLed") ;
    if (NULL == myDevice){
        goto ERROR_DEVICE;
    }
    /*Initialize pins*/

    if (gpio_request(Led_Pin, "")){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_REQUEST;
    }

    if (
        gpio_direction_output(Led_Pin,0)
    ){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_DIR;
    }
    
    return 0;

    ERROR_GPIO_DIR:
        gpio_free(Led_Pin);
    ERROR_GPIO_REQUEST:
        device_destroy(myClass, device_number);
    ERROR_DEVICE:
        class_destroy(myClass);

    ERROR_CLASS:
        cdev_del(&CharacterDevice);

    ERROR_CHARACTER_DEVICE:
        unregister_chrdev_region(device_number, 1);
        
        return -1;
}

static void __exit AvailableUserLedModuleExit(void)
{

  
    gpio_set_value(Led_Pin, 0);
    gpio_direction_input(Led_Pin);
    gpio_free(Led_Pin);

    cdev_del(&CharacterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    /*Second argument is the number of devices*/
    unregister_chrdev_region(device_number, 1);
    

}


ssize_t AvailableUserLedModuleDriver_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off){
    int not_copied;
    short int LedTurnOnTime;
    static unsigned char buffer[WRITE_BUFFER_SIZE] = "";

    /*Adjust count.*/ /*Adjust offset.*/
    if ((count + *off) > WRITE_BUFFER_SIZE){
        count = WRITE_BUFFER_SIZE - (*off);
    }
    
    if (!count){

        return -1;
    }
    /*Copy from user to kernel.*/
    not_copied = copy_from_user(&buffer[*off], user_buffer, count);
    if (not_copied){
        return -1;
    }
    *off = count;
    
    sscanf(buffer, "%hd", &LedTurnOnTime);

    gpio_set_value(Led_Pin, 1);

    msleep(LedTurnOnTime);
    gpio_set_value(Led_Pin, 0);
    /*Return succeessfully count.*/
    return count;

}

static int AvailableUserLedModuleDriver_open(struct inode *devicefile, struct file *instance){
    
    return 0;
}

static int AvailableUserLedModuleDriver_close(struct inode *devicefile, struct file *instance){
  
    return 0;
}

/*
==================================================
  End Section --> 
==================================================
*/