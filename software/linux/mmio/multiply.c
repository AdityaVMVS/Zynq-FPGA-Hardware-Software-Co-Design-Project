/* multiply.c - Simple multiplier kernel module
 *
 * Demonstrates module initialization, module release, printk,
 * and memory-mapped I/O to a hardware multiplier.
 */

#include <linux/module.h>   /* Needed by all modules        */
#include <linux/kernel.h>   /* Needed for KERN_* and printk */
#include <linux/init.h>     /* Needed for __init and __exit */
#include <asm/io.h>         /* Needed for IO reads/writes   */
#include "xparameters.h"    /* Needed for physical address of multiplier */

/* from xparameters.h */
#define PHY_ADDR  XPAR_MULTIPLY_0_S00_AXI_BASEADDR   /* physical address of multiplier */
/* size of physical address range for multiply */
#define MEMSIZE   (XPAR_MULTIPLY_0_S00_AXI_HIGHADDR - \
                   XPAR_MULTIPLY_0_S00_AXI_BASEADDR + 1)

void *virt_addr;     /* virtual address pointing to multiplier */

/* This function is run upon module load. This is where you setup data
 * structures and reserve resources used by the module.
 */
static int __init my_init(void)
{
    printk(KERN_INFO "Mapping virtual address...\n");

    /* Map virtual address to multiplier physical address */
    virt_addr = ioremap(PHY_ADDR, MEMSIZE);
    if (!virt_addr) {
        printk(KERN_ERR "ioremap failed for PHY_ADDR 0x%08x\n", PHY_ADDR);
        return -ENOMEM;
    }

    /* Print physical and virtual addresses */
    printk(KERN_INFO "Multiplier PHY_ADDR = 0x%08x, virt_addr = %p\n",
           PHY_ADDR, virt_addr);

    /* Write 7 to register 0 */
    printk(KERN_INFO "Writing a 7 to register 0\n");
    iowrite32(7, virt_addr + 0);      /* base address + offset 0 */

    /* Write 2 to register 1 */
    printk(KERN_INFO "Writing a 2 to register 1\n");
    iowrite32(2, virt_addr + 4);      /* base address + offset 4 */

    /* Read back registers 0, 1, and 2 */
    printk(KERN_INFO "Read %d from register 0\n",
           ioread32(virt_addr + 0));
    printk(KERN_INFO "Read %d from register 1\n",
           ioread32(virt_addr + 4));
    printk(KERN_INFO "Read %d from register 2\n",
           ioread32(virt_addr + 8));

    /* A non‑zero return means init module failed; module can't be loaded. */
    return 0;
}

/* This function is run just prior to the module's removal from the system.
 * You should release ALL resources used by your module here.
 */
static void __exit my_exit(void)
{
    printk(KERN_ALERT "Unmapping virtual address space...\n");
    if (virt_addr)
        iounmap(virt_addr);
}

/* These define info that can be displayed by modinfo */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ECEN449 Student (and others)");
MODULE_DESCRIPTION("Simple multiplier module");

/* Here we define which functions we want to use for initialization and cleanup */
module_init(my_init);
module_exit(my_exit);
