/*
 * start.c - Illustration of multi filed modules
 */

#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h>
#include <linux/init.h>

MODULE_DESCRIPTION("multifile test module");

static int __init init_start(void)
{
  printk(KERN_INFO "Hello, world - this is the kernel speaking club\n");
  return 0;
};

module_init(init_start);

MODULE_LICENSE("GPL");
