/**
 * TrafficLightsButton.ino
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
 * See the corresponding TrafficLightsButton.fzz Fritzing file 
 * for the breadboard layout and schematic.
 *
 * @author Janette H. Griggs
 * @version 1.2 03/29/14
 */

#include <DigitalLed.h>
#include <TimedDigitalLed.h>
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

// Create vehicle traffic lights.
TimedDigitalLed redLed(RED_PIN, RED_DURATION); // red
TimedDigitalLed yellowLed(YELLOW_PIN, YELLOW_DURATION); // yellow
TimedDigitalLed greenLed(GREEN_PIN, GREEN_DURATION); // green

// Create pedestrian traffic lights.
DigitalLed walkLed(WALK_PIN); // walk
DigitalLed noWalkLed(NO_WALK_PIN); // don't walk

// Create walk request button.
PushButton walkButton(WALK_BUTTON_PIN, WALK_BUTTON_RESISTOR, 
                      WALK_BUTTON_DEBOUNCE);

// Create other variables.
TimedDigitalLed* pVehicleLed; // current vehicle light pointer
DigitalLed* pPedestrianLed; // current pedestrian light pointer
unsigned long previousMillis; // previous time
unsigned long deltaMillis; // change in time
boolean walkRequested; // walk request status

void setup() {
  // Set initial vehicle light to green.
  pVehicleLed = &greenLed;
  // Set initial pedestrian light to don't walk.
  pPedestrianLed = &noWalkLed;
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
  if (!walkRequested && walkButton.detectPush(deltaMillis)) {
    walkRequested = true;  
  }
  
  // If a walk request has been made and the light is green,
  // change the light to yellow after the green light has
  // been active for a third of the duration.
  // When the light turns red, reset the walk request.
  if (walkRequested) {
    if (pVehicleLed == &greenLed && 
        pVehicleLed->getActiveTimer() >=
        (pVehicleLed->getActiveDuration() / 3)) {
      pVehicleLed->resetLed();
      pVehicleLed = &yellowLed;
    } else if (pVehicleLed == &redLed) {
      walkRequested = false;
    }
  }
  
  // Cycle through the vehicle and pedestrian lights
  // based on the vehicle lights' timers.
  if (pVehicleLed == &greenLed) { // if green light
    // Activate the vehicle light (green).
    pVehicleLed->runSteadyLed(deltaMillis);
    // Activate the pedestrian light (don't walk).
    pPedestrianLed->runSteadyLed(deltaMillis);
    
    // If the vehicle light (green) is no longer active,
    // reset it.
    // Set the vehicle light to yellow.
    if (!(pVehicleLed->getIsActiveState())) {
      pVehicleLed->resetLed();    
      pVehicleLed = &yellowLed;
    }  
  } else if (pVehicleLed == &yellowLed) { // if yellow light
    // Activate the vehicle light (yellow).
    pVehicleLed->runSteadyLed(deltaMillis);
    
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
    pVehicleLed->runSteadyLed(deltaMillis);
    
    // If the vehicle light timer is at half of its specified
    // duration, change the pedestrian light to don't walk.
    if ((pPedestrianLed == &walkLed) && 
          pVehicleLed->getActiveTimer() >=
          (pVehicleLed->getActiveDuration() / 2)) {
      pPedestrianLed->resetLed();
      pPedestrianLed = &noWalkLed;
    }
    
    if (pPedestrianLed == &walkLed) {
      // Activate the pedestrian light (walk).
      pPedestrianLed->runSteadyLed(deltaMillis);
    } else {
      // Activate the pedestrian light (don't walk) with blinking warning.
      pPedestrianLed->runBlinkingLed(deltaMillis, WARNING_BLINK_INTERVAL); 
    }
    
    // If the vehicle light (red) is no longer active,
    // reset it. Set the vehicle light to green.
    if (!(pVehicleLed->getIsActiveState())) {
      pVehicleLed->resetLed(); 
      pVehicleLed = &greenLed;
    }
  }
  
  // The current time should now be the previous 
  // time in the next loop.
  previousMillis = currentMillis; 
}
