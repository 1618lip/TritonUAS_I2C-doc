/*
  Name: Ultrasonic_Pixhawk_i2C.ino
  Testing the i2c commmunication with 
*/

#include <Wire.h>
#include <NewPing.h>

// Slave i2c address define
#define addr 0x70 //112

// Ultrasonic Sensor Pin define
#define trig 8
#define echo 8

// Maximum Distance is ?? cm
#define maxDist 300 //cm

NewPing sonar(trig, echo, maxDist);
uint16_t un16distance;
byte bReceived;

void setup() {
  Serial.begin(57600);
  Wire.begin(addr);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop() {
  un16distance = sonar.ping_cm(); // Get Distance
  Serial.print("Distance: "); Serial.print(un16distance); Serial.println(" cm");
  delay(300);
}

void receiveEvent(int howmany) {
  while (Wire.available()) {
    bReceived = Wire.read();
  }
}
void requestEvent() {
  Wire.write(highByte(un16distance));
  Wire.write(lowByte(un16distance));
}