/*
 * hello.c - The simplest kernel module
 */
#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed for KERN_INFO */
#include <linux/init.h>   /* needed for the macros */

MODULE_DESCRIPTION("test hello world module");

static int __init init_hello(void)
{
  printk(KERN_INFO "Hello world 1\n");

  /*
   * A non 0 return means init_module failed; module can't be loaded
   */
  return 0;
};

static void __exit cleanup_hello(void)
{
  printk(KERN_INFO "Goodbye world 1.\n");
};

module_init(init_hello);
module_exit(cleanup_hello);


MODULE_LICENSE("GPL");
