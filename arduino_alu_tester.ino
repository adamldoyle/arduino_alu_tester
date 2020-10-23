/*
 * Arduino program for testing ALU from Ben Eater's 8-bit computer. Iterates through all numbers on both registers and validates ALU output.
 * Note: doesn't currently check any flags or carry out because I ran out of pins on my Arduino Nano.
 * 
 * Requires Arduino with at least 12 data pins (not including serial pins) - tested with Arduino Nano. Pin definitions are at top of source file.
 * 
 * Requires two 8-bit shift registers to handle writing to registers and configuring register input flags and ALU sub flag. When writing to registers, you can either connect to bus if register inputs are connected to bus (make sure transceiver output is disabled so it's not writing to bus as well) or just connect to inputs of both registers.
 * 
 * Bit shift register 1:
 * - Qa-Qh => LSB of register A/B input (Qa) through MSB of register A/B input (Qh)
 * - Qh' => Chained to bit shift register 2
 * 
 * Bit shift register 2:
 * - Qa => IE for register A
 * - Qb => IE for register B
 * - Qc => SU flag for ALU to control subtraction
 * 
 * Outputs of ALU are connected directly to Arduino data pins. Note: my ALU output isn't connected to common bus with registers. If your's is, make sure the transceiver is disabled so it's not interfering with inputs.
 */

// Pins for driving bit shift registers
#define PIN_DATA 2
#define PIN_SRCLK 3
#define PIN_RCLK 4

// Pin for driving register clock
#define PIN_REGCLK 13

// How long in ms to delay between number outputs. Runs fine at 0 for quick testing (LSB LEDs likely won't flash).  Looks "better" with a slight delay to actually show what's going on.
#define OUTPUT_DELAY 0
//#define OUTPUT_DELAY 20

// Pins that are connected to ALU output
#define PIN_IN_LSB 5
#define PIN_IN_MSB 12

// If true, print out every calculation; if false, only print out errors
#define DEBUG false

// If true, halts on bad calculation; if false, continues
#define STOP_ON_ERROR true

bool stop = false;
char buf[30];

void setup() {
  Serial.begin(57600);

  // Set pins for bit shift registers to output
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_RCLK, OUTPUT);
  pinMode(PIN_REGCLK, OUTPUT);

  // Set pins for reading ALU to input
  for (int i = PIN_IN_LSB; i <= PIN_IN_MSB; i++) {
    pinMode(i, INPUT);
  }

  // Set everything low to start
  digitalWrite(PIN_SRCLK, LOW);
  digitalWrite(PIN_RCLK, LOW);
  digitalWrite(PIN_REGCLK, LOW);

  // Clear both registers
  outputNumber(0, 0, false);
}

// Sends a number to a register
void outputNumber(int number, int regOut, bool sub) {
  // Push out active register and add/sub settings
  shiftOut(PIN_DATA, PIN_SRCLK, MSBFIRST, regOut | (sub ? 0b100 : 0));
  
  // Push out number
  shiftOut(PIN_DATA, PIN_SRCLK, MSBFIRST, number);
  
  // Pulse output clock to make data available
  digitalWrite(PIN_RCLK, LOW);
  digitalWrite(PIN_RCLK, HIGH);
  digitalWrite(PIN_RCLK, LOW);

  // Pulse register clock to bring in data
  pulseClock();

  // Delay before next output
  delay(OUTPUT_DELAY);
}

// Pulse register clock to bring in data from "bus"
void pulseClock() {
  digitalWrite(PIN_REGCLK, HIGH);
  digitalWrite(PIN_REGCLK, LOW);
}

// Read 8-bit number from ALU
int readAnswer() {
  int answer = 0;
  for (int i = PIN_IN_MSB; i >= PIN_IN_LSB; i--) {
    answer = (answer << 1) | digitalRead(i);
  }
  return answer;
}

// Outputs aNumber and bNumber to A and B registers and validates ALU produces correct solution
bool verifyAddition(int aNumber, int bNumber) {
  outputNumber(aNumber, 2, false);
  outputNumber(bNumber, 1, false);
  
  int answer = readAnswer();
  int solution = (aNumber + bNumber) & 255;
  if (answer != solution) {
    sprintf(buf, "Error: %i + %i <> %i (%i)", aNumber, bNumber, answer, solution);
    Serial.println(buf);
    return false;
  } else {
#if (DEBUG)
    sprintf(buf, "%i + %i = %i", aNumber, bNumber, answer);
    Serial.println(buf);
#endif
  }
  return true;
}

// Outputs aNumber and bNumber to A and B registers and validates ALU produces correct solution
bool verifySubtraction(int aNumber, int bNumber) {
  outputNumber(aNumber, 2, true);
  outputNumber(bNumber, 1, true);
  
  signed char answer = readAnswer();
  signed char bNumber2 = bNumber;
  signed char solution = aNumber - bNumber2;
  if (answer != solution) {
    sprintf(buf, "Error: %i - %i <> %i (%i)", aNumber, bNumber2, answer, solution);
    Serial.println(buf);
    return false;
  } else {
#if (DEBUG)
    sprintf(buf, "%i - %i = %i", aNumber, bNumber2, answer);
    Serial.println(buf);
#endif
  }
  return true;
}

void loop() {
  if (stop) {
    return;
  }

#if (!DEBUG)
  Serial.println("Addition");
#endif
  // Loop through all iterations of [0-255] + [0-255] and verify addition
  for (int aNumber = 0; aNumber < 256; aNumber++) {
    for (int bNumber = 0; bNumber < 256; bNumber++) {
      if (!verifyAddition(aNumber, bNumber)) {
#if (STOP_ON_ERROR)
        stop = true;
        return;
#endif
      }
    }
#if (!DEBUG)
    Serial.print(".");
    if (aNumber && aNumber % 50 == 0) {
      Serial.println(aNumber);
    }
#endif    
  }
#if (!DEBUG)
  Serial.println(" done");
  Serial.println("\nSubtraction");
#endif

  // Loop through all iterations of [0-255] + [0-255] and verify subtraction
  for (int aNumber = 0; aNumber < 256; aNumber++) {
    for (int bNumber = 0; bNumber < 256; bNumber++) {
      if (!verifySubtraction(aNumber, bNumber)) {
#if (STOP_ON_ERROR)
        stop = true;
        return;
#endif
      }
    }
#if (!DEBUG)
    Serial.print(".");
    if (aNumber && aNumber % 50 == 0) {
      Serial.println(aNumber);
    }
#endif
  }
#if (!DEBUG)
  Serial.println(" done");
#endif
}
