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

#include "PulseBuzzer_CONFIG.h"
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
MODULE_DESCRIPTION("Pulse buzzer driver");

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

static short int PulseBuzzer_Pin;

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
module_param (PulseBuzzer_Pin, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(PulseBuzzer_Pin, "Pulse Buzzer pin.");
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

static int  __init PulseBuzzerModuleInit(void);
static void __exit PulseBuzzerModuleExit(void);

ssize_t    PulseBuzzer_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off);
static int PulseBuzzer_open(struct inode *devicefile, struct file *instance);
static int PulseBuzzer_close(struct inode *devicefile, struct file *instance);

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

module_init(PulseBuzzerModuleInit);
module_exit(PulseBuzzerModuleExit);

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
    .write = PulseBuzzer_write,
    .open = PulseBuzzer_open,
    .release = PulseBuzzer_close,
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
static int __init PulseBuzzerModuleInit(void)
{   
    int returnValue;
    /*1- Allocate device number*/
    /*Reserve major number.*/
    returnValue = alloc_chrdev_region(&device_number, 0, TOTAL_NUMBER_OF_DEVICES, "PulseBuzzerDeviceDriverNum");
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
    if (NULL == (myClass=class_create(THIS_MODULE, "PulseBuzzer"))){
        /*Failed to create Device driver class.*/
        goto ERROR_CLASS;
    }

    /* 4- Second Parameter is the parent file*/
    myDevice = device_create(myClass, NULL, device_number, NULL, "PulseBuzzer") ;
    if (NULL == myDevice){
        goto ERROR_DEVICE;
    }
    /*Initialize pins*/

    if (gpio_request(PulseBuzzer_Pin, "")){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_REQUEST;
    }

    if (
        gpio_direction_output(PulseBuzzer_Pin,0)
    ){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_DIR;
    }

    return 0;

    ERROR_GPIO_DIR:
        gpio_free(PulseBuzzer_Pin);
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

static void __exit PulseBuzzerModuleExit(void)
{

  
    gpio_set_value(PulseBuzzer_Pin, 0);
    gpio_direction_input(PulseBuzzer_Pin);
    gpio_free(PulseBuzzer_Pin);

    cdev_del(&CharacterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    /*Second argument is the number of devices*/
    unregister_chrdev_region(device_number, TOTAL_NUMBER_OF_DEVICES);
    

}


ssize_t PulseBuzzer_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off){
    int not_copied;
    short int BuzzerTurnOnTime;
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
    
    sscanf(buffer, "%hd", &BuzzerTurnOnTime);

    gpio_set_value(PulseBuzzer_Pin, 1);

    msleep(BuzzerTurnOnTime);
    gpio_set_value(PulseBuzzer_Pin, 0);
    /*Return succeessfully count.*/
    return count;

}

static int PulseBuzzer_open(struct inode *devicefile, struct file *instance){
    
    return 0;
}

static int PulseBuzzer_close(struct inode *devicefile, struct file *instance){
  
    return 0;
}

/*
==================================================
  End Section --> Implementation
==================================================
*/