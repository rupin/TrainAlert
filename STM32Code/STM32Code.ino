
#include "RS485.cpp"
#include <TaskScheduler.h>
#include "LightController.cpp"
#include "PresenceSensor.cpp"
#include "Node.cpp"


#define ALARM_PIN PB14
#define MASTER_SLAVE_PIN PA0


#define LEDSequencePeriod 125
#define SensorCheckPeriod 1000


#define SlaveRS485CommandCheckPeriod 100
#define SlaveRS485CommandSendPeriod 500

#define MasterRS485SlaveAckowledgeCheck 100
#define MasterRS485DataRequestPeriod 1000

#define NUMBER_OF_SLAVE_DEVICES 4
#define SENSOR_QUERY_PHASE 1
#define DEAD_SLAVE_TIMEOUT 5000


uint8_t currentQueriedSlaveAddress = 0;
uint8_t currentPhase = SENSOR_QUERY_PHASE;
Node slaveNode[NUMBER_OF_SLAVE_DEVICES];




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
Task t_RS485_Slave_Dead_TimeOut(3000, TASK_FOREVER, &f_RS485_Process_Slave_Ack);



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



void setup() {

  pinMode(MASTER_SLAVE_PIN, INPUT);
  Serial.begin(115200);
  HardwareSerial SerialObject(USART3);
  myRS485.initClassObject(SerialObject);
  pinMode(ALARM_PIN, OUTPUT);
  role = digitalRead(MASTER_SLAVE_PIN);
  myRS485.setMasterSlaveStatus(role);

  if (role == SLAVE_ROLE) // Defined in RS485Lib
  {
    Serial.print("I am a Slave with Address: ");
    Serial.println(MY_RS485_ADDRESS);
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
    Serial.print("I am a Master with Address: ");
    Serial.println(MY_RS485_ADDRESS);
    runner.addTask(t_SensorCheck);
    runner.addTask(t_SensorCheck);
    t_SensorCheck.enable();

    runner.addTask(t_RunLED);
    t_RunLED.enable();

    runner.addTask(t_RS485_Request_Slave_Data);
    //t_RS485_Request_Slave_Data.enable();

    runner.addTask(t_RS485_Process_Slave_Ack); // runs fully when new data arrives at Master.
    t_RS485_Process_Slave_Ack.enable();

  }

}



void loop()
{
  t_RS485_Request_Slave_Data.enable();
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

void f_RS485_Process_Master_Sent_Command()
{
  //Serial.println("Waiting for Request");
  myRS485.recieveRS485Packet();// This just checks if a packet is recieved, and it is addressed to me.
  //Serial.println(myRS485.getAmIAddressed());
  if (myRS485.getAmIAddressed() == RECIEVED_DATA_FOR_ME) // this means the packet is intended for me.
  {
    Serial.println("I am Addressed");
    t_RS485_Send_Slave_Response.enable();
    t_RS485_Send_Slave_Response.restart(); // enable the Task that sends the response, respond by sending an appropriate message
  }

}


void f_RS485_Send_Slave_Response()
{
  Serial.println("Responding");
  uint8_t destinationAdd = myRS485.getSourceAddress(); //When sending, the source which sent now becomes the destination
  uint8_t commandRecieved = myRS485.getCommand();
  Serial.println(destinationAdd);

  uint32_t dataToBeSent = 0;
  uint8_t sensorStatus ;
  uint8_t batteryState ;
  uint8_t temperature ;
  switch (commandRecieved)
  {
    case RS485_SLAVE_SENSOR_STATUS:
      Serial.println("RS485_SLAVE_SENSOR_STATUS");
      dataToBeSent = 0;
      sensorStatus = myPresenceSensor.getPIRStatus();
      batteryState = 33;
      temperature = 25;

      dataToBeSent = dataToBeSent + sensorStatus;
      dataToBeSent = dataToBeSent << 8;
      dataToBeSent = (dataToBeSent + batteryState);
      dataToBeSent = dataToBeSent << 8;
      dataToBeSent = (dataToBeSent + temperature);

      myRS485.setSendingArrayData(destinationAdd, RS485_SLAVE_SENSOR_STATUS, 8473);
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
  {
    currentQueriedSlaveAddress = currentQueriedSlaveAddress + 1;
    if (currentQueriedSlaveAddress > NUMBER_OF_SLAVE_DEVICES)
    {
      //We have checked up on every slave and updated the slaveNode object array.
      //Let us analyse the data recieved from Slaves which are alive
      //Basically, let us enable a task which will do the analysis
      //this is also the right place to switch phases.
      currentQueriedSlaveAddress = 1;
    }

    uint32_t dataToBeSent = 0;
    myRS485.setSendingArrayData(currentQueriedSlaveAddress, RS485_SLAVE_SENSOR_STATUS, dataToBeSent);
    myRS485.sendRS485Packet();
  }


}

void f_RS485_Process_Slave_Ack()
{

  if (currentPhase == SENSOR_QUERY_PHASE)
  {
    myRS485.recieveRS485Packet();// This just checks if a packet is recieved, and it is addressed to me.
    slaveNode[currentQueriedSlaveAddress].setAliveStatus(false); // Set it to false, if the response comes it will set itself to true.
    if (myRS485.getAmIAddressed())// this means the packet is intended for me.
    {
      uint8_t sourceAddress = myRS485.getSourceAddress();
      if (sourceAddress == currentQueriedSlaveAddress)
      {
        //currentQueriedSlaveAddress-1 becuse array starts at 0, but slave addressing starts at 1, because Master Node is
        slaveNode[currentQueriedSlaveAddress - 1].setAliveStatus(true); //coz it responded
        slaveNode[currentQueriedSlaveAddress - 1].setBatteryHealth(myRS485.getData(BATTERY_HEALTH_INDEX));
        slaveNode[currentQueriedSlaveAddress - 1].setPIRStatus(myRS485.getData(PIR_STATUS_INDEX));
        slaveNode[currentQueriedSlaveAddress - 1].setTemperature(myRS485.getData(TEMPERATURE_BYTE_INDEX));
        t_RS485_Request_Slave_Data.enable();
        t_RS485_Request_Slave_Data.restart();
      }

    }
    else
    {
      t_RS485_Request_Slave_Data.disable();// dont respond
    }
  }
}
