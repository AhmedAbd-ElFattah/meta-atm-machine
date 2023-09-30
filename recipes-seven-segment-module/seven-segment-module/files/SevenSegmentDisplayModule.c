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

#include "SevenSegmentDisplay_PRIVATE.h"
#include "SevenSegmentDisplay_CONFIG.h"
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
MODULE_DESCRIPTION("Seven segment module driver based on MAX7219 IC.");

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

static short int Data_Pin;
static short int Clock_Pin;
static short int Enable_Pin;

/*
* If module has 2 displays each one shows 3 digits of number at maximum,
* The value of Displays_Count will be have the value 2 And
* The value of Digits_Count  will be have the value 3.
*/
static short int Displays_Count;
static short int Digits_Count;



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
module_param (Displays_Count, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(Displays_Count, "Count seven segment displays.");

module_param (Digits_Count, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP );
MODULE_PARM_DESC(Digits_Count, "Count of max number of digits the each unit can display.");

module_param (Data_Pin, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP );
MODULE_PARM_DESC(Data_Pin, "Data Pin Number.");

module_param (Clock_Pin, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP );
MODULE_PARM_DESC(Clock_Pin, "Clock Pin Number.");

module_param (Enable_Pin, short, S_IRUSR|S_IWUSR | S_IRGRP | S_IWGRP );
MODULE_PARM_DESC(Enable_Pin, "Enable Pin Number.");
/*
==================================================
  End Section --> Module parameter configurations
==================================================
*/


/*
==================================================
  Start Section --> Seven Segment Modules Function prototypes.
==================================================
*/

void SevenSegmentModuleConfigure(void);
void SevenSegmentModuleSendCommand(short int command, short int parameter);
void SevenSegmentDisplaySetNumber(unsigned short int DiplayUnitNumber,unsigned int Number);
void SevenSegmentModuleDeinitPeripheral(void);
/*
==================================================
  End Section --> Seven Segment Module Function prototypes.
==================================================
*/


/*
==================================================
  Start Section --> Init and Exit module function prototypes
==================================================
*/

static int  __init SevenSementSevenSementDisplayModuleInit(void);
static void __exit SevenSementSevenSementDisplayModuleExit(void);

ssize_t    SevenSegmentDisplayModuleDriver_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off);
static int SevenSegmentDisplayModuleDriver_open(struct inode *devicefile, struct file *instance);
static int SevenSegmentDisplayModuleDriver_close(struct inode *devicefile, struct file *instance);

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

module_init(SevenSementSevenSementDisplayModuleInit);
module_exit(SevenSementSevenSementDisplayModuleExit);

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
    .write = SevenSegmentDisplayModuleDriver_write,
    .open = SevenSegmentDisplayModuleDriver_open,
    .release = SevenSegmentDisplayModuleDriver_close,
};


/*
==================================================
  End Section --> Module global vals.
==================================================
*/

/*
==================================================
  Start Section --> Seven Segment Modules Function Implementation.
==================================================
*/

/*
 * This function will be called when module is inserted to
 * provide the seven segment module display it's basic configuration.
 */
void SevenSegmentModuleInitPeripheral(void){
    short int i;
    /*Power on module*/
    SevenSegmentModuleSendCommand(CMD_SHUTDOWN, 0x01);
    
    SevenSegmentModuleSendCommand(CMD_SCAN_LIMIT, 0x07);
    SevenSegmentModuleSendCommand(CMD_INTENSITY, 0x0F);
    SevenSegmentModuleSendCommand(CMD_DECODE_MODE, 0xFF);

    for (i =0; i<Displays_Count; i++){
      SevenSegmentDisplaySetNumber(i, 0);
    }
    
    
}

void SevenSegmentModuleDeinitPeripheral(void){
  short int i;
  const short int pins[] = {Data_Pin, Clock_Pin, Enable_Pin };
  /*Power off*/
    SevenSegmentModuleSendCommand(CMD_SHUTDOWN, 0x00);

    for (i =0; i<Displays_Count; i++){
      SevenSegmentDisplaySetNumber(i, 0);
    }
    SevenSegmentModuleSendCommand(CMD_SHUTDOWN, 0x00);
      
    for (i = 0; i<3; i++){
    gpio_set_value(pins[i], 0);
    gpio_direction_input(pins[i]);
    gpio_free(pins[i]);
  }
}

