
#include "RS485Lib.cpp"


#define LED_COUNT 4
#define PIR_COUNT 4
#define ALARM_PIN PB14
#define MASTER_SLAVE_PIN PA0

int LED_PINS[4] = {PB4, PB3, PA15, PA8};
int PIR_PINS[4] = {PB9, PB8, PB13, PB12};
byte pirStatus = 0;
byte loopIndex = 0;
byte LEDSequenceIndex = 0;
byte stayOnFor = 20000; // Stay on for 20 seconds once triggered.
long int startedSince = 0;
bool debugLEDstate = true;
boolean lightsOn = false;
boolean masterDevice = false;

boolean humanDetected = false;
boolean trainDetected = false;
int waitTime = 125;
long int timeElapsed = 0;


void setup() {

  for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
  {
    pinMode(LED_PINS[loopIndex], OUTPUT);
  }
  for (loopIndex = 0; loopIndex < PIR_COUNT; loopIndex++)
  {
    // The sensor reports a High active state, but sometimes the output can float. Let us set it to a low state.
    pinMode(PIR_PINS[loopIndex], INPUT_PULLUP);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MASTER_SLAVE_PIN, INPUT);
  Serial.begin(115200);
  timeElapsed = millis();


  pinMode(ALARM_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  masterDevice = digitalRead(MASTER_SLAVE_PIN);
  if (!masterDevice)
  {
    Serial.print ("I am a Slave Device with Address: ");
    Serial.println(MY_RS485_ADDRESS);
  }
  else
  {
    Serial.print ("I am a Master Device with Address: ");
    Serial.println(0);
  }
}

//void loop() {
//
//  humanDetected = false; //Set this to false to avoid accidental retriggering
//  pirStatus = 0; //Reset the Variable to correct initial State.
//  for (loopIndex = 0; loopIndex < PIR_COUNT; loopIndex++)
//  {
//    byte state = digitalRead(PIR_PINS[loopIndex]); //digital read returns value 0(0b00000000) or 1(0b00000001)
//    /*Shift the LSB up, based on the sensor index, append 0 on LSB.
//      OR operation just sets the bit which corresponds to the sensor index*/
//
//    pirStatus = pirStatus | (state << loopIndex);
//  }
//  pirStatus = 1;
//  //Serial.print("PIR Status:");
//  //Serial.println(pirStatus);
//  if (pirStatus > 0) // atleast one of the sensor was reporting an active state.
//  {
//    //timeElapsed=millis(); // Set this to current milliseconds time
//    humanDetected = true; //Set variable to true;
//    //startedSince=millis();// Set this to current milliseconds time
//  }
//
//
//  if (humanDetected)
//  {
//    lightsOn = true;
//    startLEDSequence(); //do this till twenty seconds
//
//  }
//  else
//  {
//
//    if (lightsOn) // So that we turn off the lights only once.
//    {
//      shutDownLights();
//      lightsOn = false;
//    }
//  }
//}
RS485 myRS485;
void loop()
{
  myRS485.setRS485Mode(1);
}


void startLEDSequence()
{
  if (millis() - timeElapsed >= waitTime)
  {
    timeElapsed = millis(); //Save the current time
    digitalWrite(LED_PINS[LEDSequenceIndex], LOW); //Set the LED in sequence to low, this will turn it off. the current LEd has stayed on for "waitTime' milliseconds
    LEDSequenceIndex = LEDSequenceIndex + 1; //increment LEDSequenceIndex by 1
    LEDSequenceIndex = LEDSequenceIndex % LED_COUNT; // Limit the sequence index to less than the number of LED's in the system.
    digitalWrite(LED_PINS[LEDSequenceIndex], HIGH); //Set the next LED in sequence to high, this will turn it on.
    debugLEDstate = !debugLEDstate;
    digitalWrite(LED_BUILTIN, debugLEDstate);
  }

}

void shutDownLights()
{

  for (loopIndex = 0; loopIndex < LED_COUNT; loopIndex++)
  {
    digitalWrite(LED_PINS[loopIndex], LOW);
  }
}


void turnOnAlarm() //alarm can only be sounded if both a person and a train is present
{
  if (humanDetected && trainDetected)
  {
    digitalWrite(ALARM_PIN, HIGH);
  }

}
