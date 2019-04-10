
#include <Arduino.h>
#include <HardwareSerial.h>

#define RS485_DIR_PIN PB1 // Direction Pin for MAX485
#define SENDING HIGH
#define RECIEVING LOW


#define MASTER_ROLE 1
#define SLAVE_ROLE 0


//#define MASTER_RS485_ADDRESS 0
#define MY_RS485_ADDRESS 2
#define RS485_PACKET_SIZE 11
#define RS485_DATA_SIZE 4

//Following Flags determine if the data recieved is completed or not, and is it intended for me or no.
#define RECIEVED_DATA_UNSURE 2
#define RECIEVED_DATA_FOR_ME 1
#define RECIEVED_DATA_NOT_FOR_ME 0



#define PACKET_BYTE_UNSET_VALUE 0 //When data in arrays in unset, it will be this value
#define DESTINATION_BYTE_INDEX 3 //Index of the byte in the array which points to recipient address
#define SOURCE_BYTE_INDEX 2 //Index of the byte in the array which points to recipient address
#define COMMAND_BYTE_INDEX 4 //Index of the byte in the array which points to command
#define FIRST_DATA_BYTE 5 //Index of the byte in the array which points to FIRST DATA BYTE
#define PIR_STATUS_INDEX 6 //Index of byte where the status of the PIR Sensor is stored
#define BATTERY_HEALTH_INDEX 7 //Index of the the byte where the health of the Battery is stored
#define TEMPERATURE_BYTE_INDEX 8 //index of the byte where the temperature of the slave is stored



#define RS485_SLAVE_SENSOR_STATUS 0x01 //Combined Report for Sensor Status, Battery and Ambient Temperature 
#define RS485_RADIO_STATUS 0x02 //Report For Slaves to send Radio Data
#define RS485_TRAIN_ALERT 0x03 //Alert slaves of Impending Train
#define RS485_HUMAN_ALERT 0x04 //Alert slaves of Human Presence




class RS485
{
  private:

    /*Packet sequence is as follow
      Byte 0 : 0x00 Start of Packet
      Byte 1: Data Size in Bytes Including Addresses, command, data and CRC
      Byte 2: Address of Sending Node
      Byte 3: Address of Recieving Node
      Byte 4: Command
      Byte 5, 6, 7 and 8: Data
      Byte 9: CRC
      Byte 10: 0xFF End Byte
    */

    uint8_t RS485_SENDING_PAYLOAD [RS485_PACKET_SIZE];
    uint8_t RS485_RECIEVING_PAYLOAD [RS485_PACKET_SIZE];

    HardwareSerial RS485Port = new HardwareSerial(USART3);
    uint8_t amIAddressed = RECIEVED_DATA_NOT_FOR_ME;
    boolean isMaster = false;

    uint8_t sourceAddress, rs485Command, destinationAddress;
    uint32_t recievedData;




    uint8_t isThePacketforMe()//check if the destination Address belongs to me
    {
      uint8_t recievedAddress = getRS485PacketByte(DESTINATION_BYTE_INDEX);
      //Serial.println(recievedAddress);
      if (recievedAddress == MY_RS485_ADDRESS)
      {
        return RECIEVED_DATA_FOR_ME; // Yes the packet is intended for me
      }
      else
      {
        return RECIEVED_DATA_NOT_FOR_ME;// No the packet is not intended for me
      }

    }


  public:

    RS485()
    {
      //do Nothing here

    }

    void initClassObject(HardwareSerial obj )
    {
      //Serial.println("RS485 Object Created");
      RS485Port = obj; //Reinitialise USART3 Object
      RS485Port.begin(9600);
      pinMode(RS485_DIR_PIN, OUTPUT);


    }
    void setMasterSlaveStatus(boolean role)
    {
      isMaster = role;
    }

    boolean getMasterSlaveStatus()
    {
      return isMaster;
    }


    void setRS485Mode(boolean mode)
    {
      digitalWrite(RS485_DIR_PIN, mode);
    }


    void sendRS485Packet()
    {
      setRS485Mode(SENDING);//Change Mode to Sending
      RS485Port.write(RS485_SENDING_PAYLOAD, RS485_PACKET_SIZE); // Serial.write can send the whole array.
      RS485Port.flush();//Wait till all data has been sent.
      setRS485Mode(RECIEVING);//Change back to Recieving
      //Reset amIAddressed to prevent false checks
      amIAddressed = RECIEVED_DATA_NOT_FOR_ME;
    }

