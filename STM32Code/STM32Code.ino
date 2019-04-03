
#include "RS485Lib.cpp"
#include <TaskScheduler.h>
#include "LightController.cpp"
#include "PresenceSensor.cpp"


#define ALARM_PIN PB14
#define MASTER_SLAVE_PIN PA0
#define LEDSequencePeriod 125
#define SensorCheckPeriod 1000
#define RS485CommandCheckPeriod 5000
#define RS485CommandSendPeriod 1000

//Task List
void f_EvaluateSensor();
void f_LEDExecuteOnSensor();
void f_RS485_CommandCheck();
void f_RS485_CommandSend();

Task t_SensorCheck(1000, TASK_FOREVER, &f_EvaluateSensor); //Run check every Second
Task t_RunLED(125, TASK_FOREVER, &f_LEDExecuteOnSensor);//run every 125 ms
Task t_RS485_Recieved_Command_Check(RS485CommandCheckPeriod, TASK_FOREVER, &f_RS485_CommandCheck);
Task t_RS485_Send_Command(RS485CommandCheckPeriod, TASK_FOREVER, &f_RS485_CommandSend);

Scheduler runner;

boolean role = false;


RS485 myRS485;
LightController myLightController;
PresenceSensor myPresenceSensor;



void setup() {

  pinMode(MASTER_SLAVE_PIN, INPUT);
  Serial.begin(115200);
  pinMode(ALARM_PIN, OUTPUT);
  role = digitalRead(MASTER_SLAVE_PIN);
  myRS485.setMasterSlaveStatus(role);

  runner.addTask(t_SensorCheck);
  t_SensorCheck.enable();


  runner.addTask(t_RunLED);
  t_RunLED.enable();


  runner.addTask(t_RS485_Recieved_Command_Check);
  t_RunLED.enable();
  runner.addTask(t_RS485_Send_Command);



}



void loop()
{
  /*myRS485.resetRS485SendingPacket();
    myRS485.setSendingArrayData(0,RS485_KEEP_ALIVE,0);
    myRS485.sendRS485Packet();*/
  runner.execute();
}

void f_EvaluateSensor()
{
  myPresenceSensor.evaluateSensorState();
}


void f_LEDExecuteOnSensor()
{
  if (myPresenceSensor.getHumanDetected())
  {
    myLightController.turnOnLED();
  }
  else
  {
    myLightController.shutDownLights();
  }

}
void f_RS485_CommandCheck()
{
  myRS485.recieveRS485Packet();
  if (myRS485.getAmIAddressed())// this means the packet is intended for me. 
  {
    t_RS485_Send_Command.enable(); // enable the Task that sends the response, respond by sending an appropriate message
  }
  else
  {
    t_RS485_Send_Command.disable();// dont respond
  }
}
void f_RS485_CommandSend()
{

}
