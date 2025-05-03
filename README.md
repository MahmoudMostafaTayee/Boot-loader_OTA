# Bootloader-OTA

A lightweight bootloader for the TM4C123GXL (Tiva C) microcontroller with support for over-the-air (OTA) firmware updates.

## Features

- ✅ Supports two application modes:
  - **OTA update mode** when the user button is pressed at startup
  - **Normal application mode** after a timeout or no button press
- ✅ Integrated with TI’s UniFlash or CCS flashing tools

## Hardware Used

- TM4C123GXL LaunchPad (EK-TM4C123GXL)

## How It Works

At startup:
1. Button state is polled.
2. If the user presses the button during the 3-second window:
   - Enters OTA update mode.
3. If no press is detected:
   - Proceeds to the normal application start.

## Usage

### Build

Build the bootloader and application binaries using Code Composer Studio (CCS).

### Flashing

Use [TI UniFlash](https://www.ti.com/tool/UNIFLASH) or CCS post-build hooks to flash both the bootloader and the main application.

