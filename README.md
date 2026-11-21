# Flipper Claude Code Buddy

Turn your Flipper Zero into a physical companion for Claude Code. Get tactile feedback for every AI action — feel the difference between a completed task, an error, and an approval request — and control Claude with real buttons instead of typing.

> Supports macOS and Linux. Windows is not tested.

## LilyGo T-Embed CC1101 (ESP32 port) build

This fork runs Claude Buddy on the
[Flipper Zero ESP32 port](https://github.com/Sor3nt/Flipper-Zero-ESP32-Port)
(firmware 2.0.0) on a LilyGo T-Embed CC1101. It is not a build for a stock Flipper Zero.

- **Bluetooth bridge mode only.** Port 2.0.0 gives apps no USB serial, so the USB transport
  is stubbed. Desktop (Hardware Buddy) mode needs the STM32 Bluetooth stack, so choosing it
  falls back to the Bluetooth bridge. Set `FLIPPER_TRANSPORT=ble` for the host bridge
  (e.g. `"env": {"FLIPPER_TRANSPORT": "ble"}` in `~/.claude/settings.json`): with the cable
  in, `auto` picks the T-Embed's log-only USB port.
- **Rotary controls.** The wheel has no long presses on the arrows, so a long wheel press
  opens an **Actions** menu: Commands, Approve (yes), Esc, Interrupt (Ctrl+C), Mute,
  Dictate (click stops), Info, Exit. Turn = Down / dictation, click = Enter, side key =
  Backspace, hold side key = exit.
- **Bluetooth once per boot.** Port 2.0.0 only survives one app-started Bluetooth profile
  per boot. Boot with Bluetooth **off** in settings, then open Claude Buddy first.
- Firmware symbols that port 2.0.0 does not export (power, RTC set, notification
  sequences, `strtol`, …) are defined in `flipper-app/esp32_compat.c`. Battery voltage and
  current read 0, and host time sync is ignored.

Install: copy `claude_buddy.fap` to `apps/Bluetooth/` on the SD card.

## What it does

**You feel what Claude is doing.**
Every significant event triggers a distinct sound and vibration pattern on your Flipper, so you always know Claude's status even when you're not looking at the screen.

**You control Claude with physical buttons.**
Interrupt a runaway task, submit a prompt, trigger voice dictation, or open the slash command menu — all without touching the keyboard.

## Two modes

Claude Buddy runs in one of two modes, switchable from the on-device menu (long-press **Right → MENU**, then the top row):

- **Claude Code (USB/BLE)** (default) — talks to Claude Code in the terminal via the companion plugin and Python host bridge. USB or BLE. Full keystroke forwarding: Enter, Esc, voice dictation, slash-command menu, etc.
- **Claude Desktop (BLE)** — talks directly to the Claude Desktop app over BLE using Anthropic's [Hardware Buddy](https://github.com/anthropics/claude-desktop-buddy) protocol (Nordic UART Service). No plugin, no host bridge. The Flipper shows live status from Claude Desktop (running sessions, token counts, recent transcript) and lets you Allow / Deny permission prompts right from the device.

### Enabling Hardware Buddy mode in Claude Desktop

1. On the Flipper, switch to **Claude Desktop (BLE)** in the info menu.
2. In the Claude Desktop app: **Help → Troubleshooting → Enable Developer Mode**.
3. Open **Developer → Open Hardware Buddy** and pick your Flipper from the scan list. macOS will prompt for Bluetooth permission the first time.

Once paired, Claude Desktop auto-reconnects whenever both sides are online.

## Buttons

| Button | Action |
|--------|--------|
| UP | Start / stop voice dictation |
| UP (hold) | Hold Space for voice input |
| LEFT | Interrupt Claude (Esc) |
| LEFT (hold) | Send Ctrl+C |
| RIGHT | Open slash command menu |
| RIGHT (hold) | Open menu |
| OK | Submit Enter (⏎) |
| OK (hold) | Type "yes" and submit |
| DOWN | Send Down arrow (↓) |
| DOWN (hold) | Toggle mute |
| BACK | Send Backspace (⌫) |
| BACK (hold) | Exit |

## Setup

### 1. Install the Flipper app

Download `claude_buddy.fap` from the [latest release](../../releases/latest) and copy it to your Flipper Zero:

- **Via qFlipper:** SD Card → `apps/USB/` → drag and drop
- **Via SD card:** copy to `apps/USB/` directly

### 2. Install the Claude Code plugin

> **Requires Python 3.10 or higher.** If you're on an older system Python, upgrade first (e.g. via [pyenv](https://github.com/pyenv/pyenv) or [python.org](https://www.python.org/downloads/)), then reinstall the plugin.

```bash
claude plugin marketplace add jxw1102/flipper-claude-buddy
claude plugin install flipper-claude-buddy@flipper-claude-buddy
```

Claude Code will ask for your connection preference (`auto`, `usb`, or `ble`). Leave everything else empty for auto-detect.

The plugin starts automatically with every Claude Code session and stops when you close it.

### 3. Launch Claude Buddy on your Flipper

Go to **Applications → USB → Claude Buddy**. You'll hear the startup fanfare when the connection is established.

## Connection

Connects over USB (plug-and-play) or Bluetooth LE — whichever is available. USB is preferred when the cable is plugged in; it falls back to BLE automatically.

**macOS — First-time Bluetooth pairing:** on first BLE connection macOS will pair with the Flipper. Accept the pairing prompt on both sides. If the connection fails after a firmware flash or factory reset, remove the Flipper from System Settings → Bluetooth and let it re-pair.

**macOS — Bluetooth permission:** Terminal (or your terminal app) must have Bluetooth access. Grant it in System Settings → Privacy & Security → Bluetooth.

**macOS — Accessibility permission (required for keystroke forwarding):** Flipper button presses are delivered to your terminal via AppleScript (`osascript`), which needs Accessibility permission. Grant it in System Settings → Privacy & Security → Accessibility and toggle on your terminal app (Terminal, iTerm2, WezTerm, Alacritty, Ghostty, etc.). Without this, the Flipper will see Claude's status (e.g. "thinking...") but pressing OK, LEFT, RIGHT, etc. will do nothing. If your terminal doesn't prompt automatically, add it manually. The bridge log will show `osascript is not allowed to send keystrokes` when this permission is missing. Voice dictation (UP) also depends on this.

**Linux — USB:** Flipper appears as `/dev/ttyACM*`. No additional drivers needed. If you get a permission error, add your user to the `dialout` group:
```bash
sudo usermod -aG dialout $USER  # log out and back in to apply
```

**Linux — Keystroke forwarding:** Flipper button presses are forwarded to your terminal via `xdotool` (X11 only). Install it if needed:
```bash
sudo apt install xdotool
```
Wayland is not yet supported for keystroke forwarding.

**Linux — BLE:** BLE transport works via BlueZ. Make sure BlueZ is installed and running:
```bash
sudo apt install bluetooth bluez
sudo systemctl enable --now bluetooth
sudo usermod -aG bluetooth $USER  # log out and back in to apply
```

## Troubleshooting

| Problem | Fix |
|---|---|
| Flipper not found over USB (macOS) | Make sure no other app (qFlipper, Chrome serial, etc.) is using the port. If it still fails, set `FLIPPER_SERIAL_PORT=/dev/cu.usbmodemXXX` explicitly. |
| Flipper not found over USB (Linux) | Check `ls /dev/ttyACM*` — if empty, try a different USB cable. If the port exists but access is denied, run `sudo usermod -aG dialout $USER` and log out/in. Set `FLIPPER_SERIAL_PORT=/dev/ttyACMX` explicitly if needed. |
| Flipper not found over BLE | Make sure Bluetooth is on and the app is running on the Flipper |
| No sound on task complete | Check that the bridge is running: `cat /tmp/claude-flipper-bridge.log` |
| Buttons do nothing / `osascript is not allowed to send keystrokes` in log (macOS) | Grant your terminal app Accessibility permission in System Settings → Privacy & Security → Accessibility. Terminals like WezTerm, Alacritty, or Ghostty often don't prompt automatically — add them manually. |

## Support

If you find this useful, consider [buying me a coffee](https://ko-fi.com/jxw1102).

## License

MIT — see [LICENSE](LICENSE).
