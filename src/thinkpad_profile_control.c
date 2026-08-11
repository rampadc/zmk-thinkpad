/* Shared ThinkPad USB/BLE profile command controller. SPDX-License-Identifier: MIT */

#define DT_DRV_COMPAT zmk_behavior_thinkpad_profile_command

#include <errno.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <drivers/behavior.h>
#include <dt-bindings/thinkpad/profile.h>
#include <thinkpad/profile_control.h>
#include <zmk/behavior.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

LOG_MODULE_REGISTER(thinkpad_profile_control, CONFIG_ZMK_LOG_LEVEL);

/* ZMK currently keeps these internal to ble.c. The controller needs to restart
 * advertising after intentionally cycling the Bluetooth host stack.
 */
extern int advertising_status;
extern int update_advertising(void);

static struct thinkpad_profile_state requested = {.ble_requested = true, .profile = 0};
static thinkpad_profile_listener_t status_listener;

static int suspend_ble_radio(void) {
    if (!bt_is_ready()) {
        advertising_status = 0;
        return 0;
    }

    int err = bt_disable();
    if (err && err != -EALREADY) {
        LOG_ERR("Failed to suspend Bluetooth: %d", err);
        return err;
    }

    advertising_status = 0;
    LOG_INF("Bluetooth suspended for exclusive USB mode");
    return 0;
}

static int resume_ble_radio(void) {
    if (bt_is_ready()) {
        return 0;
    }

    int err = bt_enable(NULL);
    if (err && err != -EALREADY) {
        LOG_ERR("Failed to resume Bluetooth: %d", err);
        return err;
    }

    advertising_status = 0;
    LOG_INF("Bluetooth resumed for BLE mode");
    return 0;
}

static void restore_radio_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!requested.ble_requested) {
        suspend_ble_radio();
    }
}

K_WORK_DELAYABLE_DEFINE(restore_radio_work, restore_radio_work_handler);

static void notify(enum thinkpad_profile_command command) {
    if (status_listener != NULL) {
        status_listener(command, &requested);
    }
}

static void save_requested(void) {
#if IS_ENABLED(CONFIG_SETTINGS)
    settings_save_one("thinkpad/profile", &requested, sizeof(requested));
#endif
}

struct thinkpad_profile_state thinkpad_profile_get_state(void) { return requested; }

void thinkpad_profile_register_listener(thinkpad_profile_listener_t listener) {
    status_listener = listener;
    notify(THINKPAD_PROFILE_COMMAND_RESTORE);
}

#if IS_ENABLED(CONFIG_SETTINGS)
static int profile_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                                void *cb_arg) {
    if (name[0] != '\0' || len != sizeof(requested)) {
        return -EINVAL;
    }

    int err = read_cb(cb_arg, &requested, sizeof(requested));
    if (err < 0) {
        return err;
    }

    if (requested.profile >= ZMK_BLE_PROFILE_COUNT) {
        requested.profile = 0;
    }

    notify(THINKPAD_PROFILE_COMMAND_RESTORE);
    k_work_reschedule(&restore_radio_work, K_MSEC(50));

    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(thinkpad_profile, "thinkpad/profile", NULL, profile_settings_set,
                               NULL, NULL);
#endif

static int select_ble_profile(uint8_t profile) {
    if (profile >= ZMK_BLE_PROFILE_COUNT) {
        return -EINVAL;
    }

    int err = resume_ble_radio();
    if (err) {
        return err;
    }

    err = zmk_endpoints_select_transport(ZMK_TRANSPORT_BLE);
    if (err) {
        return err;
    }

    err = zmk_ble_prof_select(profile);
    if (err) {
        return err;
    }

    err = update_advertising();
    if (err) {
        return err;
    }

    requested.ble_requested = true;
    requested.profile = profile;
    return 0;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    ARG_UNUSED(event);
    int err;

    switch (binding->param1) {
    case THINKPAD_PROFILE_SELECT:
        err = select_ble_profile(binding->param2);
        if (err) {
            return err;
        }
        save_requested();
        notify(THINKPAD_PROFILE_COMMAND_SELECT);
        return 0;
    case THINKPAD_PROFILE_PAIR:
        err = select_ble_profile(binding->param2);
        if (err) {
            return err;
        }
        zmk_ble_clear_bonds();
        save_requested();
        notify(THINKPAD_PROFILE_COMMAND_PAIR);
        return 0;
    case THINKPAD_PROFILE_USB:
        err = zmk_endpoints_select_transport(ZMK_TRANSPORT_USB);
        if (err) {
            return err;
        }
        requested.ble_requested = false;
        save_requested();
        notify(THINKPAD_PROFILE_COMMAND_USB);
        return suspend_ble_radio();
    default:
        return -ENOTSUP;
    }
}

static const struct behavior_driver_api profile_command_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &profile_command_driver_api);
