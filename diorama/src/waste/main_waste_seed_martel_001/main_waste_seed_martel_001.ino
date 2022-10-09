//code for seed-martel-001
#include <LoRaWan.h>

#define DEBUG 0 //1 to show debugging print in serial monitor, 0 to hide them

#define FLAME_SENSOR 1 //connect SENSOR to digital pin1

 
unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];
 
 
void setup(void)
{
    SerialUSB.begin(115200);
    while(!SerialUSB);
 
    lorawanConfig();

    //FLAME SENSOR
    //pinMode(FLAME_SENSOR, INPUT);
    
}
 
void loop(void)
{   
    //working example
    // int booleanomagico = 1;
    // char data[2];
    // itoa(booleanomagico,data,10);  
    // lora.transferPacket(data,10);

    // bool result = false;

    int flameBool = flameSensorLoop();
    Serial.println(flameBool);
    char data[2];
    itoa(flameBool,data,10);
    lora.transferPacket(data, 10);

    //result = lora.transferPacket("buongiorno",10);
    //result = lora.transferPacket(data, 10);
    //result = lora.transferPacket(data, 10, 10);
 
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


//settings for the connection with the 
void lorawanConfig(){
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
 
    while(!lora.setOTAAJoin(JOIN));
}

//FLAME SENSOR

int flameSensorLoop(){
  if(isFlameDetected())
    Serial.println("flame");
  else Serial.println("nothing");
    delay(1000);

  return isFlameDetected();
}

bool isFlameDetected()
{
    if(DEBUG) Serial.println(!digitalRead(FLAME_SENSOR));
    return !digitalRead(FLAME_SENSOR);
}
