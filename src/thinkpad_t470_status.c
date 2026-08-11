/*
 * ThinkPad T470 BLE profile indicators.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <thinkpad/profile_control.h>

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
static bool identifying_usb;
static bool ble_requested;
static int64_t identify_until;
static int identify_profile = -1;

#define PROFILE_IDENTIFY_DURATION_MS 5000
#define USB_IDENTIFY_DURATION_MS 1000
#define PROFILE_PAIRING_BLINK K_MSEC(150)
#define PROFILE_DISCONNECTED_ON K_MSEC(120)
#define PROFILE_DISCONNECTED_OFF K_MSEC(1880)

static void set_led(const struct gpio_dt_spec *led, bool on) {
    int err = gpio_pin_set_dt(led, on ? 1 : 0);

    if (err) {
        LOG_ERR("Failed to set profile LED pin %u: %d", led->pin, err);
    }
}

static void set_profile_led(int profile, bool on) {

    for (size_t i = 0; i < ARRAY_SIZE(profile_leds); i++) {
        set_led(&profile_leds[i], on && profile == (int)i);
    }
}

static void set_selected_led(bool on) { set_profile_led(zmk_ble_active_profile_index(), on); }

static void set_all_profile_leds(bool on) {
    for (size_t i = 0; i < ARRAY_SIZE(profile_leds); i++) {
        set_led(&profile_leds[i], on);
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

    if (!ble_requested) {
        if (identifying_usb && now < identify_until) {
            set_all_profile_leds(true);
            schedule_profile(K_MSEC(identify_until - now));
        } else if (!identifying_usb && now < identify_until) {
            set_profile_led(identify_profile, true);
            schedule_profile(K_MSEC(identify_until - now));
        } else {
            set_all_profile_leds(false);
        }
        return;
    }

    if (now < identify_until) {
        set_profile_led(identify_profile, true);
        schedule_profile(K_MSEC(identify_until - now));
        return;
    }

    if (zmk_ble_active_profile_is_connected()) {
        set_profile_led(identify_profile, false);
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
    identifying_usb = false;
    identify_until = identify ? k_uptime_get() + PROFILE_IDENTIFY_DURATION_MS : 0;
    identify_profile = identify ? zmk_ble_active_profile_index() : -1;
    schedule_profile(K_NO_WAIT);
}

static void identify_usb(void) {
    if (!ready) {
        return;
    }

    blink_on = false;
    identifying_usb = true;
    ble_requested = false;
    identify_profile = -1;
    identify_until = k_uptime_get() + USB_IDENTIFY_DURATION_MS;
    schedule_profile(K_NO_WAIT);
}

static void profile_command_listener(enum thinkpad_profile_command command,
                                     const struct thinkpad_profile_state *state) {
    ble_requested = state->ble_requested;

    if (command == THINKPAD_PROFILE_COMMAND_USB) {
        identify_usb();
        return;
    }

    blink_on = false;
    identifying_usb = false;
    identify_profile = state->profile;

    if (command == THINKPAD_PROFILE_COMMAND_SELECT &&
        !zmk_ble_profile_is_open(state->profile)) {
        identify_until = k_uptime_get() + PROFILE_IDENTIFY_DURATION_MS;
    } else {
        identify_until = 0;
    }

    schedule_profile(K_NO_WAIT);
}

static int profile_listener(const zmk_event_t *eh) {
    if (as_zmk_ble_active_profile_changed(eh) != NULL) {
        refresh_profile(false);
    } else if (as_zmk_endpoint_changed(eh) != NULL) {
        if (zmk_endpoints_selected().transport == ZMK_TRANSPORT_BLE) {
            refresh_profile(true);
        }
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
    ready = true;
    thinkpad_profile_register_listener(profile_command_listener);

    return 0;
}

SYS_INIT(profile_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
