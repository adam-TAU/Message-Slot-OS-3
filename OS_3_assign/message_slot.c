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
	size_t message_length;
	unsigned int channel_id;
	struct channel_entry *next;
} channel_entry;

//------------------------------------------------------------------------------
// a data structure used to maintain message slots: A Linked-List of Channels
typedef struct slot_entry {
	unsigned int dev_ref_count;
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
/* This function extracts and returns a valid pointer to the message channel entry,
 * in the given message slot <message_slot> which corresponds to the channel id <channel_id>.
 * Returns NULL if no channel found. */
static channel_entry* message_slot_get_channel_entry(slot_entry message_slot, unsigned int channel_id);
//------------------------------------------------------------------------------
/* This function returns true if the message slot is currently maintaining a channel with the channel id <channel_id>.
 * Else, false is returned */
static bool message_slot_contains(slot_entry message_slot, unsigned int channel_id);
//------------------------------------------------------------------------------
/* This function accepts a message slot <message_slot> and a channel id <channel_id>, and is ought to change the channel buffer
 * length (size) to <message_length>. Like, that, we would maintain the obliged space complexity, 
 * of O(C * M). If a channel with such channel id doesn't exist in the message slot's Data Structure (Linked list of Channels),
 * we simply create such channel and insret it into the start of the Linked List of Channels of the message slot.
 * 
 * Return values:
 * 		- We must check that the message length <message_length> fits are requirement of at most 128 bytes, but at least one byte.
 *		  If this check fails, NULL is returned and errno is set to EMSGSIZE.
 *		- Allocating memory might result in an error. If such error occurs, NULL is returned and errno is set ENOMEM.
 *		- On any other successful run, a valid pointer to the updated message channel entry is returned.
 */ 
static channel_entry* message_slot_update_channel(slot_entry message_slot, unsigned int channel_id, size_t message_length);
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
static channel_entry* message_slot_get_channel_entry(slot_entry message_slot, unsigned int channel_id) {
	channel_entry *curr = message_slot->head;
	
	do {
		curr = curr -> next;
		if (curr -> channel_id == channel_id) {
			return curr;
		}
		
	} while (curr != NULL);
	
	return NULL;
}
//------------------------------------------------------------------------------
static bool message_slot_contains(slot_entry message_slot, unsigned int channel_id) {
	if (NULL != message_slot_get_channel_entry(message_slot, channel_id)) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
static channel_entry* message_slot_update_channel(slot_entry message_slot, unsigned int channel_id, size_t message_length) {
  // validating message length
  if (message_length == 0 || message_length > 128) {
  	errno = EMSGSIZE;
  	return NULL;
  }

  // extracting the channel entry in the message slot corresponding to <channel_id>
  channel_entry *curr_channel = message_slot_get_channel_entry(message_slot, channel_id);
  
  // if a channel entry was never created that corresponds to the channel id under the message slot:
  // create one and add it to the start of the message slot's linked list of channels
  if (NULL == curr_channel) {
	  curr_channel = kmalloc(sizeof(channel_entry), GFP_KERNEL);
	  if (!curr_channel) {
	  	errno = ENOMEM;
	  	return NULL;
	  }

	  curr_channel->channel_id = channel_id;
	  curr_channel->next = message_slot.head;
	  message_slot.head = curr_channel;
  }
  
  // reallocating memory for the channel's buffer (To maintain the space complexity of: O(C * M))
  if (NULL == curr_channel -> message) {
  	 kfree(curr_channel -> message);
  }
  curr_channel -> message = kmalloc(sizeof(char) * length, GFP_KERNEL);
  
  // changing the message length
  curr_channel -> message_length = message_length;
  
  // returning the updated channel
  return curr_channel;
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
  printk("Invoking device_write(%p,%ld)\n", file, length);

  // verifying that the file descriptor holds a valid channel id
  if (!is_channel_set(file)) {
  	errno = EINVAL;
  	return -1;
  }
  
  // extracting the minor of the message slot
  unsigned int minor = get_minor_from_file(file);
  
  // extracting the message-channel entry
  channel_entry *curr_channel;
  if (NULL == (curr_channel = message_slot_get_channel_entry(slots[minor], file->private_data)) ) { // if no channel is found, message mustn't exist
  	errno = EWOULDBLOCK;
  	return -1;
  } else if (NULL == curr_channel -> message) { // if channel exists, but with no message inside
  	errno = EWOULDBLOCK;
  	return -1;
  }
  
  // validating that the user buffer is big enough for the message
  if (length < channel_entry -> message_length) {
  	errno = ENOSPC;
  	return -1;
  }
  
  // reading the message lying in the channel buffer
  for( size_t i = 0; i < length && i < BUF_LEN; ++i ) {
    if (0 != put_user(curr_channel->message[i], &buffer[i])) {
    	errno = ECANCELED;
    	return -1;
    }
  }
  
  return SUCCESS;
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
  
  // verifying that the file descriptor holds a valid channel id
  if (!is_channel_set(file)) {
  	errno = EINVAL;
  	return -1;
  }
  
  // extracting the minor of the message slot
  unsigned int minor = get_minor_from_file(file);
  
  // updating the messages channel
  channel_entry *curr_channel;
  if (NULL == (curr_channel = message_slot_update_channel(slots[minor], file->private_data, length)) ) {
  	return -1;
  }
  
  // writing the message from the user into the channel's buffer
  for( size_t i = 0; i < length && i < BUF_LEN; ++i ) {
    if (0 != get_user(curr_channel->message[i], &buffer[i])) {
    	errno = ECANCELED;
    	return -1;
    }
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
  
  // freeing all of the memory used by different message-slot's message-channels
  for (size_t i = 0; i < 256; i++) {
  	channel_entry* curr = slots[i].head;
  	channel_entry* tmp;
  	
  	do {
  		if (curr->message != NULL) kfree(curr->message); // free the space the message occupied
  		tmp = curr; // save the current channel entry
  		curr = curr->next; // advance in the Linked List of Channels
  		kfree(tmp); // free previous channel entry
  		
  	} while (curr != NULL);
  }
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================

