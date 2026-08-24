/* SPDX-License-Identifier: MIT */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/usb/usb_device.h>

#define ROW_COUNT 16
#define COL_COUNT 8
#define DIRECT_COUNT 2
#define DEBOUNCE_SCANS 4
#define UF2_REBOOT_MAGIC 0x57

#define GPIO_SPEC(node_id) GPIO_DT_SPEC_GET(node_id, gpios),
static const struct gpio_dt_spec rows[] = {
    DT_FOREACH_CHILD(DT_NODELABEL(factory_rows), GPIO_SPEC)
};
static const struct gpio_dt_spec cols[] = {
    DT_FOREACH_CHILD(DT_NODELABEL(factory_cols), GPIO_SPEC)
};
static const struct gpio_dt_spec direct[] = {
    DT_FOREACH_CHILD(DT_NODELABEL(factory_direct), GPIO_SPEC)
};

static bool stable[ROW_COUNT * COL_COUNT + DIRECT_COUNT];
static bool candidate[ARRAY_SIZE(stable)];
static uint8_t counts[ARRAY_SIZE(stable)];

static void print_input(size_t index, bool pressed) {
    if (index < ROW_COUNT * COL_COUNT) {
        printk("RAW %s DRV%u SENSE%u\r\n", pressed ? "PRESS" : "RELEASE",
               (unsigned)(index / COL_COUNT), (unsigned)(index % COL_COUNT));
    } else {
        const char *name = index == ROW_COUNT * COL_COUNT ? "FN P1.11" : "POWER P0.22";
        printk("RAW %s DIRECT %s\r\n", pressed ? "PRESS" : "RELEASE", name);
    }
}

static void report_macros(void) {
    static uint8_t reported;
    const bool fn = stable[ROW_COUNT * COL_COUNT];
    const size_t number_positions[] = {0 * COL_COUNT + 1, 1 * COL_COUNT + 1,
                                       2 * COL_COUNT + 1, 3 * COL_COUNT + 1};
    uint8_t now = 0;

    if (fn) {
        for (size_t i = 0; i < ARRAY_SIZE(number_positions); i++) {
            if (stable[number_positions[i]]) {
                now |= BIT(i);
            }
        }
    }

    for (size_t i = 0; i < 4; i++) {
        if ((now & BIT(i)) && !(reported & BIT(i))) {
            printk("MACRO PASS FN+%u (FN P1.11 + DRV%u/SENSE1)\r\n",
                   (unsigned)i + 1U, (unsigned)i);
        }
    }
    reported = now;
}

static void debounce(size_t index, bool value) {
    if (value != candidate[index]) {
        candidate[index] = value;
        counts[index] = 1;
        return;
    }
    if (counts[index] < DEBOUNCE_SCANS) {
        counts[index]++;
    }
    if (counts[index] == DEBOUNCE_SCANS && stable[index] != value) {
        stable[index] = value;
        print_input(index, value);
        report_macros();
    }
}

int main(void) {
    const struct device *cdc = DEVICE_DT_GET(DT_NODELABEL(cdc_acm_uart0));

    BUILD_ASSERT(ARRAY_SIZE(rows) == ROW_COUNT);
    BUILD_ASSERT(ARRAY_SIZE(cols) == COL_COUNT);
    BUILD_ASSERT(ARRAY_SIZE(direct) == DIRECT_COUNT);

    for (size_t i = 0; i < ARRAY_SIZE(rows); i++) {
        gpio_pin_configure_dt(&rows[i], GPIO_OUTPUT_INACTIVE);
    }
    for (size_t i = 0; i < ARRAY_SIZE(cols); i++) {
        gpio_pin_configure_dt(&cols[i], GPIO_INPUT);
    }
    for (size_t i = 0; i < ARRAY_SIZE(direct); i++) {
        gpio_pin_configure_dt(&direct[i], GPIO_INPUT);
    }

    usb_enable(NULL);
    while (!device_is_ready(cdc)) {
        k_sleep(K_MSEC(10));
    }

    printk("T430 REV A RAW FACTORY TEST\r\n");
    printk("No HID or BLE is enabled. Press keys to print raw wiring.\r\n");
    printk("Fn+1/2/3/4 prints MACRO PASS when both contacts are held.\r\n");

    while (true) {
        uint32_t baud_rate;

        if (uart_line_ctrl_get(cdc, UART_LINE_CTRL_BAUD_RATE, &baud_rate) == 0 &&
            baud_rate == 1200U) {
            k_sleep(K_MSEC(100));
            sys_reboot(UF2_REBOOT_MAGIC);
        }

        for (size_t row = 0; row < ARRAY_SIZE(rows); row++) {
            gpio_pin_set_dt(&rows[row], 1);
            k_busy_wait(30);
            for (size_t col = 0; col < ARRAY_SIZE(cols); col++) {
                debounce(row * COL_COUNT + col, gpio_pin_get_dt(&cols[col]) > 0);
            }
            gpio_pin_set_dt(&rows[row], 0);
        }
        for (size_t i = 0; i < ARRAY_SIZE(direct); i++) {
            debounce(ROW_COUNT * COL_COUNT + i, gpio_pin_get_dt(&direct[i]) > 0);
        }
        k_sleep(K_MSEC(1));
    }
    return 0;
}
