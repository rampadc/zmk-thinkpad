/*
 * ThinkPad Revision A sequenced PWM backlight.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT thinkpad_reva_pwm_backlight

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

struct reva_backlight_config {
    struct gpio_dt_spec enable;
    struct pwm_dt_spec pwm;
    uint32_t startup_delay_ms;
};

static int reva_backlight_set_brightness(const struct device *dev, uint32_t led,
                                         uint8_t value);

struct reva_backlight_data {
    bool enabled;
#if defined(CONFIG_THINKPAD_T430_REVA_BACKLIGHT_TEST)
    const struct device *dev;
    struct k_work_delayable test_work;
#endif
};

#if defined(CONFIG_THINKPAD_T430_REVA_BACKLIGHT_TEST)
static void reva_backlight_test_work(struct k_work *work) {
    struct k_work_delayable *delayable = k_work_delayable_from_work(work);
    struct reva_backlight_data *data =
        CONTAINER_OF(delayable, struct reva_backlight_data, test_work);
    const struct reva_backlight_config *config = data->dev->config;

    /* Polarity probe: establish a steady high PWM level before connecting BL_5V. */
    if (pwm_set_pulse_dt(&config->pwm, config->pwm.period) != 0) {
        return;
    }
    k_msleep(100);
    if (gpio_pin_set_dt(&config->enable, 1) != 0) {
        pwm_set_pulse_dt(&config->pwm, 0);
        return;
    }
    k_msleep(5000);
    gpio_pin_set_dt(&config->enable, 0);
    pwm_set_pulse_dt(&config->pwm, 0);
}
#endif

static int reva_backlight_set_brightness(const struct device *dev, uint32_t led,
                                         uint8_t value) {
    const struct reva_backlight_config *config = dev->config;
    struct reva_backlight_data *data = dev->data;
    int err;

    if (led != 0 || value > 100) {
        return -EINVAL;
    }

    if (value == 0) {
        err = pwm_set_pulse_dt(&config->pwm, 0);
        if (err) {
            return err;
        }

        err = gpio_pin_set_dt(&config->enable, 0);
        if (!err) {
            data->enabled = false;
        }
        return err;
    }

    if (!data->enabled) {
        err = gpio_pin_set_dt(&config->enable, 1);
        if (err) {
            return err;
        }

        data->enabled = true;
#if defined(CONFIG_THINKPAD_T430_REVA_BACKLIGHT_TEST)
        k_msleep(5000);
#else
        k_msleep(config->startup_delay_ms);
#endif
    }

    err = pwm_set_pulse_dt(&config->pwm,
                           (uint32_t)((uint64_t)config->pwm.period * value / 100));
    if (err) {
        gpio_pin_set_dt(&config->enable, 0);
        data->enabled = false;
    }
    return err;
}

static int reva_backlight_on(const struct device *dev, uint32_t led) {
    return reva_backlight_set_brightness(dev, led, 100);
}

static int reva_backlight_off(const struct device *dev, uint32_t led) {
    return reva_backlight_set_brightness(dev, led, 0);
}

static int reva_backlight_init(const struct device *dev) {
    const struct reva_backlight_config *config = dev->config;
    struct reva_backlight_data *data = dev->data;
    int err;

    if (!device_is_ready(config->enable.port) || !pwm_is_ready_dt(&config->pwm)) {
        return -ENODEV;
    }

    err = gpio_pin_configure_dt(&config->enable, GPIO_OUTPUT_INACTIVE);
    if (err) {
        return err;
    }

    data->enabled = false;
    err = pwm_set_pulse_dt(&config->pwm, 0);
    if (err) {
        return err;
    }

#if defined(CONFIG_THINKPAD_T430_REVA_BACKLIGHT_TEST)
    data->dev = dev;
    k_work_init_delayable(&data->test_work, reva_backlight_test_work);
    k_work_schedule(&data->test_work, K_SECONDS(10));
#endif
    return 0;
}

static const struct led_driver_api reva_backlight_api = {
    .on = reva_backlight_on,
    .off = reva_backlight_off,
    .set_brightness = reva_backlight_set_brightness,
};

#define REVA_BACKLIGHT_DEVICE(inst)                                                            \
    static struct reva_backlight_data reva_backlight_data_##inst;                              \
    static const struct reva_backlight_config reva_backlight_config_##inst = {                  \
        .enable = GPIO_DT_SPEC_INST_GET(inst, enable_gpios),                                    \
        .pwm = PWM_DT_SPEC_GET(DT_INST_CHILD(inst, pwm_led_0)),                                 \
        .startup_delay_ms = DT_INST_PROP(inst, startup_delay_ms),                               \
    };                                                                                          \
    DEVICE_DT_INST_DEFINE(inst, reva_backlight_init, NULL, &reva_backlight_data_##inst,          \
                          &reva_backlight_config_##inst, POST_KERNEL, CONFIG_LED_INIT_PRIORITY,   \
                          &reva_backlight_api);

DT_INST_FOREACH_STATUS_OKAY(REVA_BACKLIGHT_DEVICE)
