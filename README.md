# zmk-config-charybdis-nano

ZMK config for a BastardKB-style **Charybdis Nano**, 35 keys (3x5 per half,
3 left thumbs, 2 right thumbs), integrated **PMW3610 trackball** on the right
half.

## Hardware

- Controllers: SuperMini nRF52840 (nice!nano v2 clone) → board `nice_nano_v2`
- Wireless split, **no dongle**: right half (trackball side) is the BLE central
- Trackball: PMW3610 (VDD/GND/CLK/SDIO/CS/MOTION)
- BLE name: `BKB-Nano35`

Built on the [victorlucachi/zmk-keyboards-charybdis](https://github.com/victorlucachi/zmk-keyboards-charybdis)
module + [badjeff/zmk-pmw3610-driver](https://github.com/badjeff/zmk-pmw3610-driver),
ZMK `main`, with ZMK Studio enabled on the right half.

## Keymap

Port of [Temper_zmk](https://github.com/bdimitrako/Temper_zmk) (36-key
Colemak-DH, sticky mods, combos) adapted to 35 keys:

- The 3rd right thumb key (`&lt 5 DEL`) no longer exists:
  - **FUN layer** → combo: both right thumb keys together (toggle; exit with
    the same combo or the `tobase` combo).
  - **DEL** → hold NAV (left SPACE thumb) + tap the right BSPC thumb.
- All Temper base-layer combos carried over unchanged (same positions).
- Trackball-native mouse handling:
  - pointer always active; **hold NAV = scroll** (1/3 speed, natural direction)
  - **MSE layer (4)**: clicks on the left home row (T=left, S=middle,
    R=right click), `G` = hold for snipe (1/3 pointer speed), browser
    shortcuts on the bottom row, thumb clicks as before
  - an optional **auto-mouse layer** (`&zip_temp_layer`) is included
    commented-out in `config/charybdis.keymap` with tuning notes
- NAV zoom keys (`Ctrl -` / `Ctrl Shift =`) were dropped with the thumb key;
  media prev on BLT is now Shift+next (mod-morph).

## Flashing

1. Download the `firmware` artifact from the latest green
   [Actions](../../actions) run.
2. Double-tap the reset button on a half → it mounts as a USB drive
   (`NICENANO`).
3. Copy the matching uf2: `charybdis_right-nice_nano_v2-zmk.uf2` (right,
   Studio-enabled central), `charybdis_left-nice_nano_v2-zmk.uf2` (left).
4. If pairing misbehaves, flash `settings_reset-nice_nano_v2-zmk.uf2` to both
   halves first, then re-flash the real firmware.

> The original vendor uf2 files (BLE name "V&Z-Nano35") are the recovery
> fallback if this firmware misbehaves — keep them.
