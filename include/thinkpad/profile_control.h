/* SPDX-License-Identifier: MIT */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum thinkpad_profile_command {
    THINKPAD_PROFILE_COMMAND_RESTORE,
    THINKPAD_PROFILE_COMMAND_SELECT,
    THINKPAD_PROFILE_COMMAND_PAIR,
    THINKPAD_PROFILE_COMMAND_USB,
};

struct thinkpad_profile_state {
    bool ble_requested;
    uint8_t profile;
};

typedef void (*thinkpad_profile_listener_t)(enum thinkpad_profile_command command,
                                            const struct thinkpad_profile_state *state);

void thinkpad_profile_register_listener(thinkpad_profile_listener_t listener);
struct thinkpad_profile_state thinkpad_profile_get_state(void);
