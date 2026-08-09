/*
 * ThinkPad T430 indicator support.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/hid_usage.h>

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

LOG_MODULE_REGISTER(thinkpad_t430_status, CONFIG_ZMK_LOG_LEVEL);

static const struct gpio_dt_spec power_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(t430_power_led), gpios);
static const struct gpio_dt_spec mute_led = GPIO_DT_SPEC_GET(DT_NODELABEL(t430_mute_led), gpios);
static const struct gpio_dt_spec micmute_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(t430_micmute_led), gpios);
static const struct gpio_dt_spec profile_leds[] = {
    GPIO_DT_SPEC_GET(DT_NODELABEL(t430_profile_1_led), gpios),
    GPIO_DT_SPEC_GET(DT_NODELABEL(t430_profile_2_led), gpios),
    GPIO_DT_SPEC_GET(DT_NODELABEL(t430_profile_3_led), gpios),
};

static struct k_work_delayable status_work;
static struct k_work_delayable profile_status_work;
static bool indicators_ready;
static bool normal_blink_on;
static bool profile_blink_on;
static int64_t profile_identify_until;
static int last_profile = -1;
static bool mute_on;
static bool micmute_on;

#define PROFILE_IDENTIFY_DURATION_MS 2500
#define PROFILE_PAIRING_BLINK K_MSEC(150)
#define PROFILE_DISCONNECTED_ON K_MSEC(120)
#define PROFILE_DISCONNECTED_OFF K_MSEC(1880)

static void set_led(const struct gpio_dt_spec *led, bool on) {
    int err = gpio_pin_set_dt(led, on ? 1 : 0);

    if (err) {
        LOG_ERR("Failed to set indicator %s pin %u: %d", led->port->name, led->pin, err);
    }
}

static void schedule_status(k_timeout_t delay) {
    if (indicators_ready) {
        k_work_reschedule(&status_work, delay);
    }
}

static void schedule_profile_status(k_timeout_t delay) {
    if (indicators_ready) {
        k_work_reschedule(&profile_status_work, delay);
    }
}

static void set_selected_profile_led(bool on) {
    int selected = zmk_ble_active_profile_index();

    for (size_t i = 0; i < ARRAY_SIZE(profile_leds); i++) {
        set_led(&profile_leds[i], on && selected == (int)i);
    }
}

static void show_normal_status(void) {
    struct zmk_endpoint_instance endpoint = zmk_endpoints_selected();
    bool next_on;
    k_timeout_t next_delay;

    if (endpoint.transport == ZMK_TRANSPORT_USB) {
        if (zmk_usb_is_hid_ready()) {
            set_led(&power_led, true);
            normal_blink_on = true;
            return;
        }

        /* USB selected but not enumerated: one short pulse per second. */
        next_on = !normal_blink_on;
        next_delay = next_on ? K_MSEC(120) : K_MSEC(880);
    } else if (zmk_ble_active_profile_is_connected()) {
        set_led(&power_led, true);
        normal_blink_on = true;
        return;
    } else if (zmk_ble_active_profile_is_open()) {
        /* An empty profile is advertising for a new host. */
        next_on = !normal_blink_on;
        next_delay = K_MSEC(150);
    } else {
        /* A bonded profile is disconnected and trying to reconnect. */
        next_on = !normal_blink_on;
        next_delay = next_on ? K_MSEC(200) : K_MSEC(800);
    }

    normal_blink_on = next_on;
    set_led(&power_led, next_on);
    schedule_status(next_delay);
}

static void status_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    show_normal_status();
}

