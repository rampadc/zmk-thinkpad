/*
 * ThinkPad T470 BLE profile indicators.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

LOG_MODULE_REGISTER(thinkpad_t470_status, CONFIG_ZMK_LOG_LEVEL);

static const struct gpio_dt_spec profile_leds[] = {
    GPIO_DT_SPEC_GET(DT_NODELABEL(t470_profile_1_led), gpios),
    GPIO_DT_SPEC_GET(DT_NODELABEL(t470_profile_2_led), gpios),
    GPIO_DT_SPEC_GET(DT_NODELABEL(t470_profile_3_led), gpios),
};

static struct k_work_delayable profile_work;
static bool ready;
static bool blink_on;
static int64_t identify_until;
static int last_profile = -1;

#define PROFILE_IDENTIFY_DURATION_MS 5000
#define PROFILE_PAIRING_BLINK K_MSEC(150)
#define PROFILE_DISCONNECTED_ON K_MSEC(120)
#define PROFILE_DISCONNECTED_OFF K_MSEC(1880)

static void set_led(const struct gpio_dt_spec *led, bool on) {
    int err = gpio_pin_set_dt(led, on ? 1 : 0);

    if (err) {
        LOG_ERR("Failed to set profile LED pin %u: %d", led->pin, err);
    }
}

static void set_selected_led(bool on) {
    int selected = zmk_ble_active_profile_index();

    for (size_t i = 0; i < ARRAY_SIZE(profile_leds); i++) {
        set_led(&profile_leds[i], on && selected == (int)i);
    }
}

static void schedule_profile(k_timeout_t delay) {
    if (ready) {
        k_work_reschedule(&profile_work, delay);
    }
}

static void profile_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    int64_t now = k_uptime_get();

    if (zmk_endpoints_selected().transport == ZMK_TRANSPORT_USB) {
        set_selected_led(false);
        return;
    }

    if (zmk_ble_active_profile_is_connected()) {
        if (now < identify_until) {
            set_selected_led(true);
            schedule_profile(K_MSEC(identify_until - now));
        } else {
            set_selected_led(false);
        }
        return;
    }

    blink_on = !blink_on;
    set_selected_led(blink_on);

    if (zmk_ble_active_profile_is_open()) {
        schedule_profile(PROFILE_PAIRING_BLINK);
    } else {
        schedule_profile(blink_on ? PROFILE_DISCONNECTED_ON : PROFILE_DISCONNECTED_OFF);
    }
}

static void refresh_profile(bool identify) {
    if (!ready) {
        return;
    }

    blink_on = false;
    identify_until = identify ? k_uptime_get() + PROFILE_IDENTIFY_DURATION_MS : 0;
    schedule_profile(K_NO_WAIT);
}

static int profile_listener(const zmk_event_t *eh) {
    if (as_zmk_ble_active_profile_changed(eh) != NULL) {
        int profile = zmk_ble_active_profile_index();
        bool changed = profile != last_profile;

        last_profile = profile;
        refresh_profile(changed || zmk_ble_active_profile_is_connected());
    } else if (as_zmk_endpoint_changed(eh) != NULL) {
        refresh_profile(zmk_endpoints_selected().transport == ZMK_TRANSPORT_BLE);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(thinkpad_t470_profile, profile_listener);
ZMK_SUBSCRIPTION(thinkpad_t470_profile, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(thinkpad_t470_profile, zmk_endpoint_changed);

static int profile_init(void) {
    for (size_t i = 0; i < ARRAY_SIZE(profile_leds); i++) {
        if (!gpio_is_ready_dt(&profile_leds[i])) {
            return -ENODEV;
        }

        int err = gpio_pin_configure_dt(&profile_leds[i], GPIO_OUTPUT_INACTIVE);
        if (err) {
            return err;
        }
    }

    k_work_init_delayable(&profile_work, profile_work_handler);
    last_profile = zmk_ble_active_profile_index();
    ready = true;
    refresh_profile(false);

    return 0;
}

SYS_INIT(profile_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
