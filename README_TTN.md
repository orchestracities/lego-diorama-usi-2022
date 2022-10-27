# The Things Gateway Configuration

Official tutorial is available [here](https://www.thethingsindustries.com/docs/gateways/thethingskickstartergateway/)

## Procedure

1. Remove the white cover
1. Power on the TTN Gateway
1. Press the red button for 5 seconds to reset WiFi settings
  and Activation Settings
1. Connect to the WiFi signal emitted from the Gateway
  (ex. TheThings-Gateway-xxxxx)
1. Go to [http://192.168.84.1/](http://192.168.84.1/) and configure

```bash
GatewayID:
martel-ttn-01

Account Server:
https://eu1.cloud.thethings.network/

Gateway Key:
NNSXS.NACIYY37UADMZG6O6XD7HRH6OM6XDHEKEIUU4CI.VSZHH6U77GJZHBQB2LNO6RQN3RFONSVYEUKI3OBZXPJGCU5EB4KQC0MM3NT01T4L1ANO
```

- Remember to remove the security part.

After configuration wait until 4 out of 5 blue leds turn on.

The TTN Gateway is now ready to use.

> *NOTE*: The led meaning documentation is available [here](https://www.thethingsnetwork.org/docs/gateways/gateway/ledstatus/)

## Seeeduino LoRaWAN Configuration

### Device Info (seed-martel-001)

```bash
AppEUI: 8CF957200005727C
DevEUI: 70B3D57ED00561EC
AppKey: 51F58CAC3F5735E3D1F88DD3EADBE9C6
```

### Device Info (seed-martel-002)

```bash
AppEUI: 8CF95720000569A6
DevEUI: 70B3D57ED00561F1
AppKey: 9C14C735E02914422112AC32D7C0EC27
```

## Connection between Seeeduino LoRaWAN and TTN

At the moment there are 2 Seeeduino LoRaWAN connected to our account on
The Things Network.

- To add another Seeeduino you need to add a new End Device under the
  `oc-diorama-001` application that can be found in our TTN account.

- If there is no application, it must be created.

- Click on Add End Devide, choose the manual insertion method.

- The Frequency Plan must be Europe 863-870MHz (SF9)

- LoRaWAN version 1.0.3

- The JoinEUI value can be found on the chip present on the Seeeduino LoRaWAN.

- DevEui and AppKey must be generated and finally an ID must be chosen.

After this configuration the TTN will be ready to receive messages from
our boards.

## Send data to TTN

To send data to TTN we use has base the OTAA script provided by SeedStudio,
you can find the code snipped at this [link](https://wiki.seeedstudio.com/Seeeduino_LoRAWAN/).

That code need to be a little corrected:

```c
lora.setKey("2B7E151628AED2A6ABF7158809CF4F3C", "2B7E151628AED2A6ABF7158809CF4F3C"
"2B7E151628AED2A6ABF7158809CF4F3C");
```

The line above has to be removed and substitute with this one:

```c
//void setId(char *DevAddr, char *DevEUI, char *AppEUI);
lora.setId(NULL, "12409E2345695432", "70B3D57EF0006593");
// setKey(char *NwkSKey, char *AppSKey, char *AppKey);
lora.setKey(NULL, NULL, "47BDA77B6D7B4DDA7DC182E54295FE4E");
```

> *NOTE:* Data in the functions are just examples.

It is needed to configure those value with the one of the Seeeduino the user is
intended to send messages with.

Details are available [here](https://blog.squix.org/2017/07/seeeduino-lora-gps-getting-started-with-lorawan-and-ttn.html)

Compiling the OTAA sketch should make the Seeeduino connect to the Gateway and
send messages to the TTN.

## Required libraries for seed-martel-001

- [Accelerometer](https://github.com/Seeed-Studio/Accelerometer_MMA7660)

- [Temperature and Humidity](https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor)

- [Ultrasonic Ranger](https://github.com/Seeed-Studio/Seeed_Arduino_UltrasonicRanger/archive/master.zip)

### Sensors Specifications

- [Accelerometer](https://wiki.seeedstudio.com/Grove-3-Axis_Digital_Accelerometer-1.5g/)

- [Temperature and Humidity](https://wiki.seeedstudio.com/Grove-Temperature_and_Humidity_Sensor_Pro/)

- [Ultrasonic Ranger](https://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/)

- [Flame Sensor](https://wiki.seeedstudio.com/Grove-Flame_Sensor/)

## Required libraries for seed-martel-002

- [Air Quality Sensor](https://github.com/Seeed-Studio/Grove_Air_quality_Sensor)

- [Gas Sensor](https://github.com/Seeed-Studio/Seeed_Multichannel_Gas_Sensor/archive/master.zip)

### Sensors Specifications

- [Air Quality Sensor](https://wiki.seeedstudio.com/Grove-Air_Quality_Sensor_v1.3/)

- [Gas Sensor](https://wiki.seeedstudio.com/Grove-Multichannel-Gas-Sensor-V2/)

- [UV Sensor](https://wiki.seeedstudio.com/Grove-UV_Sensor/)

- [Sound Sensor](https://wiki.seeedstudio.com/Grove-Sound_Sensor/)
