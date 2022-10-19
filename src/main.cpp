/*
 * I, Justin Borzi, 000798465 certify that this material is my original
 * work. No other person's work has been used without due
 * acknowledgement.
 */

#include <Arduino.h>

// Delays for the alarm.
#define LED_DELAY 62.5
#define ALARM_DELAY 10
#define ALARM_LOOP_DELAY 1000

// Times to blink within a second.
#define BLINK_COUNT 4

// digital input pin definitions.
#define PIN_PIR D5
#define PIN_BUTTON D6

// define all possible alarm states.
#define ALARM_DISABLED 0
#define ALARM_ENABLE 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3

// Track the Alarm state.
int iAlarmState = ALARM_DISABLED;

// Setup a DEBUG control variable.
bool debugEnabled = true;

// Track the state of the button and light.
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

/**
 * @brief Check if the button was pressed, and if so change the state of the alarm and device.
 */
void checkButtonPress()
{
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

        // Swap the alarm state to on or off depending on previous state.
        if (iAlarmState == ALARM_DISABLED)
        {
          iAlarmState = ALARM_ENABLE;
        }
        else if (iAlarmState != ALARM_DISABLED)
        {
          iAlarmState = ALARM_DISABLED;
        }

        if (debugEnabled)
        {
          Serial.println();
          if (ledState == LOW)
          {
            Serial.println("Button Pressed: On");
          }
          else
          {
            Serial.println("Button Pressed: Off");
          }

          if (iAlarmState == ALARM_DISABLED)
          {
            Serial.println("Alarm State: Off");
          }
          else
          {
            Serial.println("Alarm State: On");
          }

          Serial.println();
        }
      }
    }
  }
}

/**
 * @brief Check the state of the Alarm.
 * @param state The state of the alarm to check against.
 */
bool checkAlarmState(int state)
{
  return iAlarmState == state;
}

/**
 * @brief Blink the LED a set amount of times.
 * @param times The amount of times to blink the LED.
 */
void blinkLED(int times)
{
  for (int i = 0; i < times; i++)
  {
    checkButtonPress();
    digitalWrite(LED_BUILTIN, LOW);
    delay(LED_DELAY);
    checkButtonPress();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(LED_DELAY);
    checkButtonPress();
  }
}

/**
 * @brief Start the countdown til the alarm is triggered.
 */
void startCountdown()
{
  // Set the amount of time to wait for alarm to trigger.
  int countdown = ALARM_DELAY;

  // Loop until the countdown is complete.
  while (countdown > 0)
  {
    if (debugEnabled)
    {
      Serial.println("Countdown: " + String(countdown));
    }

    // Check if the alarm has been disabled successfully.
    if (checkAlarmState(ALARM_DISABLED))
    {
      break;
    }

    // Start blinking the LED for the second
    blinkLED(BLINK_COUNT);

    countdown--;

    // Wait for a second
    delay(ALARM_LOOP_DELAY);
  }

  // If the alarm is still in Countdown state then trigger the alarm.
  if (checkAlarmState(ALARM_COUNTDOWN))
  {
    iAlarmState = ALARM_ACTIVE;
  }
}

// *************************************************************
void loop()
{
  bool bPIR;

  // Listen for the button press
  checkButtonPress();

  // Check if the device is enabled
  if (ledState == LOW)
  {
    // Read the incoming PIR signal
    bPIR = digitalRead(PIN_PIR);

    // Check if the Alarm is enabled
    if (checkAlarmState(ALARM_ENABLE))
    {
      // Check if the PIR is triggered
      if (bPIR)
      {
        // Set the alarm state to countdown
        iAlarmState = ALARM_COUNTDOWN;
        if (debugEnabled)
        {
          Serial.println("PIR Signal Detected: " + String(bPIR));
        }
      }
    }

    // Check if the alarm is in countdown mode
    if (checkAlarmState(ALARM_COUNTDOWN))
    {
      if (debugEnabled)
      {
        Serial.println("Alarm State: Countdown Started");
      }
      // Start the countdown
      startCountdown();
    }

    // Check if the alarm is in active mode
    if (checkAlarmState(ALARM_ACTIVE))
    {
      Serial.println("Alarm State: Active");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
