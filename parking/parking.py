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
auth = None
if username is not None and password is not None:
    if username != "" and password != "":
        # authentication object needed to publish on topic
        auth = {'username': username, 'password': password}
# topic base config
protocol = str(config['connection']['protocol'])
service_api_key = str(config['parking']['parking_service_api_key'])

# proximity sensor setup
ps_pin = int(config['parking']['ps_pin'])
ps_id = str(config['parking']['ps_id'])
ps_topic = topic_constructor(protocol, service_api_key, ps_id)
# distance from which the car is  detected
car_detection_distance = int(config['parking']['car_detection_distance'])


# led setup
red_l_pin = int(config['parking']['red_l_pin'])
red_l_id = str(config['parking']['red_l_id'])
green_l_pin = int(config['parking']['green_l_pin'])
green_l_id = str(config['parking']['green_l_id'])

# turn off red or green led and trun on the other
# takes as input the pin of the led to turn off


def set_led(led_pin_on):
    # trun on led
    grovepi.digitalWrite(led_pin_on, 1)
    if led_pin_on == red_l_pin:
        grovepi.digitalWrite(green_l_pin, 0)
    else:
        grovepi.digitalWrite(red_l_pin, 0)

# publish parking status


def pub_parking_status(parking_spot_status,  sensor_base_topic,  authentication, broker_address, port_number):
    # create topic
    pub_topic = attr_topic(sensor_base_topic)
    # create payload object (attr)
    parking_spot__status_obj = {'parking_spot_status': parking_spot_status}
    # convert to json
    parking_spot__status_obj = json.dumps(parking_spot__status_obj)
    publish.single(pub_topic, payload=parking_spot__status_obj,
                   hostname=broker_address, port=port_number, auth=authentication)
    print("published: " + str(parking_spot__status_obj) + " on topic: " + pub_topic)


def monitor_parking():
    parking_spot_status = "free"
    # ultrasonic sensor
    while True:
        try:
            # Read distance value from Ultrasonic
            car_dist = (grovepi.ultrasonicRead(ps_pin))
            print("distace: " + str(car_dist))
            # if car detected and parking spot free
            if car_dist <= car_detection_distance and parking_spot_status == "free":
                # set parking status to occupied
                parking_spot_status = "occupied"
                # send message to update parking status
                pub_parking_status(parking_spot_status,
                                   ps_topic, auth, broker_address, port)
                # turn on red turn of green
                set_led(red_l_pin)
            elif car_dist > car_detection_distance and parking_spot_status == "occupied":
                # set parking status to free
                parking_spot_status = "free"
                # send message to update parking status
                pub_parking_status(parking_spot_status,
                                   ps_topic, auth, broker_address, port)
                # turn on red turn of green
                set_led(green_l_pin)
            # used to limit requests to public mqtt broker to avoid getting banned to be removed when using private mqtt broker
            sleep(2)
        except TypeError:
            print("Error")
        except IOError:
            print("Error")


monitor_parking()
