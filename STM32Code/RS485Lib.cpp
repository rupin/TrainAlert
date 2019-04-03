
#include <Arduino.h>
#include <HardwareSerial.h>

#define RS485_DIR_PIN PB1 // Direction Pin for MAX485
#define SENDING 1
#define RECIEVING 0
#define MASTER_RS485_ADDRESS 0
#define MY_RS485_ADDRESS 1
#define RS485_PACKET_SIZE 11

//Following Flags determine if the data recieved is completed or not, and is it intended for me or no.
#define RECIEVED_DATA_UNSURE 2
#define RECIEVED_DATA_FOR_ME 1
#define RECIEVED_DATA_NOT_FOR_ME 0



#define PACKET_BYTE_UNSET_VALUE 0 //When data in arrays in unset, it will be this value
#define DESTINATION_BYTE_INDEX 3 //Index of the bytein the arraywhich points to recipient address


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
    HardwareSerial RS485Port = new HardwareSerial(USART3);


    uint8_t isThePacketforMe()//check if the destination Address belongs to me
    {
      uint8_t recievedAddress = getRS485PacketByte(DESTINATION_BYTE_INDEX);
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

      RS485Port.begin(9600);
      pinMode(RS485_DIR_PIN, OUTPUT);
    }


    void setRS485Mode(boolean mode)
    {
      digitalWrite(RS485_DIR_PIN, mode);
    }


    void sendRS485Packet()
    {
      setRS485Mode(SENDING);
      RS485Port.write(RS485_SENDING_PAYLOAD, RS485_PACKET_SIZE); // Serial.write can send the whole array.
    }

    uint8_t recieveRS485Packet()
    {
      setRS485Mode(RECIEVING);
      //It is possible the function call can happen while the packet is in transit. We should only read in data lengths equal to RS485_PACKET_SIZE
      uint8_t amIAddressed = RECIEVED_DATA_UNSURE; // I dont know if the data is for me or no. I will report I am not sure
      if (RS485Port.available() == RS485_PACKET_SIZE)
      {
        RS485Port.readBytes(RS485_RECIEVING_PAYLOAD, RS485_PACKET_SIZE);
        amIAddressed = isThePacketforMe(); //Determine if I am the intended recipient or no
      }

      return amIAddressed;
    }

    void setRS485PacketByte(uint8_t index, uint8_t value)
    {
      RS485_SENDING_PAYLOAD[index] = value;
    }

    uint8_t getRS485PacketByte(uint8_t index)
    {
      return RS485_RECIEVING_PAYLOAD[index];
    }

    void resetRS485SendingPacket()
    {
      uint8_t i;
      for (i = 0; i < RS485_PACKET_SIZE; i++)
      {
        setRS485PacketByte(i, PACKET_BYTE_UNSET_VALUE);
      }
    }

    void setSendingArrayData(uint8_t recipientAddress, uint8_t Command, uint32_t data)
    {
      uint8_t maskedByte=0;
      setRS485PacketByte(0, 0x00);
      setRS485PacketByte(1, 0x08);
      setRS485PacketByte(2, MY_RS485_ADDRESS);
      setRS485PacketByte(3, recipientAddress);
      setRS485PacketByte(4, Command);
      
      maskedByte=(uint8_t)(data &0xFF000000)>>24;
      setRS485PacketByte(5, maskedByte);
      
      maskedByte=(uint8_t)(data &0x00FF0000)>>16;
      setRS485PacketByte(6, maskedByte);
      
      maskedByte=(uint8_t)(data &0x0000FF00)>>8;
      setRS485PacketByte(7, maskedByte);

      maskedByte=(uint8_t)(data &0x000000FF);
      setRS485PacketByte(8, maskedByte);     
      
     
      setRS485PacketByte(9, 0x00);
      setRS485PacketByte(10, 0x00);
    }



};
