/*
 * procfs1.c - create a "file" in /proc
 */

#include <linux/module.h>
#include <linux/kernel.h>  /* kernel work */
#include <linux/proc_fs.h> /* because we use proc fs */

#define procfs_name "helloworld"
#define BUF_LEN 80            /* Max length of the message from the device */

static char msg[BUF_LEN];     /* The msg device will give when asked */
static char *msg_Ptr;
ssize_t procfile_read(struct file *, char *, size_t, loff_t *);

/**
 * This structure hold info about the /proc file
 */
struct proc_dir_entry *Our_Proc_File;
const struct proc_ops fops = {
  .proc_read = procfile_read
};

/*
 * Put data into the proc fs file.
 *
 * Usage and return value
 * ======================
 * A return value of zero means you have no further
 * info at this time (end of file).
 * A negative return value is an error condition.
 *
 * For more info, look at source:
 * <kernel source dir>/fs/proc/array.c
 *
 * Also, see simple_read_from_buffer:
 * <kernl source dir>/fs/libfs.c
 */

ssize_t procfile_read(struct file *flip, char *buffer, size_t length, loff_t *offset)
{
  static int counter = 0;

  int bytes_read = 0;

  printk(KERN_INFO "procfile_read (proc/%s) called\n", procfs_name);

  counter++;
  sprintf(msg, "I already told you %d times Hello world!\n", counter);
  msg_Ptr = msg;

  printk(KERN_INFO "offset: %lli\n", *offset);
  /*
   * We give all of our info in one go,
   * so if the user asks us if we have more info,
   * the answer should always be no.
   *
   * This is important because the standard read
   * function from the library would continue to issue
   * the read system call until the kernel
   * replies that it has no more info, or until its
   * buffer is filled
   */
  if (offset > 0) {
    /* we have finished to read, return 0 */
    return 0;
  }

  /*
   * Actualy put the data into the buffer
   */
  while (length && *msg_Ptr) {
    /*
     * The buffer is in the user data segment, not kernel segment.
     * "*" assignment won't work. We have to use put_user,
     * which copies data from kernel data segment to the user data segment
     */
    put_user(*(msg_Ptr++), buffer++);

    length--;
    bytes_read++;
  }

  /* fill the buffer, return the buffer size */
  return bytes_read;
}

int init_module(void)
{
  Our_Proc_File = proc_create(procfs_name, 0644, NULL, &fops);

  if (Our_Proc_File == NULL)
  {
    remove_proc_entry(procfs_name, Our_Proc_File);
    printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
      procfs_name);
    return -ENOMEM;
  }

  printk(KERN_INFO "/proc/%s created\n", procfs_name);
  return 0; /* everything is ok */
}

void cleanup_module(void)
{
  remove_proc_entry(procfs_name, Our_Proc_File);
  printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}

MODULE_LICENSE("GPL");

