#include "mavlink.h"            // Mavlink interface
#include "mavlink_msg_distance_sensor.h"
#include <Wire.h>
#define bRate 57600           // Baudrate
uint16_t distance = 0;          //Distance reading
uint8_t wait = 6;              //Delay between I2C readings
uint8_t scale = 10;             //Scale divider  ex. mm to cm

void setup(){ 
 Serial.begin(bRate);
 Wire.begin();
}


void command_heartbeat() {
  int sysid = 1;                            //< ID 1 for this system               
  int compid = MAV_COMP_ID_PATHPLANNER;       //< The component sending the message.
  uint8_t system_type =MAV_TYPE_GCS;         // Define the system type, in this case ground control station
  uint8_t autopilot_type = MAV_AUTOPILOT_INVALID;
  uint8_t system_mode = 0; 
  uint32_t custom_mode = 0;                
  uint8_t system_state = 0;
  
  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  
  // Pack the message
  mavlink_msg_heartbeat_pack(sysid,compid, &msg, system_type, autopilot_type, system_mode, custom_mode, system_state);
  
  // Copy the message to the send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  
  // Send the message 
  //Serial.write(buf, len);
}


void command_distance(uint8_t orient ,uint16_t rngDist) {
  //MAVLINK DISTANCE MESSAGE
  int sysid = 1;                   
  //< The component sending the message.
  int compid = 158;    

  uint32_t time_boot_ms = 0; /*< Time since system boot*/
  uint16_t min_distance = 30; /*< Minimum distance the sensor can measure in centimeters*/
  uint16_t max_distance = 900; /*< Maximum distance the sensor can measure in centimeters*/
  uint16_t current_distance = rngDist; /*< Current distance reading*/
  uint8_t type = 0; /*< Type from MAV_DISTANCE_SENSOR enum.*/
  uint8_t id = 1; /*< Onboard ID of the sensor*/
  uint8_t orientation = orient; /*(0=forward, each increment is 45degrees more in clockwise direction), 24 (upwards) or 25 (downwards)*/
// Consumed within ArduPilot by the proximity class
  uint8_t covariance = 0; /*< Measurement covariance in centimeters, 0 for unknown / invalid readings*/


  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
 mavlink_msg_distance_sensor_pack(sysid,compid,&msg,time_boot_ms,min_distance,max_distance,current_distance,type,id,orientation,covariance,0,0,0,0);

  // Copy the message to the send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  // Send the message (.write sends as bytes) 
  Serial.write(buf, len);
 // Serial.print (orientation);
  //Serial.print (" - ");
  //Serial.println (current_distance);
}


void wireRead(int Address,  int Data) {
  uint8_t byteLow;
  uint8_t byteHigh;
  Wire.requestFrom(Address, Data);    // Adress - Bytes (only one with Attiny)
  while (Wire.available()) { // slave may send less than requested
    byteLow = Wire.read();    // receive a byte as character
   //Serial.print(Wire.read(), HEX);         // print the character
   delay(wait);
   }
     Wire.requestFrom(Address, Data);    // Adress - Bytes (only one with Attiny)
  while (Wire.available()) { // slave may send less than requested
   byteHigh = Wire.read();    // receive a byte as character
   //Serial.println(Wire.read(), HEX);         // print the character
   delay(wait);
   }   
  distance= ((uint16_t)((byteLow) + (byteHigh*256))/scale);
}


void loop() {
/*(0=forward, each increment is 45degrees more in clockwise direction), 24 (upwards) or 25 (downwards)*/
//command_heartbeat();

  wireRead(3, 1);
  Serial.print (distance);
  Serial.print (" - ");
 command_distance(24 ,distance);

  wireRead(4, 1);
  Serial.print (distance);
  Serial.print (" - ");
 command_distance(7 ,distance);
 
  wireRead(5, 1);
  Serial.print (distance);
  Serial.print (" - ");
  command_distance(0 ,distance);
 
  wireRead(6, 1);
  Serial.println (distance);
  command_distance(1 ,distance);
  delay(wait);
  
}