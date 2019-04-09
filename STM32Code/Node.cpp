#include <Arduino.h>
//This class is instantiated for the Master Only. 
//Each slave will have an instance of this class, so that the master can keep a log of all slave Device
class Node{
  private:
  uint8_t batteryHealth;
  uint8_t PIRStatus;
  boolean isAlive;
  uint8_t temperature;

  public:

  Node()
  {
    batteryHealth=0;
    PIRStatus=0;
    isAlive=true;
    temperature=0;
  }
  void setBatteryHealth(uint8_t lBatteryHealth)
  {
    batteryHealth=lBatteryHealth;
  }
  uint8_t getBatteryHealth()
  {
    return batteryHealth;
  }

  
  void setPIRStatus(uint8_t lPIRStatus)
  {
    PIRStatus=lPIRStatus;
  }
  uint8_t getPIRstatus()
  {
    return PIRStatus;
  }
  void setAliveStatus(boolean lAliveStatus)
  {
    isAlive=lAliveStatus;
  }
  boolean getAliveStatus()
  {
    return isAlive;
  }

  void setTemperature(uint8_t lTemperature)
  {
    temperature=lTemperature;
  }
  uint8_t getTemperature()
  {
    return temperature;
  }
  
  
};
