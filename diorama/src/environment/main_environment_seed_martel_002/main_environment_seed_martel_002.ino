// code for seed-martel-001
#include <LoRaWan.h>
#include <Wire.h>
#include <string.h>

#include "MMA7660.h"

#define DEBUG 1 // 1 to show debugging print in serial monitor, 0 to hide them

// accelerometer def
MMA7660 accelerometer;

// digital pins def
#define FLAME_SENSOR 1 // connect SENSOR to digital pin1

// analog pins def
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
}

void loop(void) {
  
    bool msg1 = lora.transferPacketWithConfirmed("Test", 4);
    delay(4000);

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
