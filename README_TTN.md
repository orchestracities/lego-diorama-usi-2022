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
"Stored safely in local"
```

After configuration wait until 4 out of 5 blue leds turn on.

The TTN Gateway is now ready to use.

> *NOTE*: The led meaning documentation is available [here](https://www.thethingsnetwork.org/docs/gateways/gateway/ledstatus/)

## Seeeduino LoRaWAN Configuration

Every seeduino is connected to a ```Grove - Base Shield```, an expansion module.

The Grove sensors are connected to the shield.

### Device Info (seed-martel-001)

```bash
AppEUI: 8CF957200005727C
DevEUI: 70B3D57ED00561EC
AppKey: 51F58CAC3F5735E3D1F88DD3EADBE9C6

Application: oc-diorama-001
```


| Sensor |Port |
| ------------- | ------------- |
| Accelerometer   | I2C  |
| Flame Sensor  | D2 |
| Ultrasonic Ranger   | D3  |
| Temperature and Humidity sensor  | D5  |

### Device Info (seed-martel-002)

```bash
AppEUI: 8CF95720000569A6
DevEUI: 70B3D57ED00579DB
AppKey: F80714239C6BC515D1772ED0C38A55F0

Application: oc-diorama-002
```

| Sensor |Port |
| ------------- | ------------- |
| Multichannel Gas Sensor   | I2C  |
| Air Quality Sensor  | A0  |
| Sound Sensor  | A1  |
| UV_Sensor  | A2  |

## Connection between Seeeduino LoRaWAN and TTN

At the moment there are 2 Seeeduino LoRaWAN connected to our account on
The Things Network.

- To add another Seeeduino you need to add a new End Device under an
  application in our TTN account, like `oc-diorama-001` or `oc-diorama-002`.

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

## Receive data from TTN into IoT-LoRaWan Agent

In order to receive data from TTN the IoT-Lorawan Agent should
subscribe to the applications on the TTN platform.

The code below is the comand, already configured, that we use in order to connect.

Two keys, one for each application are generated in order to
connect and used as password.

```bash
curl --location --request POST 'localhost:4042/iot/services' \
--header 'fiware-service: openiot' \
--header 'fiware-servicePath: /' \
--header 'Content-Type: application/json' \
--data-raw '{
  "services": [
    {
      "device_id": "urn:ngsi-ld:WasteContainer:martel-ttn-001",
      "entity_name": "urn:ngsi-ld:WasteContainer:martel-ttn-001",
      "entity_type": "WasteContainer",
      "apikey": "",
      "resource": "8CF957200005727C",
      "expressionLanguage": "jexl",
      "attributes": [
        {
          "object_id": "temperature_1",
          "name": "temperature",
          "type": "Number"
        },
        {
          "object_id": "relative_humidity_1",
          "name": "relativeHumidity",
          "type": "Number"
        },
        {
          "object_id": "digital_in_1",
          "name": "fillingLevel",
          "type": "Number"
        },
        {
          "object_id": "digital_in_2",
          "name": "status",
          "type": "Text",
          "expression" : "digital_in_2 == 0 ? \"Ok\" : digital_in_2 == 1 ? \"Lid Open\" : digital_in_2 == 2 ? \"Dropped\" :\"Burning\" "
        }
      ],
      "internal_attributes": {
        "lorawan": {
          "application_server": {
            "host": "eu1.cloud.thethings.network",
            "username": "oc-diorama-001@ttn",
            "password": "pass",
            "provider": "TTN"
          },
          "app_eui": "8CF957200005727C",
          "application_id": "oc-diorama-001@ttn",
          "application_key": "51F58CAC3F5735E3D1F88DD3EADBE9C6",
          "data_model": "application_server"
        }
      }
    }
  ]}'

curl --location --request POST 'localhost:4042/iot/services' \
--header 'fiware-service: openiot' \
--header 'fiware-servicePath: /' \
--header 'Content-Type: application/json' \
--data-raw '{
  "services": [
    {
      "device_id": "urn:ngsi-ld:AirQualityObserved:martel-ttn-002",
      "entity_name": "urn:ngsi-ld:AirQualityObserved:martel-ttn-002",
      "entity_type": "environment",
      "apikey": "",
      "resource": "8CF95720000569A6",
      "expressionLanguage": "jexl",
      "attributes": [
        {
          "object_id": "digital_in_1",
          "name": "NO2",
          "type": "Number"
        },
        {
          "object_id": "digital_in_2",
          "name": "C2H50H",
          "type": "Number"
        },
        {
          "object_id": "digital_in_3",
          "name": "VOC",
          "type": "Number"
        },
        {
          "object_id": "digital_in_4",
          "name": "CO",
          "type": "Number"
        },
        {
          "object_id": "digital_in_5",
          "name": "airQuality",
          "type": "Text",
          "expression" : "digital_in_5 == 0 ? \"Good\" : digital_in_5 == 1 ? \"Low\" : digital_in_5 == 2 ? \"Med\" : \"High\" "
        },
        {
          "object_id": "digital_in_6",
          "name": "sound",
          "type": "Number"
        },
        {
          "object_id": "digital_in_7",
          "name": "UVvalue",
          "type": "Number"
        }
      ],
      "internal_attributes": {
        "lorawan": {
          "application_server": {
            "host": "eu1.cloud.thethings.network",
            "username": "oc-diorama-002@ttn",
            "password": "",
            "provider": "TTN"
            },
            "app_eui": "8CF95720000569A6",
            "application_id": "oc-diorama-002@ttn",
            "application_key": "F80714239C6BC515D1772ED0C38A55F0",
            "data_model": "application_server"
          }
        }
      }
    ]}'
```

## Receive data from TTN into Orion-Quantumleap-DB

In order to receive data from TTN the Orion-Quantumleap-DB should
subscribe to the applications on the TTN platform.

The code below is the comand, already configured, that we use in order to connect.

```bash
curl -iX POST \
    'http://localhost:1026/v2/subscriptions/' \
    -H 'Content-Type: application/json' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /' \
    -d '{
    "description": "Notify QuantumLeap of count changes of any WasteContainer Sensor",
    "subject": {
      "entities": [
        {
          "idPattern": ".*",
          "type": "WasteContainer"
        }
      ]
    },
    "notification": {
      "http": {
        "url": "http://quantumleap:8668/v2/notify"
      },
      "metadata": ["dateCreated", "dateModified"]
    },
    "throttling": 1
  }'

  curl -iX POST \
    'http://localhost:1026/v2/subscriptions/' \
    -H 'Content-Type: application/json' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /' \
    -d '{
    "description": "Notify QuantumLeap of count changes of any AirQualityObserved Sensor",
    "subject": {
      "entities": [
        {
          "idPattern": ".*",
          "type": "AirQualityObserved"
        }
      ]
    },
    "notification": {
      "http": {
        "url": "http://quantumleap:8668/v2/notify"
      },
      "metadata": ["dateCreated", "dateModified"]
    },
    "throttling": 1
  }'
```

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
