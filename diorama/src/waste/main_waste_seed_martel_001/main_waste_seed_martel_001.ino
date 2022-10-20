//code for seed-martel-001

//Temp sensor on D5
//Fire sensor on D2
//Dist Sensor on D3
//Accelerometer on I2C

#include <LoRaWan.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>
#include "MMA7660.h"
#include "DHT.h"
#include "Ultrasonic.h"

#define DEBUG 1  //1 to show debugging print in serial monitor, 0 to hide them

//accelerometer def
MMA7660 accelerometer;

//Temp&Hum def
#define DHTTYPE DHT22 // DHT 22  (AM2302)
#define DHTPIN 5      // what pin we're connected to（DHT10 and DHT20 don't need define it）
DHT dht(DHTPIN, DHTTYPE);

//digital pins def
#define FLAME_SENSOR 2  //connect SENSOR to digital pin1

Ultrasonic ultrasonic(3); //Distance Sensor

unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];


void setup(void) {
  SerialUSB.begin(115200);
  // while (!SerialUSB)
  //   ;

  //Lorawan Chip Config
  lorawanConfig();

  //Accelerometer
  accelerometerSetup();

  //Temperature & Humidity
  tempHumSetup();

  //FLAME SENSOR
  pinMode(FLAME_SENSOR, INPUT);
}

void loop(void) {

  // //Accelerometer message
  // char str2[10] = "x: ";
  // char accelerometerX[10];
  // itoa(accelerometerLoop(),accelerometerX,10);
  // strcat(str2,accelerometerX);

  // //Accelerometer Message
  // bool msg1 = lora.transferPacketWithConfirmed(str2,4);
  // delay(4000);


  //Accelerometer Message
  char accelerometerMSG[100];
  strcpy(accelerometerMSG,accelerometerLoop());
  bool msg1 = lora.transferPacketWithConfirmed(accelerometerMSG, 4);
  delay(4000);

  //Flame Sensor Message
  bool flameBool = isFlameDetected();
  bool msg2 = flameBool ? lora.transferPacketWithConfirmed("Fire detected",4) :
                          lora.transferPacketWithConfirmed("Fire not detected",4);
  delay(4000);

  //Temperature & Humidity Message
  char TempMsg[50];
  strcpy(TempMsg,tempHumLoop());
  bool msg3 = lora.transferPacketWithConfirmed(TempMsg,4);
  delay(4000);

  //Distance Msg
  char DistMsg[50];
  strcpy(DistMsg,distanceLoop());
  bool msg4 = lora.transferPacketWithConfirmed(DistMsg,4);
  delay(4000);

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

bool isFlameDetected() {
  if (DEBUG) Serial.println(!digitalRead(FLAME_SENSOR));
  return !digitalRead(FLAME_SENSOR);
}

//accelerometer
void accelerometerSetup() {
  accelerometer.init();
  Serial.begin(9600);
}

char * accelerometerLoop() {
  int8_t x;
  int8_t y;
  int8_t z;
  float ax, ay, az;

  char * accelerometerMessage = (char *) malloc(100);
  char sensorValue[6]= "";


  accelerometer.getXYZ(&x, &y, &z);

  sprintf(sensorValue, "%i",x);
  strcpy(accelerometerMessage, "x: ");
  strcat(accelerometerMessage,sensorValue);


  sprintf(sensorValue, "%i",y);
  strcat(accelerometerMessage, ", y: ");
  strcat(accelerometerMessage,sensorValue);

  sprintf(sensorValue, "%i",z);
  strcat(accelerometerMessage, ", z: ");
  strcat(accelerometerMessage,sensorValue);


  if (DEBUG) {
    Serial.print("x = "); Serial.println(x); Serial.print("y = ");
    Serial.println(y); Serial.print("z = "); Serial.println(z);
  }

  accelerometer.getAcceleration(&ax, &ay, &az);

  sprintf(sensorValue, "%.2f",ax);
  strcat(accelerometerMessage, ", ax: ");
  strcat(accelerometerMessage,sensorValue);


  sprintf(sensorValue, "%.2f",ay);
  strcat(accelerometerMessage, ", ay: ");
  strcat(accelerometerMessage,sensorValue);

  sprintf(sensorValue, "%.2f",az);
  strcat(accelerometerMessage, ", az: ");
  strcat(accelerometerMessage,sensorValue);


  if (DEBUG) {
    Serial.println("accleration of X/Y/Z: "); Serial.print(ax); Serial.println(" g");
    Serial.print(ay); Serial.println(" g"); Serial.print(az); Serial.println(" g");
    Serial.println("*************");
  }

  return accelerometerMessage;
}

// Temp & Hum
void tempHumSetup() {
    Wire.begin();
    dht.begin();
}

char * tempHumLoop() {
    
    float temp_hum_val[2] = {0};
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

    char * tempHumMessage = (char *) malloc(50);
    char buf[6]= "";

    if (!dht.readTempAndHumidity(temp_hum_val)) {
        strcpy(tempHumMessage, "Hum: ");
        sprintf(buf, "%.2f",temp_hum_val[0]);
        strcat(tempHumMessage, buf);
        strcat(tempHumMessage, ", ");
        strcat(tempHumMessage, "Temp: ");
        sprintf(buf, "%.2f",temp_hum_val[1]);
        strcat(tempHumMessage, buf);;
    } else {
        strcpy(tempHumMessage, "Failed to get temp/hum.");
    }
    Serial.println(tempHumMessage);
    return tempHumMessage;
}

char * distanceLoop() {
    int RangeInCentimeters;

    char * distanceMessage = (char *) malloc(50);
    char buf2[6]= "";

    strcpy(distanceMessage,"Distance: ");

    RangeInCentimeters = (int)ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
    sprintf(buf2, "%i",RangeInCentimeters);
    strcat(distanceMessage,buf2);
    strcat(distanceMessage," cm");

    Serial.println(distanceMessage);

    return distanceMessage;
}