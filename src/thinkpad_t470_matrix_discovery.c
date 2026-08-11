/*
 * ThinkPad T470 raw matrix discovery logger.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

LOG_MODULE_REGISTER(thinkpad_t470_matrix, LOG_LEVEL_INF);

#define MATRIX_COLUMNS 8U
#define MATRIX_POSITIONS (16U * MATRIX_COLUMNS)
#define HOTKEY_POSITION MATRIX_POSITIONS

static const char *const key_names[] = {
    "ESC",       "F1",        "F2",       "F3",        "F4",       "F5",
    "F6",        "F7",        "F8",       "F9",        "F10",      "F11",
    "F12",       "HOME",      "END",      "INSERT",    "DELETE",

    "GRAVE",     "1",         "2",        "3",         "4",        "5",
    "6",         "7",         "8",        "9",         "0",        "MINUS",
    "EQUAL",     "BACKSPACE",

    "TAB",       "Q",         "W",        "E",         "R",        "T",
    "Y",         "U",         "I",        "O",         "P",        "LEFT_BRACKET",
    "RIGHT_BRACKET", "BACKSLASH",

    "CAPSLOCK",  "A",         "S",        "D",         "F",        "G",
    "H",         "J",         "K",        "L",         "SEMICOLON", "APOSTROPHE",
    "ENTER",

    "LEFT_SHIFT", "Z",        "X",        "C",         "V",        "B",
    "N",          "M",        "COMMA",    "PERIOD",    "SLASH",    "RIGHT_SHIFT",

    "FN",         "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT",  "SPACE",    "RIGHT_ALT",
    "PRINT_SCREEN", "RIGHT_CTRL", "PAGE_UP", "UP",      "PAGE_DOWN", "LEFT",
    "DOWN",       "RIGHT",
};

#define KEY_COUNT ARRAY_SIZE(key_names)

static uint16_t key_positions[KEY_COUNT];
static size_t key_index;
static bool waiting_for_release;
static uint16_t pressed_position;

static void log_coordinate(const char *prefix, const char *key, uint16_t position) {
    if (position < MATRIX_POSITIONS) {
        LOG_INF("%s %s = DRV%u SENSE%u position=%u", prefix, key,
                position / MATRIX_COLUMNS, position % MATRIX_COLUMNS, position);
    } else {
        LOG_INF("%s %s = HOTKEY position=%u", prefix, key, position);
    }
}

static void print_prompt(void) {
    if (key_index < KEY_COUNT) {
        LOG_INF("T470_GUIDE [%u/%u] PRESS %s", key_index + 1U, KEY_COUNT,
                key_names[key_index]);
        return;
    }

    LOG_INF("T470_GUIDE COMPLETE - mapping follows");
    LOG_INF("T470_MAP_BEGIN");
    for (size_t i = 0; i < KEY_COUNT; i++) {
        log_coordinate("T470_MAP", key_names[i], key_positions[i]);
    }
    LOG_INF("T470_MAP_END");
}

static void prompt_work_handler(struct k_work *work) { print_prompt(); }

K_WORK_DELAYABLE_DEFINE(prompt_work, prompt_work_handler);

static int matrix_discovery_init(void) {
    k_work_schedule(&prompt_work, K_SECONDS(2));
    return 0;
}

SYS_INIT(matrix_discovery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int matrix_discovery_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event = as_zmk_position_state_changed(eh);

    if (event == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (event->position > HOTKEY_POSITION) {
        LOG_WRN("T470_SCAN %s unexpected position=%u", event->state ? "PRESS" : "RELEASE",
                event->position);
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (key_index >= KEY_COUNT) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (event->state) {
        if (waiting_for_release) {
            return ZMK_EV_EVENT_BUBBLE;
        }

        for (size_t i = 0; i < key_index; i++) {
            if (key_positions[i] == event->position) {
                log_coordinate("T470_GUIDE DUPLICATE", key_names[i], event->position);
                LOG_WRN("T470_GUIDE expected %s; duplicate ignored", key_names[key_index]);
                return ZMK_EV_EVENT_BUBBLE;
            }
        }

        key_positions[key_index] = event->position;
        pressed_position = event->position;
        waiting_for_release = true;
        log_coordinate("T470_GUIDE CAPTURED", key_names[key_index], event->position);
    } else if (waiting_for_release && event->position == pressed_position) {
        waiting_for_release = false;
        key_index++;
        k_work_reschedule(&prompt_work, K_MSEC(150));
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(thinkpad_t470_matrix, matrix_discovery_listener);
ZMK_SUBSCRIPTION(thinkpad_t470_matrix, zmk_position_state_changed);
