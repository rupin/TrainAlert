
#include <Arduino.h>
#include <HardwareSerial.h>

#define RS485_DIR_PIN PB1 // Direction Pin for MAX485
#define SENDING 1
#define RECIEVING 0
#define MY_RS485_ADDRESS 1
#define RS485_PACKET_SIZE 11


#define RS485_KEEP_ALIVE 0x01
#define RS485_SENSOR_STATUS 0x02
#define RS485_RADIO_STATUS 0x03
#define RS485_BATTERY_STATUS 0X04
#define RS485_TRAIN_ALERT 0x05


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
    HardwareSerial RS485Port=new HardwareSerial(USART3);
    
    

  public:
    RS485()
    {
     
      RS485Port.begin(9600);
      pinMode(RS485_DIR_PIN, OUTPUT);
    }
   

    void setRS485Mode(boolean mode)
    {
      digitalWrite(RS485_DIR_PIN, mode);
    }


    void sendRS485PacketToMaster()
    {

      setRS485Mode(SENDING);
      RS485Port.write(RS485_SENDING_PAYLOAD, RS485_PACKET_SIZE); // Serial.write can send the whole array.

    }

    void recieveRS485PacketFromMaster()
    {
      setRS485Mode(RECIEVING);

      if (RS485Port.available())
      {
        RS485Port.readBytes(RS485_RECIEVING_PAYLOAD, RS485_PACKET_SIZE);
      }
    }

};
