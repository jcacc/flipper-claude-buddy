/*
 * ESP32 port (Sor3nt Flipper-Zero-ESP32-Port) compatibility definitions.
 *
 * The port's FAP API table does not export these firmware symbols, and a FAP
 * with unresolved imports refuses to load. Defining them inside the app makes
 * the linker resolve them locally instead.
 */
#include <bt/bt_service/bt.h>
#include <furi.h>
#include <furi_hal_power.h>
#include <furi_hal_rtc.h>
#include <furi_hal_usb.h>
#include <furi_hal_version.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>

/* ── notification sequences (copied from the port's notification_messages*.c) ── */

const NotificationMessage message_green_0 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0x00,
};
const NotificationMessage message_blue_0 = {
    .type = NotificationMessageTypeLedBlue,
    .data.led.value = 0x00,
};
const NotificationMessage message_blink_set_color_red = {
    .type = NotificationMessageTypeLedBlinkColor,
    .data.led_blink.color = LightRed,
};
const NotificationMessage message_blink_set_color_cyan = {
    .type = NotificationMessageTypeLedBlinkColor,
    .data.led_blink.color = LightBlue | LightGreen,
};
const NotificationMessage message_blink_set_color_magenta = {
    .type = NotificationMessageTypeLedBlinkColor,
    .data.led_blink.color = LightBlue | LightRed,
};
const NotificationMessage message_delay_100 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 100,
};
const NotificationMessage message_note_g4 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 392.0f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_e5 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 659.26f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_g5 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 783.99f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_a5 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 880.0f,
    .data.sound.volume = 1.0f,
};

/* ── power: derived from the exported fuel-gauge capacity calls ── */

uint8_t furi_hal_power_get_pct(void) {
    uint32_t full = furi_hal_power_get_battery_full_capacity();
    uint32_t remaining = furi_hal_power_get_battery_remaining_capacity();
    if(full == 0) return 100;
    uint32_t pct = remaining * 100 / full;
    return pct > 100 ? 100 : (uint8_t)pct;
}

bool furi_hal_power_is_charging(void) {
    return false; /* not exposed to FAPs on the port */
}

bool furi_hal_power_is_charging_done(void) {
    return false;
}

float furi_hal_power_get_battery_voltage(FuriHalPowerIC ic) {
    UNUSED(ic);
    return 0.0f;
}

float furi_hal_power_get_battery_current(FuriHalPowerIC ic) {
    UNUSED(ic);
    return 0.0f;
}

/* ── misc ── */

/* Host time sync: the port gives FAPs no RTC setter, so the clock is left alone. */
void furi_hal_rtc_set_datetime(DateTime* datetime) {
    UNUSED(datetime);
}

/* serial.c power-cycles USB before restoring the previous config; on the port
 * furi_hal_usb_set_config alone performs the switch. */
void furi_hal_usb_disable(void) {
}

void furi_hal_usb_enable(void) {
}

/* NULL makes the hello carry an empty "bt" name. The host bridge caches that
 * name and scans for it next session; a made-up one would never match the
 * advertised "Flipper <name>", while an empty one keeps the default
 * 0x3082-UUID / "Flipper"-prefix discovery. */
const char* furi_hal_version_get_name_ptr(void) {
    return NULL;
}

/* Not exported to FAPs; bonds can be cleared from the firmware's BT settings. */
void bt_forget_bonded_devices(Bt* bt) {
    UNUSED(bt);
}

/* ── not exported by the flashed port firmware (2.0.0) ── */

/* Only nus_charpack.c uses this, and that runs in Desktop (NUS) mode, which
 * the port cannot enter, so the file is never opened. */
bool storage_file_is_open(File* file) {
    UNUSED(file);
    return false;
}

long strtol(const char* nptr, char** endptr, int base) {
    const char* s = nptr;
    while(*s == ' ' || (*s >= '\t' && *s <= '\r')) s++;
    bool neg = false;
    if(*s == '+' || *s == '-') neg = (*s++ == '-');
    if((base == 0 || base == 16) && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if(base == 0) {
        base = (s[0] == '0') ? 8 : 10;
    }
    unsigned long acc = 0;
    const char* digits_start = s;
    for(;; s++) {
        int d;
        if(*s >= '0' && *s <= '9')
            d = *s - '0';
        else if(*s >= 'a' && *s <= 'z')
            d = *s - 'a' + 10;
        else if(*s >= 'A' && *s <= 'Z')
            d = *s - 'A' + 10;
        else
            break;
        if(d >= base) break;
        acc = acc * base + d;
    }
    if(endptr) *endptr = (char*)(s == digits_start ? nptr : s);
    return neg ? -(long)acc : (long)acc;
}
