




// Defining __KERNEL__ and MODULE allows us
// to access kernel-level code.
// --------------------------------------------------------

// We are part of the kernel
#undef __KERNEL__
#define __KERNEL__


#undef MODULE
#define MODULE

// included for all kernel modules
#include <linux/module.h>
// included for __init and __exit macros
#include <linux/init.h>







//--- loader ---------------------------------------------
static __init int module_init(void)
{
  printk("Initializing the message slot module!\n");
  return 0;
}

//--- unloader -------------------------------------------
static void __exit module_cleanup(void)
{
  printk("Destructing the message slot module!\n");
}

//--------------------------------------------------------
module_init(module_init);
module_exit(module_cleanup);

//=================== END OF FILE ========================
