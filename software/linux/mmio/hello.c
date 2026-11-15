/*
* hello . c − Hello World kernel module
* Demonstrates module i n i t i a l i z a t i o n , module release and printk .
*
* ( Adapted from various example modules including those found in the
* Linux Kernel Programming Guide , Linux Device Drivers book and
* FSM’ s device driver t u t o r i a l )
*/

#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN */
#include <linux/init.h>

/* This function is run upon module load . This is where you setup data
s t ructures and reserve resources used by the module . */
static int __init my_init (void)
{
/* Linux kernel ’ s version of p r i n t f */
printk (KERN_INFO  "Hello world !\n" ) ;
//  A non 0 return means init module failed ; module can ’ t be loaded .
return 0;
}
/* This function is run j u s t prior to the module ’ s removal from the
system .  You should release ALL resources used by your module here ( otherwise be prepared for a reboot ) . */

static void __exit my exit (void)
{

printk (KERN_ALERT  "Goodbye world !\n" ) ;
}

/* These define info that can be displayed by modinfo */
MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("ECEN449 Student ( and others )") ;
MODULE_DESCRIPTION("Simple Hello World Module") ;
/* Here we define which functions we want to use for i n i t i a l i z a t i o n
and cleanup */
module_init(my_init);
module_exit(my_exit);
