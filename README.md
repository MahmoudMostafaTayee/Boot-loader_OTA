# 🔐 Bootloader with OTA Support

A lightweight, fail-safe bootloader for the **TM4C123GXL (Tiva C)** microcontroller supporting **Over-The-Air (OTA)** firmware updates using dual application slots.

---

## 🚀 Features

- ✅ **Dual Application Modes**:
  - **OTA Mode**: Triggered when the user button is held during startup.
  - **Normal Mode**: Automatically jumps to the active application after a timeout or if no button is pressed.
- ✅ **Dual-Slot Firmware Architecture**:
  - Stores both the current and new firmware images.
  - Enables rollback in case of corrupted or invalid updates.
- ✅ **Compatible with TI Tools**:
  - Works with **TI UniFlash** and **Code Composer Studio (CCS)** for flashing and debugging.

---

## 🧰 Hardware

- **Microcontroller**: TM4C123GH6PM (ARM Cortex-M4)
- **Board**: TM4C123GXL LaunchPad (EK-TM4C123GXL)

---

## 🧠 Memory Map Design

The TM4C123GH6PM features **256 KB of on-chip Flash memory**. Memory layout is partitioned for bootloader and two application slots:

### 📌 Flash Layout

| Section     | Start Address | Size     | Description                  |
| ----------- | ------------- | -------- | ---------------------------- |
| Bootloader  | `0x00000000`  | `16 KB`  | Secure boot and OTA logic    |
| App Slot #1 | `0x00004000`  | `120 KB` | Currently active application |
| App Slot #2 | `0x00022000`  | `120 KB` | New OTA update candidate     |

> ℹ️ Each application slot is **120 KB** (`0x1E000`) = `(256KB - 16KB) / 2`

---

## 🔄 Boot Process

1. On startup, the bootloader polls the user button for ~3 seconds.
2. If **button is pressed**:
   - Enters **OTA update mode**, allowing new firmware upload.
3. If **button is not pressed**:
   - Proceeds to the **currently active application** in App Slot #1.
4. After successful OTA update:
   - Verifies integrity (e.g., CRC/checksum or digital signature).
   - Marks new firmware as active (optionally stored in metadata region).
5. If verification fails or firmware is invalid:
   - Falls back to the last known good application.

---

## 🛠️ Building & Usage

### 🔧 Build

- Compile both the bootloader and application using **Code Composer Studio (CCS)**.
- Ensure correct linker scripts are used for:
  - Bootloader (`0x00000000` – `0x00003FFF`)
  - Application 1 (`0x00004000`)
  - Application 2 (`0x00022000`)

### ⚡ Flashing

Use [**TI UniFlash**](https://www.ti.com/tool/UNIFLASH) or CCS post-build scripts to:

- Flash the bootloader once.
- Flash the active application to the appropriate slot.
- Flash the update image to the second slot during OTA simulation (or real transfer via UART/USB/BLE).
