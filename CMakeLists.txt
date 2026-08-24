# SPDX-License-Identifier: MIT

# The fabricated T430 Revision A board inverts its TrackPoint RESET GPIO with
# Q2. The pinned upstream driver discards devicetree GPIO polarity, so apply a
# small, tracked compatibility patch only when building that shield. This keeps
# fresh West/CI checkouts reproducible and leaves the T470 build untouched.
if("thinkpad_t430_reva" IN_LIST SHIELD_AS_LIST)
  set(trackpoint_driver_dir
    "${ZEPHYR_KB_ZMK_PS2_MOUSE_TRACKPOINT_DRIVER_MODULE_DIR}")
  set(trackpoint_reset_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-reset-gpio-polarity.patch")
  set(trackpoint_ps2_race_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-ps2-write-response-race.patch")
  set(trackpoint_stream_fifo_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-stream-fifo.patch")
  set(trackpoint_stream_mode_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-explicit-stream-mode.patch")
  set(trackpoint_verified_por_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-verified-por.patch")
  set(trackpoint_button_sync_patch
    "${CMAKE_CURRENT_LIST_DIR}/patches/trackpoint-button-report-sync.patch")

  execute_process(
    COMMAND git apply --check "${trackpoint_reset_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)

  if(trackpoint_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_reset_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_patch_result)
    if(NOT trackpoint_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the T430 Rev A reset-polarity patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_reset_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_patch_already_applied EQUAL 0)
      message(FATAL_ERROR
        "The TrackPoint driver no longer matches the pinned reset-polarity patch")
  endif()

  execute_process(
    COMMAND git apply --check "${trackpoint_verified_por_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_verified_por_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)
  if(trackpoint_verified_por_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_verified_por_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_verified_por_patch_result)
    if(NOT trackpoint_verified_por_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the verified TrackPoint POR patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_verified_por_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_verified_por_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_verified_por_patch_already_applied EQUAL 0)
      message(FATAL_ERROR "The TrackPoint driver no longer matches the verified POR patch")
    endif()
  endif()
endif()

  execute_process(
    COMMAND git apply --check "${trackpoint_ps2_race_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_ps2_race_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)
  if(trackpoint_ps2_race_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_ps2_race_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_ps2_race_patch_result)
    if(NOT trackpoint_ps2_race_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the TrackPoint PS/2 response-race patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_ps2_race_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_ps2_race_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_ps2_race_patch_already_applied EQUAL 0)
      message(FATAL_ERROR "The PS/2 driver no longer matches the pinned response-race patch")
    endif()
  endif()

  execute_process(
    COMMAND git apply --check "${trackpoint_stream_fifo_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_stream_fifo_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)
  if(trackpoint_stream_fifo_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_stream_fifo_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_stream_fifo_patch_result)
    if(NOT trackpoint_stream_fifo_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the TrackPoint stream FIFO patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_stream_fifo_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_stream_fifo_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_stream_fifo_patch_already_applied EQUAL 0)
      message(FATAL_ERROR "The TrackPoint driver no longer matches the pinned stream FIFO patch")
    endif()
  endif()

  execute_process(
    COMMAND git apply --check "${trackpoint_button_sync_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_button_sync_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)
  if(trackpoint_button_sync_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_button_sync_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_button_sync_patch_result)
    if(NOT trackpoint_button_sync_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the TrackPoint button report-sync patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_button_sync_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_button_sync_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_button_sync_patch_already_applied EQUAL 0)
      message(FATAL_ERROR "The TrackPoint driver no longer matches the button report-sync patch")
    endif()
  endif()

  execute_process(
    COMMAND git apply --check "${trackpoint_stream_mode_patch}"
    WORKING_DIRECTORY "${trackpoint_driver_dir}"
    RESULT_VARIABLE trackpoint_stream_mode_patch_can_apply
    OUTPUT_QUIET ERROR_QUIET)
  if(trackpoint_stream_mode_patch_can_apply EQUAL 0)
    execute_process(
      COMMAND git apply "${trackpoint_stream_mode_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_stream_mode_patch_result)
    if(NOT trackpoint_stream_mode_patch_result EQUAL 0)
      message(FATAL_ERROR "Failed to apply the explicit TrackPoint stream-mode patch")
    endif()
  else()
    execute_process(
      COMMAND git apply --reverse --check "${trackpoint_stream_mode_patch}"
      WORKING_DIRECTORY "${trackpoint_driver_dir}"
      RESULT_VARIABLE trackpoint_stream_mode_patch_already_applied
      OUTPUT_QUIET ERROR_QUIET)
    if(NOT trackpoint_stream_mode_patch_already_applied EQUAL 0)
      message(FATAL_ERROR "The TrackPoint driver no longer matches the pinned stream-mode patch")
    endif()
  endif()

endif()

zephyr_library()
zephyr_include_directories(include ${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(src/thinkpad_profile_control.c)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_T430_STATUS src/thinkpad_t430_status.c)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_T430_REVA_BACKLIGHT
  src/drivers/led/thinkpad_reva_backlight.c)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_T430_REVA_USB_BOOT_TRIGGER
  src/thinkpad_t430_reva_usb_boot_trigger.c)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_MATRIX_DISCOVERY
  src/thinkpad_matrix_discovery.c)
zephyr_library_sources_ifdef(CONFIG_THINKPAD_T470_STATUS src/thinkpad_t470_status.c)
