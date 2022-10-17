/*
 * I, Justin Borzi, 000798465 certify that this material is my original
 * work. No other person's work has been used without due
 * acknowledgement. (Replace with your own name and student number)
 */

#include <Arduino.h>

#define BUTTON_DELAY 1000

// digital input pin definitions
#define PIN_PIR D5
#define PIN_BUTTON D6

// define all possible alarm states.
#define ALARM_DISABLED 0
#define ALARM_ENABLE 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3

int iAlarmState;

// Track the state of the button and light
byte lastButtonState = HIGH;
byte ledState = HIGH;

// Setup the debounce timings and time since it was last clicked.
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;

void setup()
{
  // configure the USB serial monitor
  Serial.begin(115200);

  // configure the LED output
  pinMode(LED_BUILTIN, OUTPUT);

  // PIR sensor is an INPUT
  pinMode(PIN_PIR, INPUT);

  // Button is an INPUT
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

// *************************************************************
void loop()
{
  bool bPIR;

  // Check the time between button presses is atleast ~50ms for a debounce.
  if (millis() - lastTimeButtonStateChanged > debounceDuration)
  {
    analogWrite(LED_BUILTIN, 1023);

    // Read the current state
    byte buttonState = digitalRead(PIN_BUTTON);

    // Determine if the button press doesnt equal the last. if it doesnt then its a new button press...
    if (buttonState != lastButtonState)
    {

      // Save the new button state as the last state.
      lastButtonState = buttonState;

      if (buttonState == LOW)
      {
        // Swap the LED state to on or off depending on previous state.
        ledState = (ledState == HIGH) ? LOW : HIGH;
      }
    }
  }

  if (ledState == LOW)
  {
    // read PIR sensor (true = Motion detected!).  As long as there
    // is motion, this signal will be true.  About 2.5 seconds after
    // motion stops, the PIR signal will become false.
    if (iAlarmState == ALARM_ACTIVE)
    {
      bPIR = digitalRead(PIN_PIR);
    }
    if (bPIR)
    {
      int countdown = 10;
      while (iAlarmState == ALARM_COUNTDOWN)
      {
        // read the brightness and set it to a global variable.
        // send the PIR signal directly to the LED
        // but invert it because true = LED off!
        digitalWrite(LED_BUILTIN, countdown % 2 == 0);
        Serial.println(countdown);
        delay(BUTTON_DELAY);
        countdown--;
      }
    }
  }
}