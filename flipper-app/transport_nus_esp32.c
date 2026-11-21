/*
 * ESP32 port (Sor3nt Flipper-Zero-ESP32-Port) replacement for transport_nus.c.
 *
 * The real Desktop (NUS) transport registers a custom GATT service on the
 * Flipper's STM32WB BLE stack (furi_ble/gatt.h, event_dispatcher.h), which
 * the ESP32 port does not provide. Desktop mode therefore falls back to the
 * BT serial transport used by the Claude Code bridge.
 */
#include "transport.h"

#include <bt/bt_service/bt.h>
#include <furi.h>

Transport* transport_nus_alloc(void) {
    FURI_LOG_W("ClaudeBuddy", "Desktop (NUS) mode unsupported on ESP32 port, using BT serial");
    return transport_bt_alloc();
}

bool transport_nus_is_secure(void) {
    return false;
}

void transport_nus_forget_bonds(void) {
    Bt* bt = furi_record_open(RECORD_BT);
    bt_forget_bonded_devices(bt);
    furi_record_close(RECORD_BT);
}
