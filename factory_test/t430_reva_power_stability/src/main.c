/*
 * Revision A power-stability diagnostic.
 *
 * Holds all three profile LEDs on continuously while forcing both backlight
 * controls low. No USB or Bluetooth stack is enabled.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

static const struct pwm_dt_spec backlight_pwm =
	PWM_DT_SPEC_GET(DT_ALIAS(diagnostic_pwm));

static int output_low(const struct device *port, gpio_pin_t pin)
{
	return gpio_pin_configure(port, pin, GPIO_OUTPUT_LOW);
}

int main(void)
{
	const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	const struct device *gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));

	if (!device_is_ready(gpio0) || !device_is_ready(gpio1)) {
		return 0;
	}

	/* Establish a safe low level before handing P1.04 to the PWM peripheral. */
	output_low(gpio1, 4);

	/* The obsolete Q8 enable remains low. */
	output_low(gpio1, 12);

	/* Revision A profile LEDs are active-low. */
	output_low(gpio1, 8);
	output_low(gpio0, 11);
	output_low(gpio0, 25);

	/* Make a reset visible before applying the backlight load. */
	k_sleep(K_SECONDS(2));

	if (!pwm_is_ready_dt(&backlight_pwm)) {
		/* All LEDs off is the diagnostic error indication. */
		gpio_pin_set(gpio1, 8, 1);
		gpio_pin_set(gpio0, 11, 1);
		gpio_pin_set(gpio0, 25, 1);
		return 0;
	}

	while (true) {
		/* Short known-working T470 duty pulse, followed by a cool/off period. */
		if (pwm_set_pulse_dt(&backlight_pwm,
				     (backlight_pwm.period * 70U) / 100U) != 0) {
			break;
		}
		k_sleep(K_SECONDS(1));

		if (pwm_set_pulse_dt(&backlight_pwm, 0) != 0) {
			break;
		}
		k_sleep(K_SECONDS(5));
	}

	/* All LEDs off is the diagnostic error indication. */
	gpio_pin_set(gpio1, 8, 1);
	gpio_pin_set(gpio0, 11, 1);
	gpio_pin_set(gpio0, 25, 1);

	return 0;
}
