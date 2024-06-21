/*
 * sleep.c - create a /proc file, and if several processes try to open it
 * at the same time, put all but one to sleep
 */

#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/kernel.h> /* for sprintf() */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h> /* because we use proc fs */
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>
#include <linux/wait.h> /* for putting processes to sleep and waking them up */

#include <asm/current.h>
#include <asm/errno.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

/* here we keep the last message received, to prove that we can
 * process our input
 */
#define MESSAGE_LENGTH 80
static char message[MESSAGE_LENGTH];

static struct proc_dir_entry *our_proc_file;
#define PROC_ENTRY_FILENAME "sleep"

/* Since we use the file operations struct, we can't use the special proc
 * output privisions - we have to use a standard read function, which is this function
 */
static ssize_t module_output(struct file *file,/* see include/linux/fs.h */
                             char __user *buf, /* the buffer to put data to data in user segment */
                             size_t len,       /* the length of the buffer */
                             loff_t *offset)
{
  static int finished = 0;
  int i;
  char output_msg[MESSAGE_LENGTH + 30];

  /* return 0 to signify end of file - that we have nothing more
   * to say at this point
   */
  if (finished) {
    finished = 0;
    return 0;
  }

  sprintf(output_msg, "last input:%s\n", message);
  for (i = 0; i < len && output_msg[i]; i++)
    put_user(output_msg[i], buf + i);

  finished = 1;
  return i; /* return the number of bytes "read" */
}

/* this function sreceives input from the user
 * when the user writes to the /proc file
 */
static ssize_t module_input(struct file *file, /* the file itself */
                            const char __user *buf, /* the buffer with input */
                            size_t length, /* the buffer's length */
                            loff_t *offset) /* offset to file - ignore */
{
  int i;

  /* put the input into message, where module_output will later be able
   * to use it
   */
  for (i = 0; i < MESSAGE_LENGTH - 1 && i < length; i++)
    get_user(message[i], buf + i);
  /* we want a standard, zero terminating string */
  message[i] = '\0';

  /* we need to return the number of input characters used */
  return i;
}

/* 1 if the file is currently open by somebody */
static atomic_t already_open = ATOMIC_INIT(0);

/* queue of processes who want our file */
static DECLARE_WAIT_QUEUE_HEAD(waitq);

/* called when the /proc file is opened */
static int module_open(struct inode *inode, struct file *file)
{
  /* if the file's flags include O_NONBLOCK, it means the process does not
   * want to wait for the file. In this case, if the file is already open,
   * we should fail with -EAGAIN, meaning "you will have to try again",
   * instead of blocking a process which would rather stay awake.
   */
  if ((file->f_flags & O_NONBLOCK) && atomic_read(&already_open))
    return -EAGAIN;

  /* this is the correct place for try_module_get(THIS_MODULE) because if
   * a process is in the loop, which is within the kernel module,
   * the kernel module must not be removed.
   */
  try_module_get(THIS_MODULE);

  while (atomic_cmpxchg(&already_open, 0, 1)) {
    int i, is_sig = 0;

    /* this function puts the current process, including any system calls,
     * such as us, to sleep. Execution will be resumed right after the fuction call,
     * either because somebody called wake_up(&waitq) (only module_close does that, when the file
     * is closed) or when a signal is sent
     */
    wait_event_interruptible(waitq, !atomic_read(&already_open));

    /* if we woke up because we got a signal we're not blocking,
     * return -EINTR (fail the system call). This allows processes
     * to be killed or stopped.
     */
    for (i = 0; i < _NSIG_WORDS && !is_sig; i++)
      is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];

    if (is_sig) {
      /* it is important to put module_put(THIS_MODULE) here, because
       * for processes where the open is interrupted there
       * will never be a corresponding close. IF we do not decrement the usage count here,
       * we will be left with a positive usage count,
       * which we will have no way to bring down to zero, giving us
       * an immportal module, which can only be killed by rebooting
       * the machine.
       */
      module_put(THIS_MODULE);
      return -EINTR;
    }
  }

  return 0; /* allow the access */
}

/* called when the /proc file is closed */
static int module_close(struct inode *inode, struct file *file)
{
  /* set already_open to zero, so one of the processes in the waitq
   * will be able to set already_open back to one and to open the file.
   * All the other proceses will be called when already_open is back to one,
   * so they'll go back to sleep
   */
  atomic_set(&already_open, 0);

  /* wake up all the processes in waitq, so if anybody is waiting for the file
   * , they can have it
   */
  wake_up(&waitq);

  module_put(THIS_MODULE);

  return 0; /* success */
}

/* structures to register as the /proc file, with pointers to all the relevant
 * functions
 */

/* file operations for our proc file. This is where we place pointers to all
 * the functions called when somebody tries to do something to our file.
 * NULL means we don't want to deal with something.
 */
#ifdef HAVE_PROC_OPS
static const struct proc_ops file_ops_4_our_proc_file = {
  .proc_read = module_output, /* "read" from the file */
  .proc_write = module_input, /* "write" to the file */
  .proc_open = module_open, /* called when the /proc file is opened */
  .proc_release = module_close, /* called when it's closed */
  .proc_lseek = noop_llseek, /* return file->f_ops */
};

#else
static const struct file_operations file_ops_4_our_proc_file = {
  .read = module_output,
  .write = module_input,
  .open = module_open,
  .release = module_close,
  .llseek = noop_llseek,
};
#endif

/* initialize the module = register the proc file */
static int __init sleep_init(void)
{
  our_proc_file =
    proc_create(PROC_ENTRY_FILENAME, 0644, NULL, &file_ops_4_our_proc_file);
  if (our_proc_file == NULL) {
    pr_debug("Error: could not initialize /proc/%s\n", PROC_ENTRY_FILENAME);
    return -ENOMEM;
  }
  proc_set_size(our_proc_file, 80);
  proc_set_user(our_proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

  pr_info("/proc/%s created\n", PROC_ENTRY_FILENAME);

  return 0;
}

/* cleanup - unregister out file from /proc.
 * this could get dangerous if there are still processes
 * waiting in waitq, because they are inside our
 * open function, which will get unloaded.
 */
static void __exit sleep_exit(void)
{
  remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
  pr_debug("/proc/%s removed\n", PROC_ENTRY_FILENAME);
}

module_init(sleep_init);
module_exit(sleep_exit);

MODULE_LICENSE("GPL");
