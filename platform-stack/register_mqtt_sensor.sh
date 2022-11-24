#Provision a service group:EVChargingStation
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
     "resource":    "",
     "attributes": [
          { "object_id": "car_counter", "name": "intensity", "type": "Integer" }
          ]
   },
   {
     "apikey":      "4jkkokgpepnvsb0sd7q21t53tu",
     "entity_type": "StreetLight",
     "resource":    "",
     "attributes": [
          { "object_id": "powerState", "name": "state", "type": "String" }
        ],
         "commands": [
           {
               "name": "power",
                "type": "command"
           }
         ]
   },
   {
     "apikey":      "parkingApi",
     "entity_type": "ParkingSpot",
     "resource":    "",
     "attributes": [
          { "object_id": "parking_spot_status", "name": "status", "type": "String" }
        ]
   },
   {
     "apikey":      "EvAPI",
     "entity_type": "EVChargingStation",
     "resource":    "",
     "attributes": [
          { "object_id": "parking_spot_status", "name": "parking_spot_status", "type": "String" },
          {"object_id": "capacity", "name": "capacity", "type": "Integer" }
        ],
         "commands": [
           {
               "name": "charging",
                "type": "command"
           }
         ]
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
        "timezone":    "Europe/Berlin"
      },
      {
        "device_id":   "traffic002",
        "entity_name": "urn:ngsi-ld:TrafficFlowObserved:002",
        "entity_type": "TrafficFlowObserved",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin"
      },
      {
        "device_id":   "StreetLight001",
        "entity_name": "urn:ngsi-ld:StreetLight:001",
        "entity_type": "StreetLight",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin"
       },
       {
        "device_id":   "StreetLight002",
        "entity_name": "urn:ngsi-ld:StreetLight:002",
        "entity_type": "StreetLight",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin"
       },
       {
        "device_id":   "ParkingSpot001",
        "entity_name": "urn:ngsi-ld:ParkingSpot:001",
        "entity_type": "ParkingSpot",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin"
      },
      {
        "device_id":   "EVChargingStation001",
        "entity_name": "urn:ngsi-ld:EVChargingStation:001",
        "entity_type": "EVChargingStation",
        "protocol":    "JSON",
        "transport":   "MQTT",
        "timezone":    "Europe/Berlin"
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
        "intensity"
      ]
    }
  },
  "notification": {
    "http": {
      "url": "http://quantumleap:8668/v2/notify"
    },
    "attrs": [
      "intensity"
    ],
    "metadata": ["dateCreated", "dateModified"]
  },
  "throttling": 1
}'

#StreetLight orion subscription
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
        "type": "StreetLight"
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

#ParkingSpot orion subscription
sudo curl -iX POST \
  'http://localhost:1026/v2/subscriptions/' \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d '{
  "description": "Notify QuantumLeap of ParkingSpot status changes",
  "subject": {
    "entities": [
      {
        "idPattern": ".*",
        "type": "ParkingSpot"
      }
    ],
    "condition": {
      "attrs": [
        "status"
      ]
    }
  },
  "notification": {
    "http": {
      "url": "http://quantumleap:8668/v2/notify"
    },
    "attrs": [
      "status"
    ],
    "metadata": ["dateCreated", "dateModified"]
  },
  "throttling": 1
}'

##ev-charging orion subscription
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
        "type": "EVChargingStation"
      }
    ],
    "condition": {
      "attrs": [
        "parking_spot_status", "capacity","status"
      ]
    }
  },
  "notification": {
    "http": {
      "url": "http://quantumleap:8668/v2/notify"
    },
    "attrs": [
      "parking_spot_status","capacity","status"
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
#   -t "/json/4jkkokgpepnvsb0sd7q21t53tu/StreetLight001/attrs"

# ##send mqtt StreetLight command
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{"power": {"switch" : "on"}}' \
#   -t "/json/4jkkokgpepnvsb0sd7q21t53tu/StreetLight001/cmd"

# #send mqtt charge start message
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{ "charging": { "charge":  "start" } }' \
#   -t "/json/EvAPI/EVChargingStation001/cmd"

# #send mqtt stop charge message
# sudo docker run -it --rm --name mqtt-publisher --network \
#   fiware_default efrecon/mqtt-client pub -h mosquitto -m '{ "charging": { "charge":  "stop" } }' \
#   -t "/json/EvAPI/EVChargingStation001/cmd"

# # #check values in orion street sensors
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
#check values in orion street StreetLight
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:StreetLight:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
#check values in orion parking
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:ParkingSpot:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
 
#  ##CHECK ev charging values in orion 
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:EVChargingStation:001?options=keyValues' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'

 ##CHECK ev charging buttons values in orion 
# sudo curl -X GET \
#   'http://localhost:1026/v2/entities/urn:ngsi-ld:EVChargingStation:002?' \
#   -H 'fiware-service: openiot' \
#   -H 'fiware-servicepath: /'
 



# #check values in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:TrafficFlowObserved:001/attrs/intensity?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'

# #check StreetLight status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:StreetLight:001/attrs/state?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
#check parking status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:ParkingSpot:001/attrs/status?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'

#check ev charging status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:EVChargingStation:001/attrs/parking_spot_status?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
#check ev charging status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:EVChargingStation:001/attrs/capacity?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
#check ev charging status in quantum leap
# sudo curl -X GET \
#   'http://localhost:8668/v2/entities/urn:ngsi-ld:EVChargingStation:002/attrs/status?limit=3' \
#   -H 'Accept: application/json' \
#   -H 'Fiware-Service: openiot' \
#   -H 'Fiware-ServicePath: /'
