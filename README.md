# ATmega32 Timer2 Button and LED Demo

A small Embedded C project for the ATmega32 that demonstrates periodic timer interrupts, button debouncing, and responsive GPIO control without blocking delays.

## Features

- Uses Timer2 in CTC mode to generate a 1 ms system tick at an 8 MHz CPU clock.
- Debounces an active-low push button in the timer compare interrupt.
- Toggles an LED on each stable button press.
- Keeps the foreground loop non-blocking; no delay-based polling is used.

## Hardware

- ATmega32 running at 8 MHz
- One momentary push button
- One LED and a current-limiting resistor (220–1,000 Ω)
- 5 V supply and common ground

### Connections

| Signal | ATmega32 pin | Connection |
| --- | --- | --- |
| Button | PD2 | Button between PD2 and GND; the internal pull-up is enabled |
| LED | PB0 | PB0 → resistor → LED anode; LED cathode → GND |

Use a regulated supply and verify the MCU's clock/fuse configuration before programming. The firmware assumes the CPU is actually clocked at 8 MHz.

## Build

Install AVR-GCC and AVR Libc, then run:

```sh
mkdir -p build
avr-gcc -mmcu=atmega32 -DF_CPU=8000000UL -std=gnu11 -Os -Wall -Wextra \
  -o build/main.elf src/main.c
avr-objcopy -O ihex -R .eeprom build/main.elf build/main.hex
```

Program with a compatible ISP programmer, for example USBasp:

```sh
avrdude -c usbasp -p m32 -U flash:w:build/main.hex:i
```

## How it works

Timer2 runs in CTC mode with a divide-by-64 prescaler and `OCR2 = 124`, producing a 1 kHz compare interrupt at 8 MHz. The interrupt applies an integrator-style 20-sample debounce filter and records a press event. The main loop consumes that event atomically and updates PB0.

The button logic and LED output are intentionally kept separate: the interrupt handles timing and input stability, while the foreground loop handles the application action.

## Configuration

- Change `F_CPU` to match the hardware clock and recalculate the Timer2 compare value if needed.
- Change `DEBOUNCE_TICKS` to adjust the debounce interval.
- Change `LED_PIN` or `BUTTON_PIN` and their ports to use different GPIO pins.

## Repository layout

```text
.
├── README.md
├── .gitignore
└── src/
    └── main.c
```

## Notes

This is an educational hardware example. Confirm pin wiring, supply voltage, clock fuses, and programmer settings for your board before use.
