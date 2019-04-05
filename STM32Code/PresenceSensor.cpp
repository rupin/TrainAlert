#include <Arduino.h>
#define SENSOR_COUNT 4

class PresenceSensor
{
  private:
    int PIR_PINS[SENSOR_COUNT] = {PB9, PB8, PB13, PB12};    
    boolean humanDetected;
    uint8_t PIRstatus=0;

    
  public:
    PresenceSensor()
    {
      uint8_t loopIndex;
      for (loopIndex = 0; loopIndex < SENSOR_COUNT; loopIndex++)
      {
        pinMode(PIR_PINS[loopIndex], INPUT);
      }
      humanDetected = false;

    }

    void setHumanDetected(bool detectionStatus)
    {
      humanDetected = detectionStatus;
    }
    
    boolean getHumanDetected()
    {
      return humanDetected;
    }
    boolean getPIRStatus()
    {
      return PIRstatus;
    }

    void evaluateSensorState()
    {
      PIRstatus = 0; //Reset the Variable to correct initial State.
      humanDetected = false;
      uint8_t loopIndex;
      
      for (loopIndex = 0; loopIndex < SENSOR_COUNT; loopIndex++)
      {
        byte state = digitalRead(PIR_PINS[loopIndex]); //digital read returns value 0(0b00000000) or 1(0b00000001)
        /*Shift the LSB up, based on the sensor index, append 0 on LSB.
          OR operation just sets the bit which corresponds to the sensor index*/

        PIRstatus = PIRstatus | (state << loopIndex);
      }
      if (PIRstatus > 0)
      {
        humanDetected = true;
      }
    }

};
