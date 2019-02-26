
#define LED_COUNT 4
#define PIR_COUNT 4
int LED_PINS[8] = {PB12, PB13, PB14, PB15, PA8, PA11, PA12, PA15};
int PIR_PINS[8] = {PB6, PB7, PB8, PB9, PB3, PB4, PB10, PB11};
byte pirStatus = 0;
byte loopIndex = 0;
byte LEDSequenceIndex = 0;
byte stayOnFor = 20000; // Stay on for 20 seconds once triggered.
long int startedSince = 0;
bool debugLEDstate = false;
boolean lightsOn = false;

boolean humanDetected = false;
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
    pinMode(PIR_PINS[loopIndex], INPUT_PULLDOWN);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  Serial1.begin(115200);
  timeElapsed = millis();

}

void loop() {

  humanDetected = false; //Set this to false to avoid accidental retriggering

  pirStatus = 0; //Reset the Variable to correct initial State.
  for (loopIndex = 0; loopIndex < PIR_COUNT; loopIndex++)
  {
    byte state = digitalRead(PIR_PINS[loopIndex]); //digital read returns value 0(0b00000000) or 1(0b00000001)
    /*Shift the LSB up, based on the sensor index, append 0 on LSB.
      OR operation just sets the bit which corresponds to the sensor index*/

    pirStatus = pirStatus || (state << loopIndex);
  }
  //pirStatus=1;
  Serial1.print("PIR Status:");
  Serial1.println(pirStatus);
  if (pirStatus > 0) // atleast one of the sensor was reporting an active state.
  {
    //timeElapsed=millis(); // Set this to current milliseconds time
    humanDetected = true; //Set variable to true;
    //startedSince=millis();// Set this to current milliseconds time
  }


  if (humanDetected)
  {
    lightsOn = true;
    startLEDSequence(); //do this till twenty seconds

  }
  else
  {

    if (lightsOn) // So that we turn off the lights only once.
    {
      shutDownLights();
      lightsOn = false;
    }
  }
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


void sendRadioTransmission()
{


}
