/*
 * Guided ThinkPad keyboard matrix validation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

LOG_MODULE_REGISTER(thinkpad_matrix_discovery, LOG_LEVEL_INF);

#define MATRIX_COLUMNS 8U
#define MATRIX_POSITIONS (16U * MATRIX_COLUMNS)

#if IS_ENABLED(CONFIG_THINKPAD_MATRIX_DISCOVERY_T430)
#define LAYOUT_NAME "T430"
static const char *const key_names[] = {
    "POWER", "SPEAKER_MUTE", "VOLUME_DOWN", "VOLUME_UP", "MIC_MUTE", "THINKVANTAGE",
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11",
    "F12", "INSERT", "DELETE", "HOME", "END", "PAGE_UP", "PAGE_DOWN",
    "GRAVE", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "MINUS", "EQUAL",
    "BACKSPACE", "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "LEFT_BRACKET", "RIGHT_BRACKET", "BACKSLASH", "CAPSLOCK", "A", "S", "D", "F", "G",
    "H", "J", "K", "L", "SEMICOLON", "APOSTROPHE", "ENTER", "LEFT_SHIFT", "Z", "X",
    "C", "V", "B", "N", "M", "COMMA", "PERIOD", "SLASH", "RIGHT_SHIFT", "FN",
    "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "SPACE", "RIGHT_ALT", "PRINT_SCREEN", "RIGHT_CTRL",
    "LEFT", "UP", "DOWN", "RIGHT",
};
#elif IS_ENABLED(CONFIG_THINKPAD_MATRIX_DISCOVERY_T470)
#define LAYOUT_NAME "T470"
static const char *const key_names[] = {
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11",
    "F12", "HOME", "END", "INSERT", "DELETE", "GRAVE", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "MINUS", "EQUAL", "BACKSPACE", "TAB", "Q", "W", "E", "R",
    "T", "Y", "U", "I", "O", "P", "LEFT_BRACKET", "RIGHT_BRACKET", "BACKSLASH", "CAPSLOCK",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", "SEMICOLON", "APOSTROPHE", "ENTER",
    "LEFT_SHIFT", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "PERIOD", "SLASH",
    "RIGHT_SHIFT", "FN", "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "SPACE", "RIGHT_ALT",
    "PRINT_SCREEN", "RIGHT_CTRL", "PAGE_UP", "UP", "PAGE_DOWN", "LEFT", "DOWN", "RIGHT",
};
#else
#error "Matrix discovery requires a supported ThinkPad shield"
#endif

static uint16_t key_positions[ARRAY_SIZE(key_names)];
static size_t key_index;

static void log_position(const char *prefix, const char *key, uint16_t position) {
    LOG_INF("%s %s = transformed position %u", prefix, key, position);
}

static void print_prompt(void) {
    if (key_index < ARRAY_SIZE(key_names)) {
        LOG_INF("%s_GUIDE [%u/%u] PRESS %s", LAYOUT_NAME, key_index + 1U,
                ARRAY_SIZE(key_names), key_names[key_index]);
        return;
    }

    LOG_INF("%s_GUIDE COMPLETE", LAYOUT_NAME);
    for (size_t i = 0; i < ARRAY_SIZE(key_names); i++) {
        log_position("MAP", key_names[i], key_positions[i]);
    }
}

static void prompt_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(prompt_work, prompt_work_handler);

static void prompt_work_handler(struct k_work *work) {
    print_prompt();
    if (key_index < ARRAY_SIZE(key_names)) {
        k_work_reschedule(&prompt_work, K_SECONDS(2));
    }
}

static int matrix_discovery_init(void) {
    k_work_schedule(&prompt_work, K_SECONDS(2));
    return 0;
}
SYS_INIT(matrix_discovery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int matrix_discovery_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event = as_zmk_position_state_changed(eh);
    if (event == NULL || key_index >= ARRAY_SIZE(key_names)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (event->state) {
        for (size_t i = 0; i < key_index; i++) {
            if (key_positions[i] == event->position) {
                log_position("DUPLICATE", key_names[i], event->position);
                LOG_WRN("Expected %s; duplicate ignored", key_names[key_index]);
                return ZMK_EV_EVENT_BUBBLE;
            }
        }
        key_positions[key_index] = event->position;
        log_position("CAPTURED", key_names[key_index], event->position);
        key_index++;
        k_work_reschedule(&prompt_work, K_MSEC(150));
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(thinkpad_matrix_discovery, matrix_discovery_listener);
ZMK_SUBSCRIPTION(thinkpad_matrix_discovery, zmk_position_state_changed);