void SevenSegmentModuleSendCommand(short int command, short int parameter){
    const short int data [] = {command, parameter}; 

    gpio_set_value(Enable_Pin, 0);
    short int data_index;
    short int i;
    for (data_index = 0;data_index < 2; data_index++){

    for (i =0 ; i<8 ; i++){
        gpio_set_value(Data_Pin, (data[data_index]>>(7-i)) &0x01 );

        gpio_set_value(Clock_Pin, 1);
        usleep_range(100, 300);

        gpio_set_value(Clock_Pin, 0 );
        usleep_range(100, 300);

    }

    }

    gpio_set_value(Enable_Pin, 1);
}


void SevenSegmentDisplaySetNumber(unsigned short int DiplayUnitNumber,unsigned int Number){
    short int current_digit;

    int MaxNumber=1;
    short int i;

    for (i = 0; i<Digits_Count;i++){
      MaxNumber *= 10;
    }
    MaxNumber--;

    if (DiplayUnitNumber > Displays_Count){
      return ;
    }

    if (Number > MaxNumber){
      return ;
    }

    for (i =0; i<Digits_Count ; i++){
        current_digit = Number % 10;
        Number = (Number / 10);
        SevenSegmentModuleSendCommand(i + (3*DiplayUnitNumber) +1, current_digit);
    }
    
}
/*
==================================================
  End Section --> Seven Segment Modules Function Implementation.
==================================================
*/

/*
==================================================
  Start Section --> 
==================================================
*/
static int __init SevenSementSevenSementDisplayModuleInit(void)
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
    if (NULL == (myClass=class_create(THIS_MODULE, "SevenSegmentDisplay"))){
        /*Failed to create Device driver class.*/
        goto ERROR_CLASS;
    }

    /* 4- Second Parameter is the parent file*/
    myDevice = device_create(myClass, NULL, device_number, NULL, "SevenSegmentDisplayModule") ;
    if (NULL == myDevice){
        goto ERROR_DEVICE;
    }
    /*Initialize pins*/

    if (gpio_request(Data_Pin, "SevenSegmentModuleDataPin") ||
        gpio_request(Clock_Pin, "SevenSegmentModuleCLKPin") ||
        gpio_request(Enable_Pin, "SevenSegmentModuleENPin")
    ){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_REQUEST;
    }

    if (
        gpio_direction_output(Data_Pin,   0) ||
        gpio_direction_output(Clock_Pin,    0) ||
        gpio_direction_output(Enable_Pin, 1)
    ){
        /*Can't allocate GPIO*/
        goto ERROR_GPIO_DIR;
    }

    SevenSegmentModuleInitPeripheral();

    return 0;

    ERROR_GPIO_DIR:
        gpio_free(Data_Pin);
        gpio_free(Clock_Pin);
        gpio_free(Enable_Pin);
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

static void __exit SevenSementSevenSementDisplayModuleExit(void)
{

    SevenSegmentModuleDeinitPeripheral();
 
    cdev_del(&CharacterDevice);
    device_destroy(myClass, device_number);
    class_destroy(myClass);
    /*Second argument is the number of devices*/
    unregister_chrdev_region(device_number, 1);
    

}


ssize_t SevenSegmentDisplayModuleDriver_write (struct file *file, const char __user *user_buffer, size_t count, loff_t *off){
    int not_copied, Number;
    short int DiplayUnitNumber;
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
    
    sscanf(buffer, "%hd,%d", &DiplayUnitNumber, &Number);

    SevenSegmentDisplaySetNumber(DiplayUnitNumber, Number);
    /*Return succeessfully count.*/
    return count;

}

static int SevenSegmentDisplayModuleDriver_open(struct inode *devicefile, struct file *instance){
    
    return 0;
}

static int SevenSegmentDisplayModuleDriver_close(struct inode *devicefile, struct file *instance){
  
    return 0;
}

/*
==================================================
  End Section --> 
==================================================
*/
