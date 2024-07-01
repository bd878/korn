/*
 * print_string.c - Send output to the tty we'are running on, regardless if
 * it is through X11, telnet etc. We do this by printing the string to the
 * tty associated with the current task
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h> /* for current */
#include <linux/tty.h> /* for the tty declarations */

static void print_string(char *str)
{
  /* the tty for the current task */
  struct tty_struct *my_tty = get_current_tty();

  /* if my_tty is NULL, the current task has no tty you can print to
   * (i.e., if it is a daemon). If so, there is nothing we can do
   */
  if (my_tty) {
    const struct tty_operations *ttyops = my_tty->driver->ops;
    /* my_tty->driver holds the tty's functions,
     * one of which is used to write strings to the tty.
     * It can be used to take strings either from the user's or
     * kernel's memory segment
     *
     * The function's 1st param is the tty to write to,
     * same func would normally be used for all tty's of a certain
     * type.
     * The 2nd param is a pointer to a string.
     * The 3rd param is the length of the string.
     *
     * As you will see below, sometimes it's necessary to use
     * preprocessor stuff to create code that works for
     * different kernel versions. The (naive) aproach we've taken here does not
     * scale well. The right way to deal with this is described in
     * section 2
     * of Documentation/SubmittingPatches
     */
    (ttyops->write)(my_tty, /* the tty itself */
                    str, /* string */
                    strlen(str)); /* length */

    /*
     * ttys were originally hardware devices, which 
     * strictly followed ASCII standard.
     * In ASCII, to move to a new line
     * you need two chars: \r\n. On unix, the ASCII
     * line feed is used for both purposes - we cannot 
     * just use \n, because it would not have a carriage return 
     * and the next line will start at the column rifht after
     * the line feed.
     *
     * This is why text files are different between unix and MS Windows.
     * In MS-DOS and MS Windows, ASCII standard was
     * strictly adhered to, and therefore a newline
     * requires both a LF and a CR
     */
    (ttyops->write)(my_tty, "\015\012", 2);
  }
}

static int __init print_string_init(void)
{
  print_string("the module has been inserted. Hello world!");
  return 0;
}

static void __exit print_string_exit(void)
{
  print_string("the module has been removed");
}

module_init(print_string_init);
module_exit(print_string_exit);

MODULE_LICENSE("GPL");
