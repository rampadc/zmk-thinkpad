# SPDX-License-Identifier: MIT

board_runner_args(jlink "--device=nrf52840_xxAA" "--speed=1000")
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)
