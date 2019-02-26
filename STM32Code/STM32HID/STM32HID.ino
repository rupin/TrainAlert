#include "USBDevice.h"
#include "Mouse.h"
#include "Keyboard.h"

void setup() {
  pinMode(PC13, OUTPUT); // PC13 and LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  USBDevice.beginSerialHID(&Mouse, &Keyboard);
}

void loop(){
  digitalWrite(PC13, HIGH);
  delay(1000);
  digitalWrite(PC13, LOW);
  Keyboard.write('a');
  Keyboard.write('b');
  Keyboard.write('c');
  Keyboard.write('d');
  Keyboard.write('e');
  Keyboard.write('f');
  Keyboard.write('g');
  delay(5000); 
}
