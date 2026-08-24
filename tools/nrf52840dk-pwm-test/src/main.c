#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static const struct pwm_dt_spec pwm =
	PWM_DT_SPEC_GET(DT_ALIAS(pwm_test));

int main(void)
{
	static const uint8_t duty_percent[] = { 0, 25, 50, 100 };

	if (!pwm_is_ready_dt(&pwm)) {
		printk("PWM device is not ready\n");
		return 0;
	}

	printk("P1.12 PWM test: 1 kHz, 0/25/50/100%%, 5 seconds each\n");

	while (true) {
		for (size_t i = 0; i < ARRAY_SIZE(duty_percent); i++) {
			uint32_t pulse = (pwm.period * duty_percent[i]) / 100U;
			int err = pwm_set_pulse_dt(&pwm, pulse);

			printk("PWM duty: %u%% (err %d)\n", duty_percent[i], err);
			k_sleep(K_SECONDS(5));
		}
	}

	return 0;
}
