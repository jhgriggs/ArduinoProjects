/**
 * TrafficLights.ino
 *
 * This program is a simple simulation of automatic
 * traffic lights. The pedestrian lights are controlled
 * by the timers of the vehicle lights.
 *
 * Vehicle lights: red, yellow, green.
 * Pedestrian lights: walk, blinking warning, don't walk.
 * The pedestrian lights in this case are set so that
 * "walk" light corresponds to a green LED, and that the 
 * "blinking warning" and "don't walk" lights use a red LED. 
 *
 * This program uses the DigitalLed library, which can be 
 * downloaded from the jhgriggs/ArduinoLibraries repository.
 *
 * See the corresponding TrafficLights.fzz Fritzing file for
 * the breadboard layout and schematic.
 *
 * @author Janette H. Griggs
 * @version 1.1 03/29/14
 */

#include <DigitalLed.h>
#include <TimedDigitalLed.h>

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
const unsigned int GREEN_DURATION = 10000;

// Blink interval for blinking warning.
const unsigned int WARNING_BLINK_INTERVAL = 500;

// Create vehicle traffic lights.
TimedDigitalLed redLed(RED_PIN, RED_DURATION); // red
TimedDigitalLed yellowLed(YELLOW_PIN, YELLOW_DURATION); // yellow
TimedDigitalLed greenLed(GREEN_PIN, GREEN_DURATION); // green

// Create pedestrian traffic lights.
DigitalLed walkLed(WALK_PIN); // walk
DigitalLed noWalkLed(NO_WALK_PIN); // don't walk

// Create other variables.
TimedDigitalLed* pVehicleLed; // current vehicle light pointer
DigitalLed* pPedestrianLed; // current pedestrian light pointer
unsigned long previousMillis; // previous time
unsigned long deltaMillis; // change in time

void setup() {
  // Set initial vehicle light to green.
  pVehicleLed = &greenLed;
  // Set initial pedestrian light to don't walk.
  pPedestrianLed = &noWalkLed;
  // Set time variables to 0.
  previousMillis = 0L;
  deltaMillis = 0L;
}

void loop() {
  // Get the current time.
  unsigned long currentMillis = millis();
  // Calculate the change in time from the previous loop time.
  deltaMillis = currentMillis - previousMillis;
  
  // Cycle through the vehicle and pedestrian lights
  // based on the vehicle lights' timers.
  if (pVehicleLed == &greenLed) { // if green light
    // Activate the vehicle light (green).
    pVehicleLed->activateLed(deltaMillis);
    // Activate the pedestrian light (don't walk).
    pPedestrianLed->activateLed(deltaMillis);
    
    // If the vehicle light (green) is no longer active,
    // reset it.
    // Set the vehicle light to yellow.
    if (!(pVehicleLed->getIsActiveState())) {
      pVehicleLed->resetLed();    
      pVehicleLed = &yellowLed;
    }
    
  } else if (pVehicleLed == &yellowLed) { // if yellow light
    // Activate the vehicle light (yellow).
    pVehicleLed->activateLed(deltaMillis);
    
    // If the vehicle light (yellow) is no longer active,
    // reset it and the pedestrian light (don't walk).
    // Set the vehicle light to red.
    // Set the pedestrian light to walk.
    if (!(pVehicleLed->getIsActiveState())) {
      pVehicleLed->resetLed();
      pPedestrianLed->resetLed();     
      pVehicleLed = &redLed;
      pPedestrianLed = &walkLed;
    }
  } else { // if red light
    // Activate the vehicle light (red).
    pVehicleLed->activateLed(deltaMillis);
    // Activate the pedestrian light (walk).
    pPedestrianLed->activateLed(deltaMillis);
    
    // If the vehicle light timer is at half of its specified
    // duration, change the pedestrian light to don't walk
    // with a blinking warning.
    if (pVehicleLed->getActiveTimer() >=
          (pVehicleLed->getActiveDuration() / 2)) {
      if (pPedestrianLed == &walkLed) {
        pPedestrianLed->resetLed();
        pPedestrianLed = &noWalkLed;
      }
      pPedestrianLed->blinkLed(deltaMillis, WARNING_BLINK_INTERVAL);   
    }
    
    // If the vehicle light (red) is no longer active,
    // reset it and stop the blinking warning.
    // Set the vehicle light to green.
    if (!(pVehicleLed->getIsActiveState())) {
      pVehicleLed->resetLed();
      pPedestrianLed->stopBlinkingLed();     
      pVehicleLed = &greenLed;
    }
  }
  
  // The current time should now be the previous 
  // time in the next loop.
  previousMillis = currentMillis; 
}
