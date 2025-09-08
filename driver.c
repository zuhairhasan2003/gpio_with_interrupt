#include<linux/gpio.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/interrupt.h>

// gpio ---------------------------------------------------------------------------------------------
#define BASE 571
int ledPinNum = BASE + 22; // attach led to gpio 22 of raspberry pi 5
int ledValue = 0;

int buttonPinNum = BASE + 17; // attach button to gpio 17 of raspberry pi 5

// interrupt -----------------------------------------------------------------------------------------
int irq;

static irqreturn_t gpio_irq_handler(int irq,void *dev_id)
{
	printk("Driver - Button pressed\n");
	ledValue = (ledValue == 0 ? 1 : 0);
	gpio_set_value(ledPinNum, ledValue);
	return IRQ_HANDLED;
}

// ---------------------------------------------------------------------------------------------------

bool init_gpio_output(int gpioPinNum , int value)
{
	if(!gpio_is_valid(gpioPinNum))
	{
		printk("Driver - Gpio pin is invalid\n");
		return false;
	}

	if(gpio_request(gpioPinNum , "GpioLed"))
	{
		printk("Driver - Gpio request failed\n");
		return false;
	}

	gpio_direction_output(gpioPinNum , value);

	printk("GPIO - %d , set to output\n", gpioPinNum);

	return true;
}

bool init_gpio_input(int gpioPinNum)
{
	if(!gpio_is_valid(gpioPinNum))
	{
		printk("Driver - Gpio pin is invalid\n");
		return false;
	}

	if(gpio_request(gpioPinNum , "GpioButton"))
	{
		printk("Driver - Gpio request failed\n");
		return false;
	}

	gpio_direction_input(gpioPinNum);

	printk("GPIO - %d , set to input\n", gpioPinNum);

	return true;
}

int my_init(void)
{
	ledValue = 0;
	if(!init_gpio_output(ledPinNum , ledValue) || !init_gpio_input(buttonPinNum))
	{
		return -1;
	}

	gpio_set_value(ledPinNum , ledValue);

	irq = gpio_to_irq(buttonPinNum);

	if(irq < 0)
	{
		printk("Driver - gpio_to_irq() failed\n");
		return -1;
	}

	if(request_irq(irq, gpio_irq_handler, IRQF_TRIGGER_RISING, "myButton", NULL))
	{
		printk("Driver - request_irq() failed\n");
		return -1;
	}

	printk("Driver - Module init with irq number : %d\n", irq);
	return 0;
}

void my_exit(void)
{
	gpio_set_value(ledPinNum , 0);
	gpio_free(ledPinNum);
	gpio_free(buttonPinNum);
	free_irq(irq, NULL);
	printk("Driver - Module exit\n");
}

MODULE_LICENSE("GPL");
module_init(my_init);
module_exit(my_exit);
