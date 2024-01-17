extern volatile unsigned long timer0_overflow_count;  // Record the most recent timer ticks
volatile boolean ticks_valid;                         // Indicates a valid reading
volatile unsigned long ticks_per_rev;                 // Number of ticks counted in the current revolution
unsigned long msSinceRPMReading;                      // Number of mSec since the last rpm_sense (used to spot zero RPM)
float lastRPM; 
float oldRPM; 
float deltaRPM;                      // Most recent RPM reading, and highest RPM recorded since last reset. Previous RPM reading.

const float __TICKS_TO_RPMS = 15e6;                   // Convert clock ticks to RPM by dividng ticks into this number (tick = 16MHz/64 default scaler = 4 microsec = 250000 tick/sec so 1 minute = 15 to power 6 =15e6)
                                                      // The number will change if there are more magnets in an rpm 
                                                      //   (e.g. 2 magnets = 29296.875)
const unsigned int __WAIT_BEFORE_ZERO_RPM = 2000;     // Number of mS to wait for an rpm_sense before assunming RPM = 0.

unsigned long msSinceSend;                            // mSec when the last data was sent to the serial port
const unsigned int __WAIT_BETWEEN_SENDS = 1000;       // Number of mS to wait before sending a batch of data.
float kmh;

void setup()
{

  Serial.begin(9600);                                 // Initialise serial comms.
  msSinceSend = millis();                             // Data sent counter

  attachInterrupt(digitalPinToInterrupt(2), rpm_sense, RISING);              // RPM sense will cause an interrupt on pin2
  msSinceRPMReading = 0;                              // If more than 2000 (i.e. 2 seconds),
                                                      // then RPMs can be assumed to be zero (< 15rpm
                                                      // at most, with a single magnet, no small IC
                                                      // can run that slowly).
  lastRPM = 0;                                        // Current RPM to zero
  
 }

// ------------------------------------------------------------------------------------------------
// FUNCTION: RPM-SENSE
// 
// Called whenever the RPM sense signal rises. In my setup, the hall effect switch is normally
// high, goes low when a south pole is introduced to the sensor, and rises back to high as the
// magnet goes away. Thus, the RPM sense is called on the trailing edge of the magnet.
// ------------------------------------------------------------------------------------------------
void rpm_sense()
{
  static unsigned long pre_count;               // Last timer0_overflow_count value
  unsigned long ticks_now;                      //

  ticks_now = timer0_overflow_count;            // Read the timer

  byte t = TCNT0;
  if ((TIFR0 & _BV(TOV0)) && (t<255)) 
    ticks_now++;
  ticks_now = (ticks_now << 8) + t;

  if (pre_count == 0) {                         // First time around the loop?
    pre_count = ticks_now;                      // Yes - set the precount, don't use this number.
  } else {
    ticks_per_rev = ticks_now - pre_count;      // No - calculate the number of ticks...
    pre_count = ticks_now;                      // ...reset the counter...
    ticks_valid = true;                         // ...and flag the change up.
  }
}

void loop()
{
  unsigned long thisMillis = millis();          // Read the time once

  // Calculate RPM
  if (ticks_valid) {                            // Only come here if we have a valid RPM reading
    unsigned long thisTicks;
    
    noInterrupts();
    thisTicks = ticks_per_rev;
    ticks_valid = false;
    interrupts();
    
    oldRPM = lastRPM;						//Save previous RPM reading
    lastRPM = __TICKS_TO_RPMS / thisTicks;      // Convert ticks to RPMs
    ticks_valid = false;                        // Reset the flag.
    msSinceRPMReading = thisMillis;             // Set the time we read the RPM.
    deltaRPM = oldRPM - lastRPM;			//Calculate change of RPM
   
// Insert the calcs for difference between running sum deltaRPM and running sum Flywheel deltaRPM over time
 
  } else {
    // No tick this loop - is it more than X seconds since the last one?
    if (thisMillis - msSinceRPMReading > __WAIT_BEFORE_ZERO_RPM) {
      lastRPM = 0;                              // At least 2s since last RPM-sense, so assume zero RPMs
      msSinceRPMReading = thisMillis;           // Reset counter
    }
  }
 
  //kmh = (lastRPM * 1.8 * 60 / 1000);
  
  // Is it time to send the data?
  if (thisMillis < msSinceSend)                  // If thisMillis has recycled, reset it
    msSinceSend = millis();
    
  if (thisMillis - msSinceSend > __WAIT_BETWEEN_SENDS) {
    // Yes: Build the serial output...
    
    Serial.print("RPMs = \t");
    Serial.print(lastRPM);                       // Current RPMs
	Serial.print("\t oldRPMs = \t");
    Serial.print(oldRPM);                       // Current RPMs
    Serial.print("\t Delta \t");
    Serial.print(deltaRPM);
    Serial.println();                            // EOL.

    msSinceSend = millis();                      // Reset the timer 
    
  }  }
