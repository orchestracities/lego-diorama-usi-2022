// code for seed-martel-002

//multichannel_gas_sensor on I2C
//air quality sensor on A0
//sound sensor on A1
//UV_sensor on A2

#include <LoRaWan.h>

#include <CayenneLPP.h>
CayenneLPP lpp(100);

#include <Wire.h>
#include <string.h>
#include <stdlib.h>


#define DEBUG 1 // 1 to show debugging print in serial monitor, 0 to hide them

//multichannel_gas_sensor
#include <Multichannel_Gas_GMXXX.h>
#ifdef SOFTWAREWIRE
    #include <SoftwareWire.h>
    SoftwareWire myWire(3, 2);
    GAS_GMXXX<SoftwareWire> gas;
#else
    #include <Wire.h>
    GAS_GMXXX<TwoWire> gas;
#endif

//air quality sensor
#include "Air_Quality_Sensor.h"

AirQualitySensor sensor(A0);

//sound sensor
const int pinSound = A1;
int thresholdValue = 200;

//UV_Sensor
const int pinUV_Sensor = A2;


static uint8_t recv_cmd[8] = {};

//unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];

//MEASURE VALUES

//multichannelGasSensor
uint32_t NO2 = 0, C2H5OH = 0, VOC = 0, CO = 0;

//AirQualitySensor
uint32_t quality = 0;

//AirQualitySensor status
// #define FORCE_SIGNAL 3;
// #define HIGH_POLLUTION 2;
// #define LOW_POLLUTION 1;
// #define FRESH_AIR 0;


//soundSensor
int soundValue = 0;

//UVSensor
long UVvalue = 0;




void setup(void) {
    SerialUSB.begin(115200);
    // while (!SerialUSB)
    //     ;

    //Lorawan Chip Config
    lorawanConfig();

    //multichannel_gas_sensor on I2C
    multGasSensorSetup();

    //air quality sensor
    airQualitySetup();
}

void loop(void) {

  lpp.reset(); //reset lpp to send new message

  //multGasSensor
  multGasSensorLoop();
  
  lpp.addDigitalInput(1,NO2);
  lpp.addDigitalInput(2,C2H5OH);
  lpp.addDigitalInput(3,VOC);
  lpp.addDigitalInput(4,CO);

  //airQualitySensor
  airQualityLoop();
  lpp.addDigitalInput(5, quality);

  //soundSensor
  soundLoop();
  lpp.addDigitalInput(6,soundValue);

  //UVSensor
  UVSensorLoop();
  lpp.addDigitalInput(7, UVvalue);

  bool msg1 = lora.transferPacketWithConfirmed(lpp.getBuffer(),lpp.getSize(),5);
  delay(4000);

}

// settings for the connection with the gateway
void lorawanConfig() {
    lora.init();

    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer);

    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);

    // lora.setKey("2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C");
    // lora.setKey("51F58CAC3F5735E3D1F88DD3EADBE9C6", "51F58CAC3F5735E3D1F88DD3EADBE9C6", "51F58CAC3F5735E3D1F88DD3EADBE9C6");

    // void setId(char *DevAddr, char *DevEUI, char *AppEUI);
    lora.setId(NULL, "70B3D57ED00561F1", "8CF95720000569A6");
    // setKey(char *NwkSKey, char *AppSKey, char *AppKey);
    lora.setKey(NULL, NULL, "9C14C735E02914422112AC32D7C0EC27");

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


//multichannel_gas_sensor
void multGasSensorSetup() {
    gas.begin(Wire, 0x08); // use the hardware I2C
}

void multGasSensorLoop() {
    NO2 = gas.measure_NO2();
    C2H5OH = gas.measure_C2H5OH();
    VOC = gas.measure_VOC();
    CO = gas.measure_CO();
}

//air quality sensor
void airQualitySetup() {
    Serial.println("Waiting sensor to init...");
    delay(20000);

    if (sensor.init()) {
        Serial.println("Sensor ready.");
    } else {
        Serial.println("Sensor ERROR!");
    }
}

void airQualityLoop() {
    int slope = sensor.slope();

    if(DEBUG){
      Serial.print("Sensor value: ");
      Serial.println(sensor.getValue());
    }  
    
    if (slope == AirQualitySensor::FORCE_SIGNAL) {
        quality = 3;
    } else if (slope == AirQualitySensor::HIGH_POLLUTION) {
        quality = 2;
    } else if (slope == AirQualitySensor::LOW_POLLUTION) {
        quality = 1;
    } else if (slope == AirQualitySensor::FRESH_AIR) {
        quality = 0;
    }

}

//sound sensor
int soundLoop()
{
    soundValue = analogRead(pinSound);   //read the sensorValue on Analog 0
    if (DEBUG) Serial.println(soundValue);
}

//UV_sensor
void UVSensorLoop()
{
    int sensorValue;
    long  sum=0;
    for(int i=0;i<1024;i++)// accumulate readings for 1024 times
    {
        sensorValue=analogRead(pinUV_Sensor);
        //if(sensorValue) Serial.println(sensorValue);
        sum=sensorValue+sum;
        delay(2);
    }
    long meanVal = sum/1024;  // get mean value

    UVvalue = (meanVal*1000/4.3-83)/21;
}
