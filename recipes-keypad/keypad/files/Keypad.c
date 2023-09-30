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
#include "Keypad_PRIVATE.h"
#include "Keypad_CONFIG.h"
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
MODULE_DESCRIPTION("Keypad driver.");

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
short int ROW_Pins [KEYPAD_ROWS];
short int COL_Pins [KEYPAD_COLS];

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
module_param_named (ROW_0, ROW_Pins[0], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (ROW_1, ROW_Pins[1], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (ROW_2, ROW_Pins[2], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (ROW_3, ROW_Pins[3], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);

module_param_named (COL_0, COL_Pins[0], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (COL_1, COL_Pins[1], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (COL_2, COL_Pins[2], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
module_param_named (COL_3, COL_Pins[3], short , S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);

MODULE_PARM_DESC(ROW_0, "ROW number 0 Pin number.");
MODULE_PARM_DESC(ROW_1, "ROW number 1 Pin number.");
MODULE_PARM_DESC(ROW_2, "ROW number 2 Pin number.");
MODULE_PARM_DESC(ROW_3, "ROW number 3 Pin number.");

MODULE_PARM_DESC(COL_0, "COL number 0 Pin number.");
MODULE_PARM_DESC(COL_1, "COL number 1 Pin number.");
MODULE_PARM_DESC(COL_2, "COL number 2 Pin number.");
MODULE_PARM_DESC(COL_3, "COL number 3 Pin number.");

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
static int __init KeypadInit(void);
static void __exit KeypadExit(void);
ssize_t KeypadDriver_read (struct file *file, char __user *user_buffer, size_t count, loff_t * off);
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

module_init(KeypadInit);
module_exit(KeypadExit);

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
    .read = KeypadDriver_read,
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

static int __init KeypadInit(void)
{
    int returnValue;
    short int i = 0;
    /*1- Allocate device number*/
    /*Reserve major number.*/
    returnValue = alloc_chrdev_region(&device_number, 0, TOTAL_NUMBER_OF_DEVICES, "KaypadDriverNum");
    if (0 != returnValue){
        /*Failed to reserve major number*/
        return -1;
    }
    /*
    *2- Define whether device is character or block or network.
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
    if (NULL == (myClass=class_create(THIS_MODULE, "KeypadClass"))){
        /*Failed to create Device driver class.*/
        goto ERROR_CLASS;
    }

    /* 4- Second Parameter is the parent file*/
    myDevice = device_create(myClass, NULL, device_number, NULL, "Keypad") ;
    if (NULL == myDevice){
        goto ERROR_DEVICE;
    }
    
    for (i=0; i < KEYPAD_ROWS; i++){
        if (gpio_request(ROW_Pins[i], "")){
            goto ERROR_GPIO_REQUEST;
        }
    }

    for (i = 0; i < KEYPAD_COLS; i++){
        if (gpio_request(COL_Pins[i], "")){
            goto ERROR_GPIO_REQUEST;
        }
    }


    
    for (i = 0; i < KEYPAD_ROWS; i++){
        if (gpio_direction_output(ROW_Pins[i],   0)){
            /*Can't allocate GPIO*/
            goto ERROR_GPIO_DIR;
        }
    }


    for (i = 0; i < KEYPAD_COLS; i++){
        if (gpio_direction_input(COL_Pins[i])){
            /*Can't allocate GPIO*/
            goto ERROR_GPIO_DIR;
        }
    }
    


    return 0;

    ERROR_GPIO_DIR:

    for (i = 0; i < KEYPAD_COLS; i++){
        gpio_free(COL_Pins[i]);
    }


    for (i = 0; i < KEYPAD_ROWS; i++){
        gpio_free(ROW_Pins[i]);
    }
        
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

static void __exit KeypadExit(void)
{
    short int i;
    for (i = 0; i < KEYPAD_ROWS; i++){
        gpio_set_value(ROW_Pins[i], 0);
    }

    for (i = 0; i < KEYPAD_ROWS; i++){
        gpio_direction_input(ROW_Pins[i]);
    }
    for (i = 0; i < KEYPAD_COLS; i++){
        gpio_direction_input(COL_Pins[i]);
    }

  
    for (i = 0; i < KEYPAD_COLS; i++){
        gpio_free(COL_Pins[i]);
    }


    for (i = 0; i < KEYPAD_ROWS; i++){
        gpio_free(ROW_Pins[i]);
    }

    cdev_del(&CharacterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    /*Second argument is the number of devices*/
    unregister_chrdev_region(device_number, 1);

}


ssize_t KeypadDriver_read (struct file *file, char __user *user_buffer, size_t count, loff_t * off){
    /*Exaplained at the end of function*/
    int not_copied;
    #define READ_SIZE (KEYPAD_COLS * KEYPAD_ROWS + 1)
    char temp[READ_SIZE]; /* "1 \n" */
    temp[READ_SIZE-1] = '\n';

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
    if ((count + *off) > READ_SIZE){
        count = READ_SIZE - (*off);
    }

    short int pin_val_r , pin_val_c;
    
    for ( pin_val_r = 0; pin_val_r < KEYPAD_ROWS; pin_val_r++){
        gpio_set_value( ROW_Pins[pin_val_r],1);
        for (pin_val_c = 0; pin_val_c < KEYPAD_COLS; pin_val_c++){
            temp[(pin_val_r * KEYPAD_ROWS) + pin_val_c] = (char)gpio_get_value(COL_Pins[pin_val_c]) + '0';
        }
        gpio_set_value( ROW_Pins[pin_val_r],0);
    }
    
    not_copied = copy_to_user(user_buffer, &temp[*off], count);
    if (not_copied){
        /*There is an error !.*/
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