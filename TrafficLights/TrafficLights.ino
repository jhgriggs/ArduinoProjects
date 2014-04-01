/**
 * TrafficLights.ino
 *
 * This program is a simple simulation of automatic
 * traffic lights. The pedestrian lights are controlled
 * by the timers of the vehicle lights. A push button
 * can read a walk request by the pedestrian; when pressed 
 * during a green light, the walk signal appears sooner 
 * than it would during a normal cycle.
 *
 * Vehicle lights: red, yellow, green.
 * Pedestrian lights: walk, blinking warning, don't walk.
 * The pedestrian lights in this case are set so that
 * "walk" light corresponds to a green LED, and that the 
 * "blinking warning" and "don't walk" lights use a red LED. 
 *
 * This program uses the DigitalLed and PushButton libraries,  
 * which can be downloaded from the jhgriggs/ArduinoLibraries 
 * repository.
 *
 * See the corresponding TrafficLights.fzz Fritzing file 
 * for the breadboard layout and schematic.
 *
 * @author Janette H. Griggs
 * @version 1.0 03/31/14
 */

#include <DigitalLed.h>
#include <PushButton.h>

// LED pin numbers for vehicle lights.
const int RED_PIN = 12;
const int YELLOW_PIN = 8;
const int GREEN_PIN = 7;

// LED pin numbers for pedestrian lights.
const int WALK_PIN = 2;
const int NO_WALK_PIN = 4;

// Active duration for each vehicle light.
const unsigned int RED_DURATION = 10000;
const unsigned int YELLOW_DURATION = 2000;
const unsigned int GREEN_DURATION = 15000;

// Blink interval for blinking warning.
const unsigned int WARNING_BLINK_INTERVAL = 500;

// Pin number for walk request button.
const int WALK_BUTTON_PIN = 5;

// Resistor mode for walk request button.
const ResistorMode WALK_BUTTON_RESISTOR = PULL_UP;

// Debounce delay for walk request button.
const unsigned long WALK_BUTTON_DEBOUNCE = 50L;

// Create vehicle traffic light, set to green.
DigitalLed vehicleLed(GREEN_PIN);

// Create pedestrian traffic light, set to don't walk.
DigitalLed pedestrianLed(NO_WALK_PIN);

// Create walk request button.
PushButton walkButton(WALK_BUTTON_PIN, WALK_BUTTON_RESISTOR);

// Create other variables.
unsigned long previousMillis; // previous time
unsigned long deltaMillis; // change in time
boolean walkRequested; // walk request status

void setup() {
  // Set time variables to 0.
  previousMillis = 0L;
  deltaMillis = 0L;
  // Set walk request status to false.
  walkRequested = false;
}

void loop() {
  // Get the current time.
  unsigned long currentMillis = millis();
  // Calculate the change in time from the previous loop time.
  deltaMillis = currentMillis - previousMillis;
  
  // If a walk request has not been made, check for it.
  if (!walkRequested && walkButton.detectPush(deltaMillis, 
                        WALK_BUTTON_DEBOUNCE)) {
    walkRequested = true;  
  }
  
  // If a walk request has been made and the light is green,
  // change the light to yellow after the green light has
  // been active for a third of the duration.
  // When the light turns red, reset the walk request.
  if (walkRequested) {
    if (vehicleLed.getLedPinNumber() == GREEN_PIN && 
        vehicleLed.getActiveTimer() >=
        (GREEN_DURATION / 3)) {
      vehicleLed.resetLed();
      vehicleLed.setLedPinNumber(YELLOW_PIN);
    } else if (vehicleLed.getLedPinNumber() == RED_PIN) {
      walkRequested = false;
    }
  }
  
  // Activate the vehicle light.
  vehicleLed.showSteadyLed(deltaMillis);
  
  // Cycle through the vehicle and pedestrian lights
  // based on the vehicle lights' timers.
  if (vehicleLed.getLedPinNumber() == GREEN_PIN) { // if green light
    // Activate the pedestrian light (don't walk).
    pedestrianLed.showSteadyLed(deltaMillis);
    
    // If the vehicle light (green) has been active
    // at least its duration, reset it.
    // Set the vehicle light to yellow.
    if (vehicleLed.getActiveTimer() >= GREEN_DURATION) {
      vehicleLed.resetLed();    
      vehicleLed.setLedPinNumber(YELLOW_PIN);
    }  
  } else if (vehicleLed.getLedPinNumber() == YELLOW_PIN) { // if yellow light
    // If the vehicle light (yellow) has been active for
    // at least its duration, reset it and the pedestrian
    // light (don't walk).
    // Set the vehicle light to red.
    // Set the pedestrian light to walk.
    if (vehicleLed.getActiveTimer() >= YELLOW_DURATION) {
      vehicleLed.resetLed();
      pedestrianLed.resetLed();     
      vehicleLed.setLedPinNumber(RED_PIN);
      pedestrianLed.setLedPinNumber(WALK_PIN);
    }
  } else { // if red light
    // If the vehicle light timer is at half of its specified
    // duration, change the pedestrian light to don't walk.
    if (pedestrianLed.getLedPinNumber() == WALK_PIN && 
          vehicleLed.getActiveTimer() >=
          (RED_DURATION / 2)) {
      pedestrianLed.resetLed();
      pedestrianLed.setLedPinNumber(NO_WALK_PIN);
    }
    
    if (pedestrianLed.getLedPinNumber() == WALK_PIN) {
      // Activate the pedestrian light (walk).
      pedestrianLed.showSteadyLed(deltaMillis);
    } else {
      // Activate the pedestrian light (don't walk) with 
      // blinking warning.
      pedestrianLed.showBlinkingLed(deltaMillis, 
                                      WARNING_BLINK_INTERVAL); 
    }
    
    // If the vehicle light (red) has been active for at least
    // its duration, reset it. Set the vehicle light to green.
    if (vehicleLed.getActiveTimer() >= RED_DURATION) {
      vehicleLed.resetLed(); 
      vehicleLed.setLedPinNumber(GREEN_PIN);
    }
  }
  
  // The current time should now be the previous 
  // time in the next loop.
  previousMillis = currentMillis; 
}