static void profile_status_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    struct zmk_endpoint_instance endpoint = zmk_endpoints_selected();
    int64_t now = k_uptime_get();

    if (endpoint.transport == ZMK_TRANSPORT_USB) {
        if (now < profile_identify_until) {
            set_selected_profile_led(true);
            schedule_profile_status(K_MSEC(profile_identify_until - now));
        } else {
            set_selected_profile_led(false);
        }
        return;
    }

    if (zmk_ble_active_profile_is_connected()) {
        if (now < profile_identify_until) {
            set_selected_profile_led(true);
            schedule_profile_status(K_MSEC(profile_identify_until - now));
        } else {
            set_selected_profile_led(false);
        }
        return;
    }

    profile_blink_on = !profile_blink_on;
    set_selected_profile_led(profile_blink_on);

    if (zmk_ble_active_profile_is_open()) {
        schedule_profile_status(PROFILE_PAIRING_BLINK);
    } else {
        schedule_profile_status(profile_blink_on ? PROFILE_DISCONNECTED_ON
                                                 : PROFILE_DISCONNECTED_OFF);
    }
}

static void refresh_power_status(void) {
    if (!indicators_ready) {
        return;
    }

    normal_blink_on = false;
    schedule_status(K_NO_WAIT);
}

static void refresh_profile_status(bool identify_profile) {
    if (!indicators_ready) {
        return;
    }

    profile_blink_on = false;
    profile_identify_until =
        identify_profile ? k_uptime_get() + PROFILE_IDENTIFY_DURATION_MS : 0;
    schedule_profile_status(K_NO_WAIT);
}

static int t430_status_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *key_event = as_zmk_keycode_state_changed(eh);

    if (key_event != NULL) {
        if (!key_event->state || key_event->usage_page != HID_USAGE_CONSUMER) {
            return ZMK_EV_EVENT_BUBBLE;
        }

        if (key_event->keycode == HID_USAGE_CONSUMER_MUTE) {
            mute_on = !mute_on;
            set_led(&mute_led, mute_on);
        } else if (key_event->keycode ==
                   HID_USAGE_CONSUMER_START_OR_STOP_MICROPHONE_CAPTURE) {
            micmute_on = !micmute_on;
            set_led(&micmute_led, micmute_on);
        }

        return ZMK_EV_EVENT_BUBBLE;
    }

    if (as_zmk_ble_active_profile_changed(eh) != NULL) {
        int profile = zmk_ble_active_profile_index();
        bool profile_changed = profile != last_profile;

        last_profile = profile;
        refresh_power_status();
        refresh_profile_status(profile_changed ||
                               (zmk_endpoints_selected().transport == ZMK_TRANSPORT_BLE &&
                                zmk_ble_active_profile_is_connected()));
    } else if (as_zmk_endpoint_changed(eh) != NULL) {
        refresh_power_status();
        refresh_profile_status(zmk_endpoints_selected().transport == ZMK_TRANSPORT_BLE);
    } else if (as_zmk_usb_conn_state_changed(eh) != NULL) {
        refresh_power_status();
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(thinkpad_t430_status, t430_status_listener);
ZMK_SUBSCRIPTION(thinkpad_t430_status, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(thinkpad_t430_status, zmk_endpoint_changed);
ZMK_SUBSCRIPTION(thinkpad_t430_status, zmk_keycode_state_changed);
ZMK_SUBSCRIPTION(thinkpad_t430_status, zmk_usb_conn_state_changed);

static int thinkpad_t430_status_init(void) {
    const struct gpio_dt_spec *leds[] = {
        &power_led,       &mute_led,        &micmute_led,
        &profile_leds[0], &profile_leds[1], &profile_leds[2],
    };

    for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
        if (!gpio_is_ready_dt(leds[i])) {
            LOG_ERR("Indicator GPIO controller is not ready");
            return -ENODEV;
        }

        int err = gpio_pin_configure_dt(leds[i], GPIO_OUTPUT_INACTIVE);
        if (err) {
            LOG_ERR("Failed to configure indicator GPIO: %d", err);
            return err;
        }
    }

    k_work_init_delayable(&status_work, status_work_handler);
    k_work_init_delayable(&profile_status_work, profile_status_work_handler);
    last_profile = zmk_ble_active_profile_index();
    indicators_ready = true;
    refresh_power_status();
    refresh_profile_status(false);

    LOG_INF("T430 power, mute, mic-mute, and BLE profile indicators initialized");
    return 0;
}

SYS_INIT(thinkpad_t430_status_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
