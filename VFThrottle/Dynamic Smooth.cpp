#include <Arduino.h> // for VS Code

//#include <Arduino.h> // for VS Code

// Throttle code from Akom's smooth throttle using an Arduino (Pro Micro 5V or similar)
// Input pin for throttle signal
#define PIN_IN A1
// Output pin to the controller
#define PIN_OUT 3 //UNO R4 DAC 0. Nano PWM 3
// ms delay between debugging prints
#define PRINT_DELAY 100

/* fine tune the throttle range to eliminate deadband */
// Normal range of throttle
#define THROTTLE_MAP_IN_MIN 1
#define THROTTLE_MAP_IN_MAX 1023
// Range we want to send to the controller
// Throttle code from Akom's smooth throttle using an Arduino (Pro Micro 5V or similar)
// Input pin for throttle signal
#define PIN_IN A1
// Output pin to the controller
#define PIN_OUT 3 //UNO R4 DAC 0. Nano PWM 3
// ms delay between debugging prints
#define PRINT_DELAY 100

/* fine tune the throttle range to eliminate deadband */
// Normal range of throttle
#define THROTTLE_MAP_IN_MIN 1
#define THROTTLE_MAP_IN_MAX 1023
// Range we want to send to the controller
// values from "no movement yet" to "max speed":
#define THROTTLE_MAP_OUT_MIN 10
#define THROTTLE_MAP_OUT_MAX 800

/* Jerkiness Mitigation */
// how quickly to adjust output, larger values are slower. Were #define but variable for use with dynamic ramp.
unsigned long INCREASE_ERROR_FACTOR = 100;
int DECREASE_ERROR_FACTOR = 1;
unsigned long INCREASE_FACTOR = 1;         //multiply with Period to get INCREASE_ERROR_FACTOR. Adjust to suit.
int INCREASE_MIN = 300;
int INCREASE_MAX = 20;
// Basically delay between loops:
#define TICK_LENGTH_MS 1

// operational variables
int throttleValue = 0;      //input value from 3-wire throttle 
int limitValue = 0;         //input value from potentiometer 
int delta = 0;              //computed error from last setting (outputValue)
float outputValue = 0;      // 0-1024, later mapped to 0-255
float adjustmentAmount = 0; // 
int mapped = 0;             // outputValue after mapping to controller range
unsigned long lastPrint = 0;

//Motor Period variables
bool MeasEnd = false;
unsigned long Tcapture;
volatile unsigned long T1 = 0;
volatile unsigned long T2 = 0;
volatile unsigned long T = 0;
unsigned long PERIOD_MIN;
unsigned long PERIOD_MAX;
int RPM_MIN = 200;
int RPM_MAX = 5000;
byte MOTOR_POLES = 5;

void INT0_ISR()
{
Tcapture = micros(); // micros() for Atmel devices microseconds() for R4

  if(!MeasEnd)
  {
    T1 = Tcapture;
    MeasEnd = true;
  }
  else
    {
    T2 = Tcapture;
    T = T2 - T1;
    MeasEnd = false;
    }

}

void setup() {
	Serial.begin(115200);
    pinMode(PIN_IN, INPUT);
    pinMode(PIN_OUT, OUTPUT);
    outputValue = analogRead(PIN_IN); // initial value 
    pinMode(2, INPUT);
    attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, RISING);
    PERIOD_MIN = 1000000/((RPM_MIN / 60) * MOTOR_POLES);
    PERIOD_MAX = 1000000/((RPM_MAX / 60) * MOTOR_POLES);

}

void loop() {
  // INCREASE_ERROR_FACTOR = (INCREASE_FACTOR * T) / 10; //this factor isn't working. Range of T is too wide. Probably use map() BELOW
  
  INCREASE_ERROR_FACTOR = map(
        T,
        PERIOD_MIN,
        PERIOD_MAX,
        INCREASE_MIN,
        INCREASE_MAX
  );
  
  throttleValue = analogRead(PIN_IN);
    delta = throttleValue - outputValue;
    adjustmentAmount = (float) delta / (float) (delta > 0 ? INCREASE_ERROR_FACTOR : DECREASE_ERROR_FACTOR);
// adjustment = difference between Throttle and PreviousThrottle / Increase factor (300) or decrease factor (1)

outputValue += adjustmentAmount;

    // throttle to output value map. Integer math only.
    mapped = map(
            outputValue,
            THROTTLE_MAP_IN_MIN,
            THROTTLE_MAP_IN_MAX,
            THROTTLE_MAP_OUT_MIN,
            THROTTLE_MAP_OUT_MAX
    );

    analogWrite(
            PIN_OUT,
            mapped * 4 // DAC is 0-4096 while our values are 0-1023 - multiply (or divide) to suit.
    );

 //	write IO to serial for debugging
	if ((lastPrint + PRINT_DELAY) < millis()) {
	lastPrint = millis();
	Serial.print("Input: ");
	Serial.print(throttleValue);
	Serial.print(" Output: ");
	Serial.print(outputValue);
	Serial.print(" Period: ");
	Serial.print(T);	
	Serial.print(" PeriodMin: ");
	Serial.print(PERIOD_MIN);
	Serial.print(" Period_Max: ");
	Serial.print(PERIOD_MAX);
	Serial.print(" INCREASE: ");
	Serial.print(INCREASE_ERROR_FACTOR);	
	Serial.print(" Mapped: ");
	Serial.print(mapped);
 	Serial.print(" +/-: ");
	Serial.print(adjustmentAmount);
// 	Serial.print(" Lim: ");
//	Serial.print(limitValue);
// 	Serial.print(" Lim Map: ");
//	Serial.print(map(limitValue, LIMIT_MAP_IN_MIN, LIMIT_MAP_IN_MAX, LIMIT_MAP_OUT_MIN, LIMIT_MAP_OUT_MAX));
	Serial.println("");
	}

    delay(TICK_LENGTH_MS);
}