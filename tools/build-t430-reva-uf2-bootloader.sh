#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
source_dir="$repo_root/.zmk/adafruit-nrf52-bootloader"
output_dir="$repo_root/.zmk/build/thinkpad_t430_reva_bootloader"
revision=c67f0bcf0fa8e841426335b1bbde91cda6ca1f50
adafruit_toolchain="$HOME/Library/Arduino15/packages/adafruit/tools/arm-none-eabi-gcc/9-2019q4/bin/arm-none-eabi-"

if [ -n "${CROSS_COMPILE:-}" ]; then
    cross_compile=$CROSS_COMPILE
elif [ -x "${adafruit_toolchain}gcc" ]; then
    cross_compile=$adafruit_toolchain
else
    cross_compile="$(brew --prefix)/bin/arm-none-eabi-"
fi

if [ ! -d "$source_dir/.git" ]; then
    git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git \
        "$source_dir"
fi

git -C "$source_dir" fetch origin "$revision"
git -C "$source_dir" checkout --detach "$revision"
git -C "$source_dir" submodule update --init lib/nrfx lib/tinycrypt lib/tinyusb lib/uf2
git -C "$source_dir" apply "$repo_root/patches/adafruit-nrf52-bootloader-uf2-timeout.patch"

board_dir="$source_dir/src/boards/thinkpad_t430_reva"
mkdir -p "$board_dir" "$output_dir"
cp "$repo_root"/bootloader/thinkpad_t430_reva/* "$board_dir"/

make -C "$source_dir" clean BOARD=thinkpad_t430_reva GIT_VERSION=0.9.2 _VER3="0 9 2"
make -C "$source_dir" BOARD=thinkpad_t430_reva \
    GIT_VERSION=0.9.2 \
    _VER3="0 9 2" \
    CROSS_COMPILE="$cross_compile" \
    _build/build-thinkpad_t430_reva/thinkpad_t430_reva_bootloader-0.9.2_nosd.hex

cp "$source_dir/_build/build-thinkpad_t430_reva/thinkpad_t430_reva_bootloader-"*_nosd.hex \
    "$output_dir/thinkpad_t430_reva_bootloader_nosd.hex"

printf '%s\n' "$output_dir/thinkpad_t430_reva_bootloader_nosd.hex"
