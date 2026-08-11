/*
 * ThinkPad T470 raw matrix discovery logger.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

LOG_MODULE_REGISTER(thinkpad_t470_matrix, LOG_LEVEL_INF);

#define MATRIX_COLUMNS 8U
#define MATRIX_POSITIONS (16U * MATRIX_COLUMNS)
#define HOTKEY_POSITION MATRIX_POSITIONS

static int matrix_discovery_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event = as_zmk_position_state_changed(eh);

    if (event == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (event->position < MATRIX_POSITIONS) {
        const uint32_t drv = event->position / MATRIX_COLUMNS;
        const uint32_t sense = event->position % MATRIX_COLUMNS;

        LOG_INF("T470_SCAN %s DRV%u SENSE%u position=%u",
                event->state ? "PRESS" : "RELEASE", drv, sense, event->position);
    } else if (event->position == HOTKEY_POSITION) {
        LOG_INF("T470_SCAN %s HOTKEY position=%u", event->state ? "PRESS" : "RELEASE",
                event->position);
    } else {
        LOG_WRN("T470_SCAN %s unexpected position=%u", event->state ? "PRESS" : "RELEASE",
                event->position);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(thinkpad_t470_matrix, matrix_discovery_listener);
ZMK_SUBSCRIPTION(thinkpad_t470_matrix, zmk_position_state_changed);

