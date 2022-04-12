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

// inclusions
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/slab.h>
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include "message_slot.h"




//================== AUXILIARY DEFINIIONS/INITIALIZATIONS ===========================
// simple boolean interface
#define true 1
#define false 0
#define bool int

//------------------------------------------------------------------------------
// the channel id for a file descriptor that has yet to be assigned a communication message channel over its message slot
#define CHANNEL_NOT_SET -1

//------------------------------------------------------------------------------
// module license
MODULE_LICENSE("GPL");

//------------------------------------------------------------------------------
// a data structure used to maintain message channels: A Linked-List Cell
typedef struct channel_entry {
	char* message;
	unsigned int channel_id;
	unsigned int channel_ref_count;
	struct channel_entry *next;
} channel_entry;

//------------------------------------------------------------------------------
// a data structure used to maintain message slots: A Linked-List of Channels
typedef struct slot_entry {
	int dev_ref_count = 0;
	channel_entry *head;
} slot_entry;

//------------------------------------------------------------------------------
// an array that maintains the info about all open message slots
static slot_entry slots[256];

//------------------------------------------------------------------------------
// character device info (no idea if should preserve)
static struct chardev_info device_info;
//------------------------------------------------------------------------------








//================== AUXILIARY FUNCTION DECLARATIONS ===========================
/* This function extracts the channel id that resides inside the file struct of the file descriptor.
 * In case the channel id is set (i.e., valid - greater than 0), return true.
 * In case the channel id is not set (i.e., invlaid - equals to 0), return false. */
static bool is_channel_set(struct file* file);
//------------------------------------------------------------------------------
/* This function extracts from the file struct of a file descriptor the minor number.
 * This is done by accessing the dentry field in the file struct, which contains the path field,
 * which contains a pointer to the inode. See: https://www.oreilly.com/library/view/linux-device-drivers/0596000081/ch03s04.html#:~:text=filp%2D%3Ef_dentry%2D%3Ed_inode.
 * By that, we simply extract the minor number from the inode struct. */
static unsigned int get_minor_from_file(struct file* file);
//------------------------------------------------------------------------------
/* This function extracts the channel entry using <channel_id>, from the message slot Data-Structure (A Linked-List of Channels).
 * Returns NULL if no channel found. */
static channel_entry get_channel_entry(slot_entry message_slot, unsigned int channel_id);
//------------------------------------------------------------------------------







//================== AUXILIARY FUNCTION DEFINITIONS===========================
static bool is_channel_set(struct file* file) {
	if (filp->private_data == CHANNEL_NOT_SET) { 
		return false;
	}
	
	return true;
}
//------------------------------------------------------------------------------
static unsigned int get_minor_from_file(struct file* file) {
	return iminor(file->f_dentry->d_inode);
}
//------------------------------------------------------------------------------
static channel_entry get_channel_entry(slot_entry message_slot, unsigned int channel_id) {
	channel_entry *curr = message_slot->head;
	
	do {
		curr = curr -> next;
		if (curr -> channel_id == channel_id) {
			return *curr;
		}
		
	} while (curr != NULL);
	
	return NULL;
}
//------------------------------------------------------------------------------







//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
	// an indicator for further instructions that a channel has not been set since opening
	file->private_data = (void*) CHANNEL_NOT_SET;
	
	// getting the minor number
	unsigned int minor = iminor(inode);
	
	// incrementing the reference count of the message slot
	slot_entry[minor].dev_ref_count++;
	
	return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
	// getting the minor number
	unsigned int minor = iminor(inode);
	
	// decrementing the reference count of the message slot
	slot_entry[minor].dev_ref_count--;
	
	// if reference count is lower than 1, remove the 
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
  printk("Invoking device_write(%p,%ld)\n", file, length);
  
  // extracting the minor of the message slot
  unsigned int minor = get_minor_from_file(file);
  
  // extracting the channel entry for the communiation channel corresponding to the one set in the file descriptor
  channel_entry channel = get_channel_entry(slots[minor], file->private_data);
  
  // allocating memory for the channel's buffer (TODO)
  
  
  // writing the message from the user into the channel's buffer
  for( int i = 0; i < length && i < BUF_LEN; ++i ) {
    get_user(channel.message[i], &buffer[i]);
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

		// if the channel id is invalid
		if (0 == ioctl_param) { 
			errno = EINVAL;
			return -1;
		}
		
		// change the channel accordin to ioctl_param
		file->private_data = (void*) ioctl_param;
		return SUCCESS;		
	}
	
	// in case the command specified wasn't the MSG_SLOT_CHANNEL command
	errno = EINVAL;
	return -1;
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
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================

