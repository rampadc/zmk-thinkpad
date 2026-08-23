/*
 * Host-triggered entry into the T430 Rev A UF2 bootloader.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>

#define UF2_REBOOT_MAGIC 0x57
#define TRIGGER_BAUD_RATE 1200U
#define POLL_INTERVAL_MS 50U

static const struct device *const cdc = DEVICE_DT_GET(DT_NODELABEL(cdc_acm_uart0));
static struct k_work_delayable poll_work;
static bool reboot_requested;

static void poll_line_coding(struct k_work *work) {
    uint32_t baud_rate;

    ARG_UNUSED(work);

    if (!reboot_requested && device_is_ready(cdc) &&
        uart_line_ctrl_get(cdc, UART_LINE_CTRL_BAUD_RATE, &baud_rate) == 0 &&
        baud_rate == TRIGGER_BAUD_RATE) {
        reboot_requested = true;
        k_msleep(100);
        sys_reboot(UF2_REBOOT_MAGIC);
    }

    k_work_reschedule(&poll_work, K_MSEC(POLL_INTERVAL_MS));
}

static int usb_boot_trigger_init(void) {
    k_work_init_delayable(&poll_work, poll_line_coding);
    k_work_schedule(&poll_work, K_MSEC(POLL_INTERVAL_MS));
    return 0;
}

SYS_INIT(usb_boot_trigger_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
