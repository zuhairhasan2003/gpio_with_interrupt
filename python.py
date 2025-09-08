from gpiozero import LED, Button
import time

led = LED(22)
button = Button(17)

pressed = False

while not pressed:
	time.sleep(0.01)
	if button.is_pressed:
		led.on()
		pressed = True
	else:
		led.off()



led.close()
button.close()
