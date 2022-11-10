import paho.mqtt.publish as publish
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

cmd_topic(ps_topic)

publish.single(str(cmd_topic(ps_topic)),
               '{ "ev_charging": { "charge":  "start" } }',
               hostname="test.mosquitto.org")
sleep(2)
publish.single(str(cmd_topic(ps_topic)),
               '{ "ev_charging": { "charge":  "stop" } }',
               hostname="test.mosquitto.org")
