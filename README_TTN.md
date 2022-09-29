# The Things Gateway Configuration
Official tutorial: https://www.thethingsindustries.com/docs/gateways/thethingskickstartergateway/

## Procedure:
- Remove the white cover
- Power on the TTN Gateway
- Press the red button for 5 seconds to reset WiFi settings and Activation Settings
- Connect to the WiFi signal emitted from the Gateway (ex. TheThings-Gateway-xxxxx)
- Go to http://192.168.84.1/ and configure

```
GatewayID:
ttn-martel-innovate-001@ttn

Account Server:
https://eu1.cloud.thethings.network/

Gateway Key:
NNSXS.V2QQSQL73SK5CRMVTPHS24PMIPMXOC6ZIKZS7DY.H7SFIQH2FNHITEJBELFZKKNVPZQRIF5Z22N7LE4GWAA5WX3IZJ2Q
```

After configuration wait until 4 out of 5 blue leds turn on.

Led meaning documentation:
https://www.thethingsnetwork.org/docs/gateways/gateway/ledstatus/

The TTN Gateway is now ready to use.


# The Things One Configuration

## Device Info (TTU#002)
```
EUI: 0004A30B001F9A4B
Battery: 3294
AppEUI: 70B3D57ED00181B5
DevEUI: 0004A30B001F9A4B
Data Rate: 0
RX Delay 1: 1000
RX Delay 2: 2000
```

## Device Info (TTU#004)
```
EUI: 0004A30B0020085D
Battery: 3294
AppEUI: 0000000000000000
DevEUI: 0004A30B0020085D
Data Rate: 0
RX Delay 1: 1000
RX Delay 2: 2000
```

TTO Activation Process:
https://www.thethingsindustries.com/docs/devices/the-things-uno/

# Known issues:
- The gateway randomly disconnects from wifi
- The gateway randomly reboots
- TTU#0004 AppEUI is 0000000000000000
- TTU#0002 gives this while trying connecting

```
Sending: mac set ch status 5 on
Sending: mac set ch freq 6 867700000
Sending: mac set ch drrange 6 0 5
Sending: mac set ch dcycle 6 499
Sending: mac set ch status 6 on
Sending: mac set ch freq 7 867900000
Sending: mac set ch drrange 7 0 5
Sending: mac set ch dcycle 7 499
Sending: mac set ch status 7 on
Sending: mac set pwridx 1
Sending: mac set retx 7
Sending: mac set dr 5
Sending: mac join otaa 
Join not accepted: denied
Check your coverage, keys and backend status.
```