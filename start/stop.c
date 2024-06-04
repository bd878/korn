/*
 * stop.c - illustratin of multifiled modules
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static void __exit cleanup_start(void)
{
  printk(KERN_INFO "Goodbye world 1.\n");
};


module_exit(cleanup_start);
