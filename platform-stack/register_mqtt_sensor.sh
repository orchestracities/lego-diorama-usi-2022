#Provision a service group:
sudo curl -iX POST \
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
   },
   {
     "apikey":      "4jkkokgpepnvsb0sd7q21t53tu",
     "entity_type": "Lights",
     "resource":    ""
   }
 ]
}'

#register mqtt device
sudo curl -iX POST \
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
   },
   {
     "device_id":   "traffic002",
     "entity_name": "urn:ngsi-ld:TrafficFlowObserved:002",
     "entity_type": "TrafficFlowObserved",
     "protocol":    "JSON",
     "transport":   "MQTT",
     "timezone":    "Europe/Berlin",
     "attributes": [
       { "object_id": "car_counter", "name": "count", "type": "Integer" }
     ]
   }
   {
     "device_id":   "lights001",
     "entity_name": "urn:ngsi-ld:Lights:001",
     "entity_type": "Lights",
     "protocol":    "JSON",
     "transport":   "MQTT",
     "timezone":    "Europe/Berlin",
     "attributes": [
       { "object_id": "powerState", "name": "status", "type": "String" }
     ],
      "commands": [
        {
            "type": "command",
            "name": "light",
            "mqtt": { "switch: "on",}
        },
        {
            "type": "command",
            "name": "light",
            "mqtt": { "switch": "off"}
        }
    ]
     
   }
 ]
}
'

#create subscription to foreward to orion to ql
sudo curl -iX POST \
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

# #send mqtt message
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"car_counter": 10}' \
#   -t "/json/4jggokgpepnvsb2uv4s40d59ov/traffic001/attrs"

# #check values in orion
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'

# #check values in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001/attrs/count?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'