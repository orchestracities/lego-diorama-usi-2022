import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
import grovepi
import json
from time import sleep
from configparser import ConfigParser

# read values from config ini
file = 'config.ini'
config = ConfigParser()
config.read(file)


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
auth = None
if username is not None and password is not None:
    if username != "" and password != "":
        # authentication object needed to publish on topic
        auth = {'username': username, 'password': password}
# topic base config
protocol = str(config['connection']['protocol'])
service_api_key = str(config['ev_parking']['ev_service_api_key'])

# proximity sensor setup
ps_pin = int(config['ev_parking']['ps_pin'])
ps_id = str(config['ev_parking']['ps_id'])
ps_topic = topic_constructor(protocol, service_api_key, ps_id)
# distance from which the car is  detected
car_detection_distance = int(config['ev_parking']['car_detection_distance'])
# sleep time
sleep_time = int(config['ev_parking']['sleep_time'])

# led setup
red_l_pin = int(config['ev_parking']['red_l_pin'])
red_l_id = str(config['ev_parking']['red_l_id'])
green_l_pin = int(config['ev_parking']['green_l_pin'])
green_l_id = str(config['ev_parking']['green_l_id'])

# turn off red or green led and trun on the other
# takes as input the pin of the led to turn off

# button
button_pin = int(config['ev_parking']['button_pin'])
button_id =  str(config['ev_parking']['button_id'])
button_topic = topic_constructor(protocol, service_api_key, ps_id)
grovepi.pinMode(button_pin, "INPUT")

#charging flag 0 no charging 1 charging on going
cf = 0

cmd_topic(ps_topic)

publish.single(str(cmd_topic(ps_topic)), '{ "ev_charging": { "charge":  "start" } }', hostname="test.mosquitto.org")
sleep(2)
publish.single(str(cmd_topic(ps_topic)), '{ "ev_charging": { "charge":  "stop" } }', hostname="test.mosquitto.org")
