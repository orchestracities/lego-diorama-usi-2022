// code for seed-martel-002

//multichannel_gas_sensor on I2C
//air quality sensor on A0
//sound sensor on A1
//UV_sensor on A2

#include <LoRaWan.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>

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

#define DEBUG 1 // 1 to show debugging print in serial monitor, 0 to hide them

unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];

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
  
    // bool msg1 = lora.transferPacketWithConfirmed("Test", 4);
    // delay(4000);

    //multichannel_gas_sensor on I2C
    char multGasSensorMsg[100];
    strcpy(multGasSensorMsg,multGasSensorLoop());
    bool msg1 = lora.transferPacketWithConfirmed(multGasSensorMsg, 4);
    delay(4000);

    //air quality sensor
    char airQualitySensorMSG[100];
    strcpy(airQualitySensorMSG,airQualityLoop());
    bool msg2 = lora.transferPacketWithConfirmed(airQualitySensorMSG, 4);
    delay(4000);

    //soundSensor
    char soundSensorValue[5];
    char soundSensorMSG[100] = "Noise value: ";
    itoa(soundLoop(),soundSensorValue,10);
    strcat(soundSensorMSG,soundSensorValue);
    bool msg3 = lora.transferPacketWithConfirmed(soundSensorMSG, 4);
    delay(4000);

    //UV_sensor
    char UVSensorMSG[100];
    strcpy(UVSensorMSG,UVSensorLoop());
    bool msg4 = lora.transferPacketWithConfirmed(UVSensorMSG, 4);
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

char * multGasSensorLoop() {
    uint8_t len = 0;
    uint8_t addr = 0;
    uint8_t i;
    uint32_t val = 0;

    char * multGasSensorMessage = (char *) malloc(100);
    char sensorValue[6]= "";

    val = gas.measure_NO2();
    Serial.print("NO2: "); Serial.print(val); Serial.print("  =  ");
    Serial.print(gas.calcVol(val)); Serial.println("V");

    sprintf(sensorValue, "%i",val);

    strcpy(multGasSensorMessage,"NO2: ");
    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"  =  ");
    
    sprintf(sensorValue, "%i",gas.calcVol(val));

    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"V, ");



    val = gas.measure_C2H5OH();
    Serial.print("C2H5OH: "); Serial.print(val); Serial.print("  =  ");
    Serial.print(gas.calcVol(val)); Serial.println("V");

    sprintf(sensorValue, "%i",val);

    strcat(multGasSensorMessage,"C2H5OH: ");
    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"  =  ");
    
    sprintf(sensorValue, "%i",gas.calcVol(val));

    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"V, ");
    
    val = gas.measure_VOC();
    Serial.print("VOC: "); Serial.print(val); Serial.print("  =  ");
    Serial.print(gas.calcVol(val)); Serial.println("V");

    strcat(multGasSensorMessage,"VOC: ");
    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"  =  ");
    
    sprintf(sensorValue, "%i",gas.calcVol(val));

    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"V, ");    

    val = gas.measure_CO();
    Serial.print("CO: "); Serial.print(val); Serial.print("  =  ");
    Serial.print(gas.calcVol(val)); Serial.println("V");

    strcat(multGasSensorMessage,"CO: ");
    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"  =  ");
    
    sprintf(sensorValue, "%i",gas.calcVol(val));

    strcat(multGasSensorMessage,sensorValue);
    strcat(multGasSensorMessage,"V"); 

    return multGasSensorMessage;
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

char * airQualityLoop() {
    int quality = sensor.slope();

    Serial.print("Sensor value: ");
    Serial.println(sensor.getValue());

    char * airQualityMSG = (char *) malloc(100);

    if (quality == AirQualitySensor::FORCE_SIGNAL) {
        strcpy(airQualityMSG,"High pollution! Force signal active.");
    } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
        strcpy(airQualityMSG,"High pollution!");
    } else if (quality == AirQualitySensor::LOW_POLLUTION) {
        strcpy(airQualityMSG,"Low pollution!");
    } else if (quality == AirQualitySensor::FRESH_AIR) {
        strcpy(airQualityMSG,"Fresh air.");
    }

    return airQualityMSG;
}

//sound sensor
int soundLoop()
{
    int sensorValue = analogRead(pinSound);   //read the sensorValue on Analog 0
    Serial.println(sensorValue);
    // if(sensorValue>thresholdValue){
    //   digitalWrite(pinLed,HIGH);
    // }
    // digitalWrite(pinLed,LOW);
    return sensorValue;
}

//UV_sensor
char * UVSensorLoop()
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

    char * UVSensorMSG = (char *) malloc(100);
    char UVSensorValue[10]= "";
    sprintf(UVSensorValue, "%f8",(meanVal*1000/4.3-83)/21);
    
    strcpy(UVSensorMSG,"The current UV index is: ");
    strcat(UVSensorMSG,UVSensorValue);

    Serial.print("The current UV index is:");
    Serial.print((meanVal*1000/4.3-83)/21);// get a detailed calculating expression for UV index in schematic files.
    Serial.print("\n");

    return UVSensorMSG;

}
