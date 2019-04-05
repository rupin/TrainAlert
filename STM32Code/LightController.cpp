<<<<<<< HEAD
#include <Arduino.h>
#define LED_COUNT 4
#define LED_SWITCHING_TIME 125

class LightController
{

  private:
    int LED_PINS[LED_COUNT] = {PB4, PB3, PA15, PA8};   
    bool debugLEDstate = true;
    uint8_t currentLEDIndex=0;
    boolean lightsOn=false;

  public:

    LightController()
    {
      uint8_t loopIndex;
      for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
      {
        pinMode(LED_PINS[loopIndex], OUTPUT);
      }
      pinMode(LED_BUILTIN, OUTPUT);
    }

    void turnOnLED()
    {
        digitalWrite(LED_PINS[currentLEDIndex], LOW); //Set the LED in sequence to low, this will turn it off. the current LEd has stayed on for "waitTime' milliseconds
        currentLEDIndex = currentLEDIndex + 1; //increment LEDSequenceIndex by 1
        currentLEDIndex = currentLEDIndex % LED_COUNT; // Limit the sequence index to less than the number of LED's in the system.
        digitalWrite(LED_PINS[currentLEDIndex], HIGH); //Set the next LED in sequence to high, this will turn it on.
        debugLEDstate = !debugLEDstate;
        digitalWrite(LED_BUILTIN, debugLEDstate);
        lightsOn=true;      
    }

    

    void shutDownLights()
    {
      if(!lightsOn)
      {
        return;
      }
      
      uint8_t loopIndex;
      for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
      {
        digitalWrite(LED_PINS[loopIndex], LOW);
      }
      digitalWrite(LED_BUILTIN, HIGH);// BuiltIn LED is Active Low
      lightsOn=false;
    }

    

};
=======
#include <Arduino.h>
#define LED_COUNT 4
#define LED_SWITCHING_TIME 125

class LightController
{

  private:
    int LED_PINS[LED_COUNT] = {PB4, PB3, PA15, PA8};   
    bool debugLEDstate = true;
    uint8_t currentLEDIndex=0;
    boolean lightsOn=false;

  public:

    LightController()
    {
      uint8_t loopIndex;
      for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
      {
        pinMode(LED_PINS[loopIndex], OUTPUT);
      }
      pinMode(LED_BUILTIN, OUTPUT);
    }

    void turnOnLED()
    {
        digitalWrite(LED_PINS[currentLEDIndex], LOW); //Set the LED in sequence to low, this will turn it off. the current LEd has stayed on for "waitTime' milliseconds
        currentLEDIndex = currentLEDIndex + 1; //increment LEDSequenceIndex by 1
        currentLEDIndex = currentLEDIndex % LED_COUNT; // Limit the sequence index to less than the number of LED's in the system.
        digitalWrite(LED_PINS[currentLEDIndex], HIGH); //Set the next LED in sequence to high, this will turn it on.
        debugLEDstate = !debugLEDstate;
        digitalWrite(LED_BUILTIN, debugLEDstate);
        lightsOn=true;      
    }

    

    void shutDownLights()
    {
      if(!lightsOn)
      {
        return;
      }
      
      uint8_t loopIndex;
      for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
      {
        digitalWrite(LED_PINS[loopIndex], LOW);
      }
      digitalWrite(LED_BUILTIN, HIGH);// BuiltIn LED is Active Low
      lightsOn=false;
    }

    

};
>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c
