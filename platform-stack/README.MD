# Platform Mini Stack

To deploy the Platform:

```bash
$ ./services start

NAMES                           STATUS                            PORTS
fiware-iot-agent                Up 6 seconds (health: starting)   0.0.0.0:4041->4041/tcp, 0.0.0.0:7896->7896/tcp
fiware-orion                    Up 6 seconds (healthy)            0.0.0.0:1026->1026/tcp
fiware-iot-agent-lora           Up 6 seconds                      0.0.0.0:4042->4041/tcp
fiware-quantumleap-db-setup-1   Up 6 seconds                      
fiware-quantumleap              Up 6 seconds (health: starting)   0.0.0.0:8668->8668/tcp
```

To stop the Platform:

```bash
$ ./services stop
```

Once deploy you are able to access:
- Orion at localhost:1026
- JSon IoT Agent at localhost:4041 (configuration api) and localhost:7896 (data api - not used with mqtt)
- LoraWan IoT Agent at localhost:4042 (configuration api)
- QuantumLeap at localhost:8668

Additionally the following services are also deployed:
- Mosquitto localhost:1883
- Grafana localhost:3000 (user: admin / pwd: admin)

For more information on each service read the [Relevant Readings](#Relevant_Readings) section.

## Examples 

### MQTT device with JSON Agent

1. Provision a service group:

  ```
  curl -iX POST \
    'http://localhost:4041/iot/services' \
    -H 'Content-Type: application/json' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /' \
    -d '{
   "services": [
     {
       "apikey":      "4jggokgpepnvsb2uv4s40d59ov",
       "entity_type": "TrafficFlowObserved",
       "resource":    ""
     }
   ]
  }'
  ```

1. To register a MQTT device:

  ```
  curl -iX POST \
    'http://localhost:4041/iot/devices' \
    -H 'Content-Type: application/json' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /' \
    -d '{
   "devices": [
     {
       "device_id":   "traffic001",
       "entity_name": "urn:ngsi-ld:TrafficFlowObserved:001",
       "entity_type": "TrafficFlowObserved",
       "protocol":    "JSON",
       "transport":   "MQTT",
       "timezone":    "Europe/Berlin",
       "attributes": [
         { "object_id": "car_counter", "name": "count", "type": "Integer" }
       ]
     }
   ]
  }
  '
  ```

1. Create to subscription to forward data from Orion to QL:

  ```
  curl -iX POST \
    'http://localhost:1026/v2/subscriptions/' \
    -H 'Content-Type: application/json' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /' \
    -d '{
    "description": "Notify QuantumLeap of count changes of any TrafficFlowObserved Sensor",
    "subject": {
      "entities": [
        {
          "idPattern": ".*",
          "type": "TrafficFlowObserved"
        }
      ],
      "condition": {
        "attrs": [
          "count"
        ]
      }
    },
    "notification": {
      "http": {
        "url": "http://quantumleap:8668/v2/notify"
      },
      "attrs": [
        "count"
      ],
      "metadata": ["dateCreated", "dateModified"]
    },
    "throttling": 1
  }'
  ```


1. To send a mqtt message:

  ```
  docker run -it --rm --name mqtt-publisher --network \
    fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"car_counter": 10}' \
    -t "/json/4jggokgpepnvsb2uv4s40d59ov/traffic001/attrs"
  ```

1. To check the values in orion:

  ```
  curl -X GET \
    'http://localhost:1026/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001?options=keyValues' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /'
  ```

1. To check the values in quantumleap:

  ```
  curl -X GET \
    'http://localhost:8668/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001/attrs/count?limit=3' \
    -H 'Accept: application/json' \
    -H 'Fiware-Service: openiot' \
    -H 'Fiware-ServicePath: /'
  ```

### LoraWan device with LoraWan Agent

1. Provision a service group:

  ```
  curl --location --request POST 'localhost:4042/iot/services' \
  --header 'fiware-service: openiot' \
  --header 'fiware-servicePath: /' \
  --header 'Content-Type: application/json' \
  --data-raw '{
      "services": [
          {
              "entity_type": "AirQualityObserved",
              "apikey": "",
              "resource": "70B3D57ED00006B2",
              "attributes": [
                  {
                      "object_id": "temperature_1",
                      "name": "temperature",
                      "type": "Number"
                  }
              ],
              "internal_attributes": {
                  "lorawan": {
                      "application_server": {
                          "host": "mqtt",
                          "username": "admin",
                          "password": "password",
                          "provider": "TTN"
                      },
                      "app_eui": "70B3D57ED00006B2",
                      "application_id": "demoTTN",
                      "application_key": "BE6996EEE2B2D6AFFD951383C1F3C3BD",
                      "data_model": "cayennelpp"
                  }
              }
          }
      ]
  }'
  ```
>NOTE: For a correct deployment, copy `.env.example` to `.env` and configure it with the correct passwords for TTN.

> NOTE: LoraWan connection data needs to be retrieved from TTN. Check the
Lorwan Agent documentation for guidance.

> NOTE: LoraWan supports self Provision, no
need to register the single device (but you can to force the name)

1. Create to subscription to forward data from Orion to QL:

  ```
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


1. To check the values in orion:

  ```
  curl -X GET \
    'http://localhost:1026/v2/entities?options=keyValues' \
    -H 'fiware-service: openiot' \
    -H 'fiware-servicepath: /'
  ```

1. To check the values in quantumleap:

  ```
  curl -X GET \
    'http://localhost:8668/v2/entities/<correct_entity_id>/attrs/temperature?limit=3' \
    -H 'Accept: application/json' \
    -H 'Fiware-Service: openiot' \
    -H 'Fiware-ServicePath: /'
  ```

## Relevant Readings
- https://fiware-orion.readthedocs.io/
- https://fiware-iotagent-json.readthedocs.io/en/latest/
- https://fiware-lorawan.readthedocs.io/en/latest/
- https://quantumleap.readthedocs.io/en/latest/
