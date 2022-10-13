# MQTT Client demo
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'
import paho.mqtt.client as mqtt
import json
from grovepi import *
from time import sleep
from configparser import ConfigParser 
##read values from config ini   
file = 'config.ini'
config = ConfigParser()
config.read(file)

##############Functions
##topic constructor
def topic_constructor(protocol, service_api_key, sensor_id, attr):
   return ("/" + protocol + "/" + service_api_key + "/" + sensor_id + "/" + attr)

def ack_topic(topic):
    return (topic + "exe")

#config connection
broker_address=str(config['connection']['broker_address'])
port = int(config['connection']['port'])
username = str(config['connection']['username'])
password = str(config['connection']['password'])
##topic base config
protocol = "json"
attrs ="attrs"
service_api_key = "test_key"#str(config['Street 2']['service_api_key'])
cmd="cmd"

##streetlight 1 def considered 1 port for 2 led to be changed if 4 led fit in one port
## Use digital pins for led from 2 to 8
sl1 = [2,3]
sl1_id = "sl1"
sl1_topic = topic_constructor(protocol, service_api_key, sl1_id, cmd)

##streetlight 2 def considered 1 port for 2 led to be changed if 4 led fit in one port
## Use digital pins for led from 2 to 8
sl2 = [2,3]
sl2_id = "sl2"
sl2_topic = topic_constructor(protocol, service_api_key, sl1_id, cmd)

###debug
print(sl2)
print(sl1)

##returns true if operation successfull
def set_lights(setter, pins):
    status = 0
    if (setter == "on"):
        for pin in pins:
            status += digitalWrite(pin,1)
        if(status == len(pins)):
            return True
        
    elif (setter == "off"):
        for pin in pins:
            status += digitalWrite(pin,0)
            if status == 0:
                return True
    else :
        print("setter invalid")
        return False

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
        if rc == 0:
                print("Client successfully connected to broker with result code "+str(rc))
                # Subscribing in on_connect() - if we lose the connection and
                # reconnect then subscriptions will be renewed.
                #subscribe to light topics to listen for lights on
                client.subscribe(sl1_topic) 
                client.subscribe(sl2_topic) 
        else:
                print("Failed connecting to Broker Check credentials and broker address")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    payload = json.loads(str(msg.payload))
    try:
        if msg.topic == (sl1_topic):
            if payload['light']['switch'] == "on":
                print("turning on lights on street 1 ...")
                if (set_lights("on", sl1)):
                    ##prep ack object
                    payload['light']['switch'] = "ok"
                    ##send ack with json
                    client.publish(ack_topic(str(msg.topic)), json.dumps(payload))
                    ##TODO error Handling
            elif payload['light']['switch'] == "off":
                print("turning off lights on street 1 ...")
                if (set_lights("off", sl1)):
                    ##prep ack object
                    payload['light']['switch'] = "ok off"
                    ##send ack with json
                    client.publish(ack_topic(str(msg.topic)), json.dumps(payload))
                    ##TODO error Handling
            

        if msg.topic == (sl1_topic):
            if payload['light']['switch'] == "on":
                print("turning on lights on street 2 ...")
                if (set_lights("on", sl2)):
                    ##prep ack object
                    payload['light']['switch'] = "ok"
                    ##send ack with json
                    client.publish(ack_topic(msg.topic), json.dumps(payload))
                    ##TODO error Handling
                elif payload['light']['switch'] == "off":
                    print("turning off lights on street 2 ...")
                    if (set_lights("off", sl2)):
                        ##prep ack object
                        payload['light']['switch'] = "ok off"
                        ##send ack with json
                        client.publish(ack_topic(str(msg.topic)), json.dumps(payload))
                        ##TODO error Handling
    except:
        print("something went wrong")

# Create an MQTT client and attach our routines to it.
client = mqtt.Client()
#client.username_pw_set(username,password) #uncomment when using local broker
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address, port, 60)
 
# Process network traffic and dispatch callbacks. This will also handle
# reconnecting. Check the documentation at
# https://github.com/eclipse/paho.mqtt.python
# for information on how to use other loop*() functions
client.loop_forever()


