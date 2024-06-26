/*
 * example_rwlock.c
 */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/rwlock.h>

static DEFINE_RWLOCK(myrwlock);

static void example_read_lock(void)
{
  unsigned long flags;

  read_lock_irqsave(&myrwlock, flags);
  pr_info("Read locked\n");

  /* read from something */

  read_unlock_irqrestore(&myrwlock, flags);
  pr_info("Read unlocked\n");
}

static void example_write_lock(void)
{
  unsigned long flags;

  write_lock_irqsave(&myrwlock, flags);
  pr_info("write locked\n");

  /* write to something */
  write_unlock_irqrestore(&myrwlock, flags);
  pr_info("write unlocked\n");
}

static int __init example_rwlock_init(void)
{
  pr_info("example_rwlock started\n");

  example_read_lock();
  example_write_lock();

  return 0;
}

static void __exit example_rwlock_exit(void)
{
  pr_info("example_rwlock exit\n");
}

module_init(example_rwlock_init);
module_exit(example_rwlock_exit);

MODULE_DESCRIPTION("Read/write locks example");
MODULE_LICENSE("GPL");
