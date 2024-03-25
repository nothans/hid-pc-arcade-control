/*
HID PC Control for Batocera running on a Lenovo Mini PC
Created: March 19, 2024 by Hans Scharler (http://nothans.com)
*/

#include <HID-Project.h>

// Define the pin where the button is connected
const int buttonInputPin = 2;

// Define variables to manage button presses
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastButtonPressTime = 0;
int pressCount = 0;

// Define constants for button press patterns
const unsigned long DOUBLE_PRESS_DELAY = 500;
const unsigned long SHUTDOWN_DELAY = 10000;

void setup() {
  pinMode(buttonInputPin, INPUT_PULLUP);

  // Initialize BootKeyboard from HID-Project
  BootKeyboard.begin();
}

void loop() {
  // Check button state
  int currentButtonState = digitalRead(buttonInputPin);

  // Check for a high to low transition
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  unsigned long elapsedTime = millis() - lastDebounceTime;
  
  if (elapsedTime > debounceDelay) {
    if (currentButtonState != buttonState) {
      buttonState = currentButtonState;

      // Button has been pressed
      if (buttonState == LOW) {
        unsigned long now = millis();
        
        // Check for button press patterns
        if (now - lastButtonPressTime < DOUBLE_PRESS_DELAY) {
          pressCount++;
        } else {
          pressCount = 1;
        }

        lastButtonPressTime = now;
      }
    }
  }

  // Perform actions based on the button press pattern
  elapsedTime = millis() - lastButtonPressTime;

  if (elapsedTime > DOUBLE_PRESS_DELAY && pressCount > 0) {
    switch (pressCount) {
      case 1:
        // Single press (power on or wakeup)
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.press('p');
        BootKeyboard.releaseAll();

        BootKeyboard.press(KEY_ENTER);
        BootKeyboard.releaseAll();

        break;
      case 2:
        // Double press (close)
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.press(KEY_F4);
        BootKeyboard.releaseAll();

        break;
      case 3:
        // Triple press (suspend)
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.press(KEY_F12);
        BootKeyboard.releaseAll();

        break;
    }
    pressCount = 0;
  }

  // Press and hold for 10 seconds (shutdown)
  if (elapsedTime > SHUTDOWN_DELAY && lastButtonPressTime > 0) {
    BootKeyboard.press(KEY_LEFT_CTRL);
    BootKeyboard.press(KEY_F12);
    BootKeyboard.releaseAll();

    lastButtonPressTime = 0;
  }

  lastButtonState = currentButtonState;
}