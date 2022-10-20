import paho.mqtt.publish as publish
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
# authentication object needed to publish on topic
auth = {'username': username, 'password': password}
# topic base config
protocol = str(config['connection']['protocol'])
service_api_key = str(config['Traffic_counter']['traffic_service_api_key'])

##proximity sensor setup
ps_pin = int(config['parking']['ps_pin'])
ps_id = str(config['parking']['ps_id'])

##led setup
red_l_pin = int(config['parking']['red_l_pin'])
red_l_id = str(config['parking']['red_l_id'])
green_l_pin = int(config['parking']['green_l_pin'])
green_l_id = str(config['parking']['green_l_id'])