    void recieveRS485Packet()
    {
      char c;
      setRS485Mode(RECIEVING);
      //amIAddressed = RECIEVED_DATA_UNSURE; // I dont know if the data is for me or no. I will report I am not sure

      //It is possible the function call can happen while the packet is in transit.
      //We should only read in data lengths equal to RS485_PACKET_SIZE
      uint8_t bufferLength = RS485Port.available();
      Serial.print("BufferLength: ");
      Serial.println(bufferLength);
      if (bufferLength >= RS485_PACKET_SIZE)
      {
        Serial.println("I recieved 11 Bytes");  
        //RS485Port.readBytes(RS485_RECIEVING_PAYLOAD, RS485_PACKET_SIZE);
        uint8_t readingIndex = 0;
        while (readingIndex < RS485_PACKET_SIZE)
        {
          RS485_RECIEVING_PAYLOAD[readingIndex] = RS485Port.read();
          readingIndex = readingIndex + 1;

        }
        Serial.write(RS485_RECIEVING_PAYLOAD, RS485_PACKET_SIZE);
        Serial.flush();

        amIAddressed = isThePacketforMe(); //Determine if I am the intended recipient or
        if (amIAddressed == RECIEVED_DATA_FOR_ME) //If I am the recipent, better collect the source of the packet, the command and the data.
        {
          sourceAddress = RS485_RECIEVING_PAYLOAD[SOURCE_BYTE_INDEX];
          rs485Command = RS485_RECIEVING_PAYLOAD[COMMAND_BYTE_INDEX];


          recievedData = 0;
          recievedData = (recievedData | RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE]);
          recievedData = recievedData << 8;

          recievedData = (recievedData | RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 1]);
          recievedData = recievedData << 8;

          recievedData = (recievedData | RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 2]);
          recievedData = recievedData << 8;

          recievedData = recievedData | RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 3];


          /*RECIEVED_DATA[0]=RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 0];
            RECIEVED_DATA[1]=RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 1];
            RECIEVED_DATA[2]=RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 2];
            RECIEVED_DATA[3]=RS485_RECIEVING_PAYLOAD[FIRST_DATA_BYTE + 3];*/

        }
        /*else//I got enough data, but it wasnt for me. I need to clear out the recieving buffer
        {
          while(RS485Port.available())
          {
            c=RS485Port.read();
            Serial.println("Flushing");
          }
        }*/
        //setRS485Mode(RECIEVING);  
        


      }





    }

    void setRS485SendingPacketByte(uint8_t index, uint8_t value)
    {
      RS485_SENDING_PAYLOAD[index] = value;
    }

    uint8_t getRS485PacketByte(uint8_t index)
    {
      return RS485_RECIEVING_PAYLOAD[index];
    }


    uint8_t getAmIAddressed()
    {
      return amIAddressed;
    }

    /*void resetRS485RecievedPacket()
      {
      uint8_t i;
      for (i = 0; i < RS485_PACKET_SIZE; i++)
      {
        setRS485RecievingPacketByte(i, PACKET_BYTE_UNSET_VALUE);
      }
      }*/



    void setSendingArrayData(uint8_t recipientAddress, uint8_t Command, uint32_t data)
    {
      destinationAddress = recipientAddress;
      uint8_t maskedByte = 0;
      setRS485SendingPacketByte(0, 0x00);
      setRS485SendingPacketByte(1, 0x08);
      setRS485SendingPacketByte(2, MY_RS485_ADDRESS);
      setRS485SendingPacketByte(3, recipientAddress);
      setRS485SendingPacketByte(4, Command);
      uint8_t seq = 8;
      for (seq = 8; seq > 4; seq--)
      {
        maskedByte = (uint8_t)(data & 0x000000FF);
        setRS485SendingPacketByte(seq, maskedByte);
        data = data >> 8;
      }


      /*maskedByte = (uint8_t)(data & 0xFF000000) >> 24;
        setRS485SendingPacketByte(5, maskedByte);

        maskedByte = (uint8_t)(data & 0x00FF0000) >> 16;
        setRS485SendingPacketByte(6, maskedByte);

        maskedByte = (uint8_t)(data & 0x0000FF00) >> 8;
        setRS485SendingPacketByte(7, maskedByte);*/




      setRS485SendingPacketByte(9, 0x00);
      setRS485SendingPacketByte(10, 0x00);
    }

    uint8_t getSourceAddress()
    {
      return sourceAddress;
    }
    uint8_t getCommand()
    {
      return rs485Command;
    }

    uint8_t getData(byte byteSequence)
    {
      return RS485_RECIEVING_PAYLOAD[byteSequence];
    }






};
