// #include <Arduino.h> For Visual Studio Code

bool MeasEnd = false;
unsigned long Tcapture;
volatile unsigned long T1 = 0;
volatile unsigned long T2 = 0;
volatile unsigned long T = 0;

void INT0_ISR()
{
Tcapture = micros();

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

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), INT0_ISR, RISING);
}

void loop()
{ 
noInterrupts();
// for testing only
Serial.print("Time: ");
Serial.print(T);
Serial.print("\t Time1: ");
Serial.print(T1);
Serial.print("\t Time2: ");
Serial.print(T2);
Serial.println();
interrupts();
delay (1000);
}
