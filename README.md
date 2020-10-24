# arduino_alu_tester
Arduino program for testing ALU from Ben Eater's 8-bit computer. Iterates through all numbers on both registers and validates ALU output.
Note: doesn't currently check any flags or carry out because I ran out of pins on my Arduino Nano.

Requires Arduino with at least 12 data pins (not including serial pins) - tested with Arduino Nano. Pin definitions are at top of source file.

Requires two 8-bit shift registers to handle writing to registers and configuring register input flags and ALU sub flag. When writing to registers, you can either connect to bus if register inputs are connected to bus (make sure transceiver output is disabled so it's not writing to bus as well) or just connect to inputs of both registers.

Bit shift register 1:
* Qa-Qh => LSB of register A/B input (Qa) through MSB of register A/B input (Qh)
* Qh' => Chained to bit shift register 2

Bit shift register 2:
* Qa => IE for register A
* Qb => IE for register B
* Qc => SU flag for ALU to control subtraction

Outputs of ALU are connected directly to Arduino data pins. Note: my ALU output isn't connected to common bus with registers. If your's is, make sure the transceiver is disabled so it's not interfering with inputs.

![](https://github.com/adamldoyle/arduino_alu_tester/raw/main/alu_and_tester.jpg)
