/**
 * ColorChangingLight.ino
 *
 * @author Janette H. Griggs
 * @version alpha v1.0 04/08/14
 */
 
#include <MemoryFree.h>
#include <PushButton.h>
#include <ResistorMode.h>
#include <AnalogRGBLed.h>
#include <LedType.h>

void setColor();
void showColor();

enum Color {RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, COLOR_COUNT};
enum ColorChangeMode {NORMAL, GRADUAL, BLINKING, FADING_IN_OUT, COLOR_CHANGE_MODE_COUNT};

const unsigned long COLOR_CHANGE_INTERVAL = 5000L;
const unsigned long BLINK_INTERVAL = 1000L;
const unsigned long FADE_INTERVAL = 2500L;
const byte MAX_BRIGHTNESS = 252;
const byte GRADUAL_CHANGE_STEPS = 3;
const unsigned long GRADUAL_CHANGE_INTERVAL = (COLOR_CHANGE_INTERVAL / 2) / 
                                              (MAX_BRIGHTNESS / 2 / GRADUAL_CHANGE_STEPS);
const LedType LED_TYPE = COMMON_ANODE;
const int RED_PIN = 11;
const int GREEN_PIN = 10;
const int BLUE_PIN = 9;
const ResistorMode BUTTON_RESISTOR = PULL_UP;
const int BUTTON_PIN = 2;
unsigned long BUTTON_DEBOUNCE = 50L;

byte redBrightness = MAX_BRIGHTNESS;
byte greenBrightness = MAX_BRIGHTNESS;
byte blueBrightness = MAX_BRIGHTNESS;

AnalogRGBLed rgbLed(RED_PIN, 
                    GREEN_PIN, 
                    BLUE_PIN, 
                    redBrightness, 
                    greenBrightness, 
                    blueBrightness, 
                    LED_TYPE);
                    
PushButton button(BUTTON_PIN, BUTTON_RESISTOR);

byte color; // color
byte colorChangeMode; // color change mode
unsigned long colorChangeTimer; // color change timer
unsigned long gradualChangeTimer; // gradual change timer
unsigned long previousMillis; // previous time
unsigned long deltaMillis; // change in time

void setup() {
  // Set color change mode to 0 (NORMAL). 
  colorChangeMode = NORMAL;
  
  // Set color to 0 (RED).
  color = RED;
  
  // Set time variables to 0.
  colorChangeTimer = 0L;
  gradualChangeTimer = 0L;
  previousMillis = 0L;
  deltaMillis = 0L;
  
  Serial.begin(9600);
}

void loop() {

  // Get the current time.
  unsigned long currentMillis = millis();
  // Calculate the change in time from the previous loop time.
  deltaMillis = currentMillis - previousMillis; 
  // Update the color change timer.
  colorChangeTimer += deltaMillis;

  if (button.detectPush(deltaMillis, BUTTON_DEBOUNCE)) {
    colorChangeMode++;
    if (colorChangeMode >= COLOR_CHANGE_MODE_COUNT) {
      colorChangeMode = NORMAL; 
    }
    
    rgbLed.resetRGBLed();
    color = RED;
    colorChangeTimer = 0L;
    gradualChangeTimer = 0L;
  }
  
  if (colorChangeTimer >= COLOR_CHANGE_INTERVAL) { 
    color++;
    if (color >= COLOR_COUNT) {
      color = RED; 
    }
    
    colorChangeTimer = 0L;
    gradualChangeTimer = 0L;
  }
  
  setColor();
  showColor();
  
  // The current time should now be the previous 
  // time in the next loop.
  previousMillis = currentMillis; 
  
  Serial.println(freeMemory());
}

