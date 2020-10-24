# arduino_alu_tester
Arduino program for testing ALU from Ben Eater's 8-bit computer. Iterates through all numbers on both registers and validates transfers, addition and subtraction.
Note: doesn't currently check any flags or carry out because I ran out of pins on my Arduino Nano.

Included are a bussed and non-bussed version of the script depending on setup. Pin designations are included at the top of each script.

Requires Arduino with at least 12 data pins (not including serial pins) - tested with Arduino Nano.

Requires two 8-bit shift registers to handle writing to registers and configuring register input flags and ALU sub flag.

Note: clock module isn't currently used for anything (clock is driven by Arduino).

## Bussed layout
![](https://github.com/adamldoyle/arduino_alu_tester/raw/main/bussed_alu_and_tester.jpg)

## Non-bussed layout
![](https://github.com/adamldoyle/arduino_alu_tester/raw/main/alu_and_tester.jpg)
