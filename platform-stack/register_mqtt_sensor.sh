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
   },
   {
     "apikey":      "parkingApi",
     "entity_type": "Parking",
     "resource":    ""
   },
   {
     "apikey":      "EVAPI",
     "entity_type": "Ev-Charging",
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
      },
      {
        "device_id":   "lights001",
        "entity_name": "urn:ngsi-ld:Lights:001",
        "entity_type": "Lights",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin",
        "attributes": [
          { "object_id": "powerState", "name": "state", "type": "String" }
        ],
         "commands": [
           {
               "name": "light",
                "type": "command"
           }
         ]
       },
       {
        "device_id":   "lights002",
        "entity_name": "urn:ngsi-ld:Lights:002",
        "entity_type": "Lights",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin",
        "attributes": [
          { "object_id": "powerState", "name": "state", "type": "String" }
        ],
         "commands": [
           {
               "type": "command",
               "name": "light"
           }
         ]
       },
       {
        "device_id":   "parking001",
        "entity_name": "urn:ngsi-ld:Parking:001",
        "entity_type": "Parking",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin",
        "attributes": [
          { "object_id": "parking_spot_status", "name": "parking_spot_status", "type": "String" }
        ]
      }
    ]
}'

#create TrafficFlow subscription to foreward to orion to ql
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

#lights orion subscription
sudo curl -iX POST \
  'http://localhost:1026/v2/subscriptions/' \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d '{
  "description": "Notify QuantumLeap of status changes of any Street Light",
  "subject": {
    "entities": [
      {
        "idPattern": ".*",
        "type": "Lights"
      }
    ],
    "condition": {
      "attrs": [
        "state"
      ]
    }
  },
  "notification": {
    "http": {
      "url": "http://quantumleap:8668/v2/notify"
    },
    "attrs": [
      "state"
    ],
    "metadata": ["dateCreated", "dateModified"]
  },
  "throttling": 1
}'

#parking orion subscription
sudo curl -iX POST \
  'http://localhost:1026/v2/subscriptions/' \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d '{
  "description": "Notify QuantumLeap of parking spot status changes",
  "subject": {
    "entities": [
      {
        "idPattern": ".*",
        "type": "Parking"
      }
    ],
    "condition": {
      "attrs": [
        "parking_spot_status"
      ]
    }
  },
  "notification": {
    "http": {
      "url": "http://quantumleap:8668/v2/notify"
    },
    "attrs": [
      "parking_spot_status"
    ],
    "metadata": ["dateCreated", "dateModified"]
  },
  "throttling": 1
}'




###########TESTING

#GET SERVICES
# sudo curl -iX GET \
#   'http://localhost:4041/iot/services' \
#   -H 'Content-Type: application/json' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /' 

#GET DEVICES
# sudo curl -iX GET \
#   'http://localhost:4041/iot/devices' \
#   -H 'Content-Type: application/json' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /' 

# # #send mqtt message
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"car_counter": 10}' \
#   -t "/json/4jggokgpepnvsb2uv4s40d59ov/traffic001/attrs"

# ##state change 
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"state" : "on"}}' \
#   -t "/json/4jkkokgpepnvsb0sd7q21t53tu/lights001/attrs"

# ##send mqtt lights command
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"light": {"switch" : "on"}}' \
#   -t "/json/4jkkokgpepnvsb0sd7q21t53tu/lights001/cmd"


# # #check values in orion street sensors
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
#check values in orion street lights
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:Lights:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
#check values in orion parking
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:Parking:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
 

# #check values in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001/attrs/count?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'

# #check lights status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:Lights:001/attrs/state?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
#chech parking status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:Parking:001/attrs/parking_spot_status?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
