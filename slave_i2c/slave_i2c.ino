// Based off of - Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>
//  appended by Andy MacGregor

// This example code is in the public domain.


#include <Wire.h>

//Constants
const int PING = 0x00;
const int LED_ON = 0x01;
const int LED_OFF = 0x02;
const int ADC_READ = 0x03;

const int ACK = 0xEE;
const int ERR = 0xAA;

int lastCmd = 0;

//pin designations
const int weightSensor = A0;
int weightSensorValue = 0;
const int ledPin = 12;

void setup() {
  Serial.begin(9600);           // start serial for output
  
  //get the hard-coded address of this slave
  Serial.println("configuring pins 0-6 as INPUT");
  int addr = 0;
  int readIn = 0;
  for(int i = 2; i < 8; i++)
  {
    pinMode(i, INPUT);
    readIn = digitalRead(i);
    Serial.println(readIn);
    addr += (readIn << (i-2));
  }
  
  Serial.println(addr);

  pinMode(ledPin, OUTPUT);
  
  Wire.begin(addr);             // join i2c bus with hard-coded address
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  
}

void loop() {
  weightSensorValue = analogRead(weightSensor);
  delay(100); //just respond to events while updating the force sensor
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  lastCmd = Wire.read(); // receive first as a character
  if(howMany != 1)
  {
    Serial.println("Got too many bytes from the Dragon");
  }
  
  switch(lastCmd)
  {
    case PING:
      Serial.println("Received PING");
      //write 1 byes
      break;
    case LED_ON:
      Serial.println("Received LED_ON");
      digitalWrite(ledPin, HIGH);
      //no response
      break;
    case LED_OFF:
      Serial.println("Received LED_OFF");
      digitalWrite(ledPin, LOW);
      //no response
      break;
    case ADC_READ:
      Serial.println("Received ADC_READ");
      //write 2 bytes
      break;
    default:
      Serial.println("Received unrecognized message: ");
      Serial.print(lastCmd);
      break;
  }
}

//Master is expecting bytes from the slave
//send them back
void requestEvent()
{
  switch(lastCmd)
  {
    case PING:
      Serial.println("Sending ACK from Ping");
      Wire.write(ACK);
      break;
    case ADC_READ:
      Serial.println("Sending ADC results");
      Wire.write((weightSensorValue >> 8) & 0xFF);
      Wire.write((weightSensorValue ) & 0xFF);
      break;
    default:
      Serial.println("Didn't expect to be requested data. Last cmd: ");
      Serial.print(lastCmd);
      break;
  }
}

