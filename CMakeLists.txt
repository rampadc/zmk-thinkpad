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
