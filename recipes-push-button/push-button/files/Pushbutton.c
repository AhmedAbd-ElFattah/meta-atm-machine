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
#include <linux/delay.h>
#include <linux/gpio.h>
#include "Pushbutton_CONFIG.h"
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
MODULE_DESCRIPTION("Push button driver.");

/*
==================================================
  End Section --> Module props
==================================================
*/

/*
==================================================
  Start Section --> Keypad H.W Module properties
==================================================
*/
short int PIN_Num;

/*
==================================================
  End Section --> Keypad H.W Module properties
==================================================
*/

/*
==================================================
  Start Section --> Module parameter configurations
==================================================
*/
module_param (PIN_Num, short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(PIN_Num, "Push button connected pin number.");
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
static int __init PushbuttonInit(void);
static void __exit PushbuttonExit(void);
ssize_t PushbuttonDriver_read (struct file *file, char __user *user_buffer, size_t count, loff_t * off);
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

module_init(PushbuttonInit);
module_exit(PushbuttonExit);

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
    .read = PushbuttonDriver_read,
};

/*
==================================================
  End Section --> Module global vals.
==================================================
*/

/*
==================================================
  Start Section --> Implementation
==================================================
*/

static int __init PushbuttonInit(void)
{
    int returnValue;
    /*1- Allocate device number*/
    /*Reserve major number.*/
    returnValue = alloc_chrdev_region(&device_number, 0, TOTAL_NUMBER_OF_DEVICES, "SevenSegmentDisplayDeviceDriverNum");
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
    if (NULL == (myClass=class_create(THIS_MODULE, "PushbuttonClass"))){
        /*Failed to create Device driver class.*/
        goto ERROR_CLASS;
    }

    /* 4- Second Parameter is the parent file*/
    myDevice = device_create(myClass, NULL, device_number, NULL, "Pushbutton") ;
    if (NULL == myDevice){
        goto ERROR_DEVICE;
    }
    /*Initialize pins*/
  


    if (gpio_request(PIN_Num, "")){
        goto ERROR_GPIO_REQUEST;
    }
    
    if (gpio_direction_input(PIN_Num)){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_DIR;
    }
    

    return 0;

    ERROR_GPIO_DIR:
    gpio_free(PIN_Num);

        
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

static void __exit PushbuttonExit(void)
{

    gpio_direction_input(PIN_Num);
    gpio_free(PIN_Num);

    cdev_del(&CharacterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    /*Second argument is the number of devices*/
    unregister_chrdev_region(device_number, 1);
}


ssize_t PushbuttonDriver_read (struct file *file, char __user *user_buffer, size_t count, loff_t * off){
  
    /*Exaplained at the end of function*/
    int not_copied;
    char temp[READ_BUFFER_SIZE];
    temp[0] = (char)gpio_get_value(PIN_Num) + '0'; /* "1 \n" */
    temp[1] = '\n';
    temp[2] = 0;
    /*
    Copy To use is a function like strcopy in functionality 
    but We can't use this function in kernel space as there are difference between 
    user space addresses and kernel space addresses. 
    */
   
   /*
   When cat (linux command) is called it send a count the indicates
   The total number of bytes that it (cat command) want o read.
   Normally this number if large so we do the following.
   */
    
    /*This part is called adusting count*/
    if ((count + *off) > READ_BUFFER_SIZE){
        count = READ_BUFFER_SIZE - (*off);
    }  


    not_copied = copy_to_user(user_buffer, &temp[*off], count);
    if (not_copied){
        return -1;
    }
    /*When it calls again the function start from the index when it has endded.*/
    *off = count;
    /*Number of bytes that successfully readed. -_-*/
    return count;
}

/*
==================================================
  End Section --> Implementation
==================================================
*/