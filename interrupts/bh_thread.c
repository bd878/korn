/*
 * bh_thread.c - Top and bottom half interrupt handling
 *
 * Based upon RPi example by Stefan Wendler:
 *   github.com/wendlers/rpi-kmod-samples
 *
 * Press one button to turn on a LED and another to turn it off
 */
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* for ARRAY_SIZE() */
#include <linux/module.h>
#include <linux/printk.h>

static int button_irqs[] = { -1, -1 };

/* Define GPIOs for LEDs.
 * Must change the numbers for the GPIO
 */
static struct gpio leds[] = { { 4, GPIOF_OUT_INIT_LOW, "LED 1" } };

/* Define GPIOs for BUTTONS
 * Must change the numbers for the GPIOs on board
 */
static struct gpio buttons[] = { { 17, GPIOF_IN, "LED 1 ON BUTTON" },
                                 { 18, GPIOF_IN, "LED 1 OFF BUTTON" } };

/* this happens immediately, when IRQ is triggered */
static irqreturn_t button_top_half(int irq, void *ident)
{
  return IRQ_WAKE_THREAD;
}

/* this can happen at leisure, freeing up IRQs for other high priority task */
static irqreturn_t button_bottom_half(int irq, void *ident)
{
  pr_info("bottom half task starts\n");
  mdelay(500); /* do something which takes a while */
  pr_info("bottom half task ends\n");
  return IRQ_HANDLED;
}

static int __init bottomhalf_init(void)
{
  int ret = 0;

  pr_info("%s\n", __func__);

  /* register LED gpios */
  ret = gpio_request_array(leds, ARRAY_SIZE(leds));

  if (ret) {
    pr_err("unable to request GPIOs for LEDs: %d\n", ret);
    return ret;
  }

  /* register BUTTON gpios */
  ret = gpio_request_array(buttons, ARRAY_SIZE(buttons));

  if (ret) {
    pr_err("unable to request GPIOs for BUTTONS: %d\n", ret);
    goto fail1;
  }

  pr_info("Current button1 value: %d\n", gpio_get_value(buttons[0].gpio));

  ret = gpio_to_irq(buttons[0].gpio);

  if (ret < 0) {
    pr_err("unable to request IRQ: %d\n", ret);
    goto fail2;
  }

  button_irqs[0] = ret;

  pr_info("successfully requested BUTTON1 IRQ # %d\n", button_irqs[0]);

  ret = request_threaded_irq(button_irqs[0], button_top_half,
                             button_bottom_half,
                             IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                             "gpiomod#button1", &buttons[0]);

  if (ret) {
    pr_err("unable to request IRQ: %d\n", ret);
    goto fail2;
  }

  ret = gpio_to_irq(buttons[1].gpio);

  if (ret < 0) {
    pr_err("unable to request IRQ: %d\n", ret);
    goto fail2;
  }

  button_irqs[1] = ret;

  pr_info("Successfully requested button2 irq # %d\n", button_irqs[1]);

  ret = request_threaded_irq(button_irqs[1], button_top_half,
                             button_bottom_half,
                             IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                             "gpiomod#button2", &buttons[1]);

  if (ret) {
    pr_err("unable to request irq: %d\n", ret);
    goto fail3;
  }

  return 0;

  /* cleanup what has been setup so far */
fail3:
  free_irq(button_irqs[0], NULL);

fail2:
  gpio_free_array(buttons, ARRAY_SIZE(leds));

fail1:
  gpio_free_array(leds, ARRAY_SIZE(leds));

  return ret;
}

static void __exit bottomhalf_exit(void)
{
  int i;

  pr_info("%s\n", __func__);

  /* free irqs */
  free_irq(button_irqs[0], NULL);
  free_irq(button_irqs[1], NULL);

  /* turn all LEDs off */
  for (i = 0; i < ARRAY_SIZE(leds); i++)
    gpio_set_value(leds[i].gpio, 0);

  /* unregister */
  gpio_free_array(leds, ARRAY_SIZE(leds));
  gpio_free_array(buttons, ARRAY_SIZE(buttons));
}

module_init(bottomhalf_init);
module_exit(bottomhalf_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Interrupt with top and bottom half");
