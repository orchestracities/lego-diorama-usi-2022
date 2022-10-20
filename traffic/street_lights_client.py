# MQTT Street Lights Client
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'
import paho.mqtt.client as mqtt
import json
from grovepi import *
from time import sleep
from configparser import ConfigParser
# read values from config ini
file = 'config.ini'
config = ConfigParser()
config.read(file)


# topic constructor
def topic_constructor(protocol, service_api_key, sensor_id):
    return ("/" + protocol + "/" + service_api_key + "/" + sensor_id)


def attr_topic(topic):
    return (topic + "/attrs")


def cmd_topic(topic):
    return (topic + "/cmd")


def ack_topic(cmd_topic):
    return (cmd_topic + "exe")


# config connection
broker_address = str(config['connection']['broker_address'])
port = int(config['connection']['port'])
username = str(config['connection']['username'])
password = str(config['connection']['password'])
    
# topic base config
protocol = str(config['connection']['protocol'])
service_api_key = str(config['street_lights']['lights_service_api_key'])

# streetlight 1 def, considered 1 port for 2 led to be changed if 4 led fit in one port
# Use digital pins for led from 2 to 8
sl1 = json.loads(config.get("street_lights", "sl1"))
sl1_id = str(config['street_lights']['sl1_id'])
sl1_topic = topic_constructor(protocol, service_api_key, sl1_id)

# streetlight 2 def considered 1 port for 2 led to be changed if 4 led fit in one port
# Use digital pins for led from 2 to 8
sl2 = json.loads(config.get("street_lights", "sl2"))
sl2_id = str(config['street_lights']['sl2_id'])
sl2_topic = topic_constructor(protocol, service_api_key, sl2_id)

# returns true if operation successfull
def set_lights(setter, pins):
    status = 0
    if (setter == "on"):
        for pin in pins:
            status += digitalWrite(pin, 1)
        if(status == len(pins)):
            return True

    elif (setter == "off"):
        for pin in pins:
            status += digitalWrite(pin, 0)
        if status == len(pins):
            return True
    else:
        print("setter invalid")
        return False

# send light ack
def light_cmd_ack(client, msg, payload, status):
    # prep ack object
    payload['light']['switch'] = status
    # send ack with json
    topic = ack_topic(str(msg.topic))
    print("sending ack to topic:" + str(topic))
    client.publish(ack_topic(str(msg.topic)), json.dumps(payload))


def update_light_status_attribute(client, msg, state):
    topic = attr_topic(str(msg.topic)[:-4])
    print("sending status update to topic: " + str(topic))
    client.publish(attr_topic(
        str(msg.topic)[:-4]), json.dumps({"powerState": state}))

# execute light comand
def light_cmd_exe(msg, client, light_pin):
    payload = json.loads(str(msg.payload))
    try:
        if payload['light']['switch'] == "on":
            print("turning on lights ...")
            if (set_lights("on", light_pin)):
                light_cmd_ack(client, msg, payload, "ok")
                update_light_status_attribute(client, msg, "on")

            else:
                light_cmd_ack(client, msg, payload,
                              "error: failed to turn on lights")
        elif payload['light']['switch'] == "off":
            print("turning off lights ...")
            if (set_lights("off", light_pin)):
                light_cmd_ack(client, msg, payload, "ok")
                update_light_status_attribute(client, msg, "off")
            else:
                light_cmd_ack(client, msg, payload,
                              "error: failed to turn off lights")
    except Exception as err:
        print("error, expected command/argument :" +
              str(err) + ", received: " + str(msg.payload))


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Client successfully connected to broker with result code "+str(rc))
        # Subscribing in on_connect() - if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # subscribe to light topics to listen for lights on
        client.subscribe(cmd_topic(sl1_topic))
        client.subscribe(cmd_topic(sl2_topic))
    else:
        print("Failed connecting to Broker Check credentials and broker address")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" " + str(msg.payload))
    if msg.topic == (cmd_topic(sl1_topic)):
        light_cmd_exe(msg, client, sl1)
    elif msg.topic == (cmd_topic(sl2_topic)):
        light_cmd_exe(msg, client, sl2)
# Create an MQTT client and attach our routines to it.
client = mqtt.Client()
if username is not None or password is not None:
    client.username_pw_set(username,password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address, port, 60)

# Process network traffic and dispatch callbacks. This will also handle
# reconnecting. Check the documentation at
# https://github.com/eclipse/paho.mqtt.python
# for information on how to use other loop*() functions
client.loop_forever()
