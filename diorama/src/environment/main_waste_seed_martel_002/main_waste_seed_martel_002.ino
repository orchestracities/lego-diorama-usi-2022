//code for seed-martel-001
#include <LoRaWan.h>
#include <string.h>
#include <Wire.h>
#include "MMA7660.h"

#define DEBUG 1  //1 to show debugging print in serial monitor, 0 to hide them

//accelerometer def
MMA7660 accelerometer;

//digital pins def
#define FLAME_SENSOR 1  //connect SENSOR to digital pin1

//analog pins def
const int rotary_angle_sensor = A0;


unsigned char data[10] = {
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  0xA,
};
char buffer[256];


void setup(void) {
  SerialUSB.begin(115200);
  while (!SerialUSB)
    ;

  lorawanConfig();

  accelerometerSetup();

  //FLAME SENSOR
  //pinMode(FLAME_SENSOR, INPUT);

  //inizialize rotary angle sensor
  pinMode(rotary_angle_sensor, INPUT);
}

void loop(void) {
  //working example
  // int booleanomagico = 1;
  // char data[2];
  // itoa(booleanomagico,data,10);
  // lora.transferPacket(data,10);

  //potenziometer example
  int booleanomagico = rotary_angle_sensor_loop();
  char data[10];
  itoa(booleanomagico, data, 10);

  char str1[10] = "angle: ";
  strcat(str1, data);

  char str2[10] = "x: ";
  char accelerometerX[10];
  itoa(accelerometerLoop(),accelerometerX,10);
  strcat(str2,accelerometerX);

  bool msg1 = lora.transferPacketWithConfirmed("buongiorno", 4);
  delay(4000);
  bool msg2 = lora.transferPacketWithConfirmed(str1, 4);
  delay(4000);
  bool msg3 = lora.transferPacketWithConfirmed(str2,4);
  delay(4000);

  // Serial.println("msg1");
  // Serial.println(msg1);

  // if(msg1){
  //   Serial.println("wait 5 seconds");
  //   delay(5000);
  //   Serial.println("WORKED");
  //   bool msg2 = lora.transferPacketWithConfirmed(str1,5);
  //   Serial.println("msg2");
  //   Serial.println(msg2);
  // }
  // bool msg2 = lora.transferPacketWithConfirmed(str1,5);
  // Serial.println("msg2");
  // Serial.println(msg2);

  // bool result = false;

  // int flameBool = flameSensorLoop();
  // Serial.println(flameBool);
  // char data[2];
  // itoa(flameBool,data,10);
  // lora.transferPacket(data, 10);

  //lora.transferPacket("buongiorno",10);
  //result = lora.transferPacket("buongiorno",10);
  // result = lora.transferPacket(data, 10);
  // result = lora.transferPacket(data, 10, 10);

  // if(result)
  // {
  //     short length;
  //     short rssi;

  //     memset(buffer, 0, 256);
  //     length = lora.receivePacket(buffer, 256, &rssi);

  //     if(length)
  //     {
  //         SerialUSB.print("Length is: ");
  //         SerialUSB.println(length);
  //         SerialUSB.print("RSSI is: ");
  //         SerialUSB.println(rssi);
  //         SerialUSB.print("Data is: ");
  //         for(unsigned char i = 0; i < length; i ++)
  //         {
  //             SerialUSB.print("0x");
  //             SerialUSB.print(buffer[i], HEX);
  //             SerialUSB.print(" ");
  //         }
  //         SerialUSB.println();
  //     }
  // }
}


//settings for the connection with the gateway
void lorawanConfig() {
  lora.init();

  memset(buffer, 0, 256);
  lora.getVersion(buffer, 256, 1);
  SerialUSB.print(buffer);

  memset(buffer, 0, 256);
  lora.getId(buffer, 256, 1);
  SerialUSB.print(buffer);

  //lora.setKey("2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C");
  //lora.setKey("51F58CAC3F5735E3D1F88DD3EADBE9C6", "51F58CAC3F5735E3D1F88DD3EADBE9C6", "51F58CAC3F5735E3D1F88DD3EADBE9C6");

  // void setId(char *DevAddr, char *DevEUI, char *AppEUI);
  lora.setId(NULL, "70B3D57ED00561EC", "8CF957200005727C");
  // setKey(char *NwkSKey, char *AppSKey, char *AppKey);
  lora.setKey(NULL, NULL, "51F58CAC3F5735E3D1F88DD3EADBE9C6");

  lora.setDeciveMode(LWOTAA);
  lora.setDataRate(DR0, EU868);

  lora.setChannel(0, 868.1);
  lora.setChannel(1, 868.3);
  lora.setChannel(2, 868.5);

  lora.setReceiceWindowFirst(0, 868.1);
  lora.setReceiceWindowSecond(869.5, DR3);

  lora.setDutyCycle(false);
  lora.setJoinDutyCycle(false);

  lora.setPower(14);

  while (!lora.setOTAAJoin(JOIN))
    ;
}

//FLAME SENSOR

int flameSensorLoop() {
  if (isFlameDetected())
    Serial.println("flame");
  else Serial.println("nothing");
  delay(1000);

  return isFlameDetected();
}

bool isFlameDetected() {
  if (DEBUG) Serial.println(!digitalRead(FLAME_SENSOR));
  return !digitalRead(FLAME_SENSOR);
}


//rotary_angle_sensor
int rotary_angle_sensor_loop() {
  int value = analogRead(rotary_angle_sensor);
  Serial.println(value);  // pirnt the value on the serial monitor screen
  delay(100);             // wait 1000ms before printing next value
  return value;
}

//accelerometer
void accelerometerSetup() {
  accelerometer.init();
  Serial.begin(9600);
}
int accelerometerLoop() {
  int8_t x;
  int8_t y;
  int8_t z;
  float ax, ay, az;
  accelerometer.getXYZ(&x, &y, &z);

  if (DEBUG) {
    Serial.print("x = ");
    Serial.println(x);
    Serial.print("y = ");
    Serial.println(y);
    Serial.print("z = ");
    Serial.println(z);
  }

  accelerometer.getAcceleration(&ax, &ay, &az);
  if (DEBUG) {
    Serial.println("accleration of X/Y/Z: ");
    Serial.print(ax);
    Serial.println(" g");
    Serial.print(ay);
    Serial.println(" g");
    Serial.print(az);
    Serial.println(" g");
    Serial.println("*************");
  }
  delay(500);
  return x;
}