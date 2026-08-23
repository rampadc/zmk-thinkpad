/* SPDX-License-Identifier: MIT */

#ifndef THINKPAD_T430_REVA_BOOTLOADER_H
#define THINKPAD_T430_REVA_BOOTLOADER_H

/* Do not drive keyboard or indicator GPIOs from the bootloader. */
#define LEDS_NUMBER 0
#define LED_STATE_ON 0

/* P0.18 is hardware reset; P0.26 is unconnected on Rev A. */
#define BUTTON_DFU PINNUM(0, 18)
#define BUTTON_DFU_OTA PINNUM(0, 26)
#define BUTTON_PULL NRF_GPIO_PIN_PULLUP

#define BLEDIS_MANUFACTURER "ThinkPad ZMK"
#define BLEDIS_MODEL "T430 Rev A"

/* Development VID/PID; obtain a registered pair before product distribution. */
#define USB_DESC_VID 0x1209
#define USB_DESC_UF2_PID 0x0430
#define USB_DESC_CDC_ONLY_PID 0x0431

#define UF2_PRODUCT_NAME "ThinkPad T430 Rev A"
#define UF2_VOLUME_LABEL "T430BOOT"
#define UF2_BOARD_ID "nRF52840-ThinkPad-T430-RevA"
#define UF2_INDEX_URL "https://github.com/zmkfirmware/zmk"

#endif
