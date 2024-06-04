/*
 * hello-5.c - Demonstrates command line argument passing to a module
 */
#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed for KERN_INFO */
#include <linux/init.h>   /* needed for the macros */
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_DESCRIPTION("test hello-5 world module");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myint_array[2] = { -1, -1 };
static int arr_argc = 0;

/*
 * module_param(foo, int, 0000)
 * The first param is the parameters name
 * The second param is it's data type
 * The final argument is the permissions bits
 * for exposing params in sysfs
 */

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param(mylong, long, S_IRUSR);
module_param(mystring, charp, 0000);

module_param_array(myint_array, int, &arr_argc, 0000);

static int __init hello_5_init(void)
{
  int i;
  printk(KERN_INFO "Hello, world5\n=====\n");
  printk(KERN_INFO "myshort is a short integer: %hd\n", myshort);
  printk(KERN_INFO "myint is an integer: %d\n", myint);
  printk(KERN_INFO "mylong is a long integer: %ld\n", mylong);
  printk(KERN_INFO "mystring is a string: %s\n", mystring);

  for (i = 0; i < (sizeof myint_array / sizeof (int)); i++)
  {
    printk(KERN_INFO "myint_array[%d] = %d\n", i, myint_array[i]);
  }
  printk(KERN_INFO "got %d arguments for myint_array\n", arr_argc);

  /*
   * A non 0 return means init_module failed; module can't be loaded
   */
  return 0;
};

static void __exit hello_5_exit(void)
{
  printk(KERN_INFO "Goodbye world 5.\n");
};

module_init(hello_5_init);
module_exit(hello_5_exit);


MODULE_LICENSE("GPL");
