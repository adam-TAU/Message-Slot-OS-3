/* Submitter: Adam
 * Operating Systems, 2022A
 * Tel Aviv University
 ============================
 * Message Slot Device Driver
*/





/*******************************************************************************************
	1. REFERENCES: a large portion of this code was copied from RECITATION 6 of this course
	2. 
*******************************************************************************************/ 






// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE


#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/slab.h>
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/

#include "message_slot.h"


#define channel_amount
MODULE_LICENSE("GPL");

// a data structure used to maintain message channels
typedef struct channel_entry {
	char* message;
	struct channel_entry next;
} channel_entry;

// a data structure used to maintain message slots
typedef struct slot_entry {
	int device_count = 0;
	channel_entry head;
} slot_entry;

// an array that maintains the info about all open message slots
static slot_entry slots[256];

// used to prevent concurent access into the same device
static int dev_open_flag = 0;

static struct chardev_info device_info;


//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
  unsigned long flags; // for spinlock
  printk("Invoking device_open(%p)\n", file);

  // We don't want to talk to two processes at the same time
  spin_lock_irqsave(&device_info.lock, flags);
  if( 1 == dev_open_flag )
  {
    spin_unlock_irqrestore(&device_info.lock, flags);
    return -EBUSY;
  }

  ++dev_open_flag;
  spin_unlock_irqrestore(&device_info.lock, flags);
  return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
  unsigned long flags; // for spinlock
  printk("Invoking device_release(%p,%p)\n", inode, file);

  // ready for our next caller
  spin_lock_irqsave(&device_info.lock, flags);
  --dev_open_flag;
  spin_unlock_irqrestore(&device_info.lock, flags);
  return SUCCESS;
}

//---------------------------------------------------------------
// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  // read doesnt really do anything (for now)
  printk( "Invocing device_read(%p,%ld) - "
          "operation not supported yet\n"
          "(last written - %s)\n",
          file, length, the_message );
  //invalid argument error
  return -EINVAL;
}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  int i;
  printk("Invoking device_write(%p,%ld)\n", file, length);
  for( i = 0; i < length && i < BUF_LEN; ++i ) {
    get_user(the_message[i], &buffer[i]);
  }
 
  // return the number of input characters used
  return i;
}

//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
	if ( MSG_SLOT_CHANNEL == ioctl_command_id ) {

		if (0 == ioctl_param) {
			errno = EINVAL;
			return -1;
		}
		
		// change the channel accordin to ioctl_param
		file->private_data = (void*) ioctl_param;
		return SUCCESS;
		
	} else {
		errno = EINVAL;
		return -1;
	}
}

//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops =
{
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init simple_init(void)
{
  // Register driver capabilities. Obtain major num
  // Return value other than 0 signfies an error: 
  // http://www.linuxsavvy.com/resources/linux/man/man9/register_chrdev.9.html#:~:text=RETURN%20VALUE-,On,-success%2C%20register_chrdev%20returns
  if (0 != register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );
  
    printk( KERN_ERR "%s registraion failed for  %d\n", DEVICE_FILE_NAME, MAJOR_NUM );
    return -1;
  }

  printk( "Registeration is successful. ");
  printk( "If you want to talk to the device driver,\n" );
  printk( "you have to create a device file:\n" );
  printk( "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM );
  printk( "You can echo/cat to/from the device file.\n" );
  printk( "Dont forget to rm the device file and "
          "rmmod when you're done\n" );

  return 0;
}

//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
  // Unregister the device
  // Should always succeed
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
  
  // The allocated memory for the message slot gets freed on termination, therefore we don't free it
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================

