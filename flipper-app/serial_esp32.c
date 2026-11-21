/*
 * ESP32 port replacement for serial.c (USB CDC transport).
 *
 * The flashed port firmware (2.0.0) does not export furi_hal_cdc_* or
 * usb_cdc_single to FAPs, so the USB transport cannot load. detect_usb_cable()
 * always reads false on the port (no charge state), so the app only ever
 * selects the BLE transport; this stub exists to satisfy the linker.
 */
#include "transport.h"

#include <furi.h>

Transport* transport_usb_alloc(void) {
    FURI_LOG_W("ClaudeBuddy", "USB transport unavailable on ESP32 port");
    return NULL;
}
