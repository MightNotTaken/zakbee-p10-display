# Display Control README

## Overview
Repository contain code for  the "Display Control" project.

## Pin Definitions
For controlling the display, the following pin definitions are used:

### Matrix Control Pins
- OE (Output Enable): GPIO 13
- LAT (Latching Pin): GPIO 23
- CLK (Clock Pin):    GPIO 21
- A:                  GPIO 27
- B:                  GPIO 19
- C:                  GPIO 14

## Key Definitions
The following keys are defined for controlling the display:

- M1_KEY: GPIO 32
- M2_KEY: GPIO 33
- M3_KEY: GPIO 34
- M4_KEY: GPIO 35
- M5_KEY: GPIO 36
- M6_KEY: GPIO 39
- M7_KEY: GPIO 25
- M8_KEY: GPIO 26

## Library Dependency
This project requires the `RGBmatrixPanel` library for controlling the display. Make sure to install this library in your Arduino IDE before compiling the code.

## Contributors
- [Tahir Mehmood](https://github.com/mightnottaken) - Initial code author and maintainer

