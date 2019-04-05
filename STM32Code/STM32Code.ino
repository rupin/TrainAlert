
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

<<<<<<< HEAD

#define LEDSequencePeriod 125
#define SensorCheckPeriod 1000


#define SlaveRS485CommandCheckPeriod 500
#define SlaveRS485CommandSendPeriod 500

#define MasterRS485SlaveAckowledgeCheck 100
#define MasterRS485DataRequestPeriod 100

//The operational Phases a Master can be in.
#define SLAVE_ENUMERATION_PHASE 0

uint8_t numberOfSlaveDevices = 0;
uint8_t currentQueriedSlaveAddress = 0;
#define SENSOR_QUERY_PHASE 1
uint8_t currentPhase = SLAVE_ENUMERATION_PHASE;


//Common Task List between Slave and Master
void f_EvaluateSensor();
void f_LEDExecuteOnSensor();
Task t_SensorCheck(1000, TASK_FOREVER, &f_EvaluateSensor); //Run check every Second
Task t_RunLED(125, TASK_FOREVER, &f_LEDExecuteOnSensor);//run every 125 ms

//Master Task and Process List
void f_RS485_Process_Slave_Ack();
void f_RS485_Request_Slave_Data();
Task t_RS485_Request_Slave_Data(MasterRS485DataRequestPeriod, 1, &f_RS485_Request_Slave_Data); //Should happen only once
Task t_RS485_Process_Slave_Ack(MasterRS485SlaveAckowledgeCheck, TASK_FOREVER, &f_RS485_Process_Slave_Ack);



//Slave Task and Process List
void f_RS485_Process_Master_Sent_Command();
void f_RS485_Send_Slave_Response();
Task t_RS485_Process_Master_Sent_Command(SlaveRS485CommandCheckPeriod, TASK_FOREVER, &f_RS485_Process_Master_Sent_Command);
Task t_RS485_Send_Slave_Response(SlaveRS485CommandSendPeriod, 1, &f_RS485_Send_Slave_Response); //Should happen only once

Scheduler runner;

boolean role = false;


RS485 myRS485;
LightController myLightController;
PresenceSensor myPresenceSensor;

=======
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

>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c


void setup() {

  pinMode(MASTER_SLAVE_PIN, INPUT);
  Serial.begin(115200);
<<<<<<< HEAD

  pinMode(ALARM_PIN, OUTPUT);
  role = digitalRead(MASTER_SLAVE_PIN);
  myRS485.setMasterSlaveStatus(role);

  if (role == SLAVE_ROLE) // Defined in RS485Lib
  {
    runner.addTask(t_SensorCheck);
    t_SensorCheck.enable();

    runner.addTask(t_RunLED);
    t_RunLED.enable();

    runner.addTask(t_RS485_Process_Master_Sent_Command);
    t_RS485_Process_Master_Sent_Command.enable();

    runner.addTask(t_RS485_Send_Slave_Response);
  }

  if (role == MASTER_ROLE) // Defined in RS485Lib
  {
    runner.addTask(t_SensorCheck);
    t_SensorCheck.enable();

    runner.addTask(t_RunLED);
    t_RunLED.enable();

    runner.addTask(t_RS485_Request_Slave_Data);
    //t_RS485_Request_Slave_Data.enable();

    runner.addTask(t_RS485_Process_Slave_Ack); // runs fully when new data arrives at Master.
    t_RS485_Process_Slave_Ack.enable();

  }










=======
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


>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c

}



void loop()
{
<<<<<<< HEAD

=======
  /*myRS485.resetRS485SendingPacket();
    myRS485.setSendingArrayData(0,RS485_KEEP_ALIVE,0);
    myRS485.sendRS485Packet();*/
>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c
  runner.execute();
}

void f_EvaluateSensor()
{
  myPresenceSensor.evaluateSensorState();
}


void f_LEDExecuteOnSensor()
{
  if (myPresenceSensor.getHumanDetected())
<<<<<<< HEAD
  {
    myLightController.turnOnLED();
  }
  else
  {
=======
  {
    myLightController.turnOnLED();
  }
  else
  {
>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c
    myLightController.shutDownLights();
  }

}
<<<<<<< HEAD

void f_RS485_Process_Master_Sent_Command()
{
  myRS485.recieveRS485Packet();// This just checks if a packet is recieved, and it is addressed to me.
  if (myRS485.getAmIAddressed())// this means the packet is intended for me.
  {
    t_RS485_Send_Slave_Response.enable(); // enable the Task that sends the response, respond by sending an appropriate message
  }
  else
  {
    t_RS485_Send_Slave_Response.disable();// dont respond
  }
}


void f_RS485_Send_Slave_Response()
{
  uint8_t destinationAdd = myRS485.getSourceAddress(); //When sending, the source which sent now becomes the destination
  uint8_t commandRecieved = myRS485.getCommand();
  uint32_t dataToBeSent = 0;
  uint8_t sensorStatus ;
  uint8_t batteryState ;
  uint8_t temperature ;
  switch (commandRecieved)
  {
    case RS485_SLAVE_SENSOR_STATUS:
      dataToBeSent = 0;
      sensorStatus = myPresenceSensor.getPIRStatus();
      batteryState = 33;
      temperature = 25;

      dataToBeSent = (dataToBeSent | sensorStatus);
      dataToBeSent = dataToBeSent << 8;
      dataToBeSent = (dataToBeSent | batteryState);
      dataToBeSent = dataToBeSent << 8;
      dataToBeSent = (dataToBeSent | temperature);

      myRS485.setSendingArrayData(destinationAdd, RS485_SLAVE_SENSOR_STATUS, dataToBeSent);
      myRS485.sendRS485Packet();
      break;


    case RS485_RADIO_STATUS:
      dataToBeSent = 0;
      myRS485.setSendingArrayData(destinationAdd, RS485_RADIO_STATUS, dataToBeSent);
      myRS485.sendRS485Packet();
      break;
    case RS485_TRAIN_ALERT:
      dataToBeSent = 44;
      myRS485.setSendingArrayData(destinationAdd, RS485_TRAIN_ALERT, dataToBeSent);
      myRS485.sendRS485Packet();
      break;
    case RS485_HUMAN_ALERT:
      dataToBeSent = 44;
      myRS485.setSendingArrayData(destinationAdd, RS485_HUMAN_ALERT, dataToBeSent);
      myRS485.sendRS485Packet();
      break;
    default:
      break;

  }



}




void f_RS485_Request_Slave_Data()
{
  if (SENSOR_QUERY_PHASE)
    currentQueriedSlaveAddress = currentQueriedSlaveAddress + 1;
  if (currentQueriedSlaveAddress > numberOfSlaveDevices)
  {
    currentQueriedSlaveAddress = 1;
  }
  uint32_t dataToBeSent = 0;
  myRS485.setSendingArrayData(currentQueriedSlaveAddress, RS485_SLAVE_SENSOR_STATUS, dataToBeSent);
  myRS485.sendRS485Packet();
=======
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
>>>>>>> 09cd5fffcd983c165a5e5fa0fc6d5c30f646fc3c


}

void f_RS485_Process_Slave_Ack()
{
  if (currentPhase == SLAVE_ENUMERATION_PHASE)
  {
    myRS485.recieveRS485Packet();// This just checks if a packet is recieved, and it is addressed to me.
    if (myRS485.getAmIAddressed())// this means the packet is intended for me.
    {
      t_RS485_Request_Slave_Data.enable(); // enable the Task that sends the response, respond by sending an appropriate message
    }
    else
    {
      t_RS485_Request_Slave_Data.disable();// dont respond
    }
  }
}
