# SPDX-License-Identifier: MIT

zephyr_library()
zephyr_library_include_directories(${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_T430_STATUS src/thinkpad_t430_status.c)