void setColor() {
  if (color == RED) {
    if (colorChangeMode == GRADUAL 
        && redBrightness < MAX_BRIGHTNESS 
        && blueBrightness > 0) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) {
        redBrightness += GRADUAL_CHANGE_STEPS;
        greenBrightness = 0;    
        blueBrightness -= GRADUAL_CHANGE_STEPS; 
        gradualChangeTimer = 0L;
      } else {
        gradualChangeTimer += deltaMillis;
      }  
    } else {
      redBrightness = MAX_BRIGHTNESS;
      greenBrightness = 0;    
      blueBrightness = 0; 
    }
  } else if (color == YELLOW) {
    if (colorChangeMode == GRADUAL 
        && redBrightness > MAX_BRIGHTNESS / 2 
        && greenBrightness < MAX_BRIGHTNESS / 2) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) {  
        redBrightness -= GRADUAL_CHANGE_STEPS;
        greenBrightness += GRADUAL_CHANGE_STEPS;    
        blueBrightness = 0;    
        gradualChangeTimer = 0L;
      } else {
        gradualChangeTimer += deltaMillis;
      }
    } else {
      redBrightness = MAX_BRIGHTNESS / 2;
      greenBrightness = MAX_BRIGHTNESS / 2;    
      blueBrightness = 0; 
    }
  } else if (color == GREEN) {
    if (colorChangeMode == GRADUAL 
        && redBrightness > 0 
        && greenBrightness < MAX_BRIGHTNESS) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) { 
        redBrightness -= GRADUAL_CHANGE_STEPS;
        greenBrightness += GRADUAL_CHANGE_STEPS;    
        blueBrightness = 0;   
        gradualChangeTimer = 0L;
      } else {
        gradualChangeTimer += deltaMillis;
      } 
    } else {
      redBrightness = 0;
      greenBrightness = MAX_BRIGHTNESS;    
      blueBrightness = 0; 
    }
  } else if (color == CYAN) {
    if (colorChangeMode == GRADUAL 
        && greenBrightness > MAX_BRIGHTNESS / 2 
        && blueBrightness < MAX_BRIGHTNESS / 2) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) {
        redBrightness = 0;
        greenBrightness -= GRADUAL_CHANGE_STEPS;    
        blueBrightness += GRADUAL_CHANGE_STEPS;  
        gradualChangeTimer = 0L;  
      } else {
        gradualChangeTimer += deltaMillis;
      }
    } else {
      redBrightness = 0;
      greenBrightness = MAX_BRIGHTNESS / 2;    
      blueBrightness = MAX_BRIGHTNESS / 2; 
    }
  } else if (color == BLUE) {
    if (colorChangeMode == GRADUAL 
        && greenBrightness > 0 
        && blueBrightness < MAX_BRIGHTNESS) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) {  
        redBrightness = 0;
        greenBrightness -= GRADUAL_CHANGE_STEPS;    
        blueBrightness += GRADUAL_CHANGE_STEPS;  
        gradualChangeTimer = 0L;
      } else {
        gradualChangeTimer += deltaMillis;
      } 
    } else {
      redBrightness = 0;
      greenBrightness = 0;    
      blueBrightness = MAX_BRIGHTNESS; 
    }
  } else if (color == MAGENTA) {
    if (colorChangeMode == GRADUAL 
        && redBrightness < MAX_BRIGHTNESS / 2 
        && blueBrightness > MAX_BRIGHTNESS / 2) {
      if (gradualChangeTimer >= GRADUAL_CHANGE_INTERVAL) {
        redBrightness += GRADUAL_CHANGE_STEPS;
        greenBrightness = 0;    
        blueBrightness -= GRADUAL_CHANGE_STEPS;  
        gradualChangeTimer = 0L;  
      } else {
        gradualChangeTimer += deltaMillis;
      }
    } else {
      redBrightness = MAX_BRIGHTNESS / 2;
      greenBrightness = 0;    
      blueBrightness = MAX_BRIGHTNESS / 2; 
    }
  }
  
  rgbLed.setRGBColor(redBrightness, greenBrightness, blueBrightness);
}

void showColor() {
  if (colorChangeMode == NORMAL || colorChangeMode == GRADUAL ) {
    rgbLed.showSteadyRGBLed(deltaMillis);  
  } else if (colorChangeMode == BLINKING) {
    rgbLed.showBlinkingRGBLed(deltaMillis, BLINK_INTERVAL);
  } else if (colorChangeMode == FADING_IN_OUT) {
    rgbLed.showFadingInOutRGBLed(deltaMillis, FADE_INTERVAL); 
  }
}
