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
sleep_time = float(config['ev_parking']['sleep_time'])

# led setup
red_l_pin = int(config['ev_parking']['red_l_pin'])
red_l_id = str(config['ev_parking']['red_l_id'])
green_l_pin = int(config['ev_parking']['green_l_pin'])
green_l_id = str(config['ev_parking']['green_l_id'])


# button
button_pin = int(config['ev_parking']['button_pin'])
button_id = str(config['ev_parking']['button_id'])
button_topic = topic_constructor(protocol, service_api_key, button_id)
grovepi.pinMode(button_pin, "INPUT")

# charging flag 0 no charging 1 charging on going
cf = 0


# iteration needed in led blink
red_blink_iterations = -1

# turn off red or green led and trun on the other
# takes as input the pin of the led to turn off


def set_led(led_pin_on):
    # trun on led
    grovepi.digitalWrite(led_pin_on, 1)
    if led_pin_on == red_l_pin:
        grovepi.digitalWrite(green_l_pin, 0)
    else:
        grovepi.digitalWrite(red_l_pin, 0)


def pub_charging_status(
        charging_status,
        sensor_base_topic,
        authentication,
        broker_address,
        port_number):
    # create topic
    pub_topic = attr_topic(sensor_base_topic)
    # create payload object (attr)
    charging_status_obj = {'capacity': charging_status}
    # convert to json
    charging_status_obj = json.dumps(charging_status_obj)
    publish.single(
        pub_topic,
        payload=charging_status_obj,
        hostname=broker_address,
        port=port_number,
        auth=authentication)
    print("published: " + str(charging_status_obj) + " on topic: " + pub_topic)


def pub_button_status(
        button_status,
        sensor_base_topic,
        authentication,
        broker_address,
        port_number):
    # create topic
    pub_topic = attr_topic(sensor_base_topic)
    # create payload object (attr)
    button_status_obj = {'button': button_status}
    # convert to json
    button_status_obj = json.dumps(button_status_obj)
    publish.single(
        pub_topic,
        payload=button_status_obj,
        hostname=broker_address,
        port=port_number,
        auth=authentication)
    print("published: " + str(button_status_obj) + " on topic: " + pub_topic)


def red_led_blink():
    global red_blink_iterations
    global cf

    if red_blink_iterations == 5:
        pub_charging_status(0, ps_topic, auth, broker_address, port)

    if (cf == 1) and (red_blink_iterations >= 0):
        # Blink the LED
        # Send HIGH to switch on LED
        grovepi.digitalWrite(red_l_pin, 1)
        print("LED ON!")
        sleep(1)

        # Send LOW to switch off LED
        grovepi.digitalWrite(red_l_pin, 0)
        print("LED OFF!")
        sleep(1)

        # charging status complete
        if (red_blink_iterations == 0):
            pub_charging_status(1, ps_topic, auth, broker_address, port)
            grovepi.digitalWrite(red_l_pin, 1)

        red_blink_iterations = red_blink_iterations - 1
    # if charge intrerrupt:
    else:
        # grovepi.digitalWrite(red_l_pin, 1)
        print("not charging")


def pub_parking_status(
        parking_spot_status,
        sensor_base_topic,
        authentication,
        broker_address,
        port_number):
    # create topic
    pub_topic = attr_topic(sensor_base_topic)
    # create payload object (attr)
    parking_spot_status_obj = {'parking_spot_status': parking_spot_status}
    # convert to json
    parking_spot_status_obj = json.dumps(parking_spot_status_obj)
    publish.single(
        pub_topic,
        payload=parking_spot_status_obj,
        hostname=broker_address,
        port=port_number,
        auth=authentication)
    print(
        "published: " +
        str(parking_spot_status_obj) +
        " on topic: " +
        pub_topic)


def monitor_parking():
    # initialize parking spot
    parking_spot_status = "free"
    # button flag
    button_pressed = False
    set_led(green_l_pin)
    # ultrasonic sensor
    while True:
        try:

            # Read distance value from Ultrasonic
            car_dist = (grovepi.ultrasonicRead(ps_pin))

            if (car_dist > car_detection_distance):
                set_led(green_l_pin)
            else:
                set_led(red_l_pin)

            print("distace: " + str(car_dist))
            # if car detected and parking spot free
            if car_dist <= car_detection_distance and parking_spot_status == "free":
                # set parking status to occupied
                parking_spot_status = "occupied"
                # send message to update parking status
                pub_parking_status(parking_spot_status,
                                   ps_topic, auth, broker_address, port)
                # turn on red and turn off green
                set_led(red_l_pin)
            elif car_dist > car_detection_distance and parking_spot_status == "occupied":
                # set parking status to free
                parking_spot_status = "free"
                # send message to update parking status
                pub_parking_status(parking_spot_status,
                                   ps_topic, auth, broker_address, port)
                # turn on red turn of green
                set_led(green_l_pin)
            # used to limit requests to public mqtt broker to avoid getting
            # banned to be removed when using private mqtt broker
            if (grovepi.digitalRead(button_pin) ==
                    1 and parking_spot_status == "occupied"):
                    pub_button_status("pressed",
                    button_topic, auth, broker_address, port)
                    button_pressed = True

            if (parking_spot_status == "occupied"):
                red_led_blink()
            if (button_pressed == True and grovepi.digitalRead(button_pin) != 1):
                button_pressed = False
                pub_button_status("released", button_topic, auth, broker_address, port)

            sleep(sleep_time)
        except TypeError:
            print("Error")
        except IOError:
            print("Error")


def ps_cmd_ack(client, msg, payload, status):
    # prep ack object
    payload['ev_charging']['charge'] = status
    # send ack with json
    topic = ack_topic(str(msg.topic))
    print("sending ack to topic:" + str(topic))
    client.publish(ack_topic(str(msg.topic)), json.dumps(payload))


def ps_cmd_exe(msg, client, ps_pin):
    global cf
    global red_blink_iterations
    data = msg.payload.decode("utf-8")
    payload = json.loads(data)

    try:
        # if message to start charge received
        if payload['ev_charging']['charge'] == "start":
            print("start charging ...")
            # start charging
            cf = 1
            red_blink_iterations = 5
            # send ack
            ps_cmd_ack(client, msg, payload, "ok")
            # red_led_blink()
            # update_light_status_attribute(client, msg, "on")
        elif payload['ev_charging']['charge'] == "stop":
            print("interrupting charge ...")
            cf = 0
            red_blink_iterations = -1
            ps_cmd_ack(client, msg, payload, "ok")
            # update_light_status_attribute(client, msg, "off")
    except Exception as err:
        print("error, expected command/argument :" +
              str(err) + ", received: " + str(msg.payload))

# define client
# The callback for when the client receives a CONNACK response from the server.


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Client successfully connected to broker with result code " + str(rc))
        # Subscribing in on_connect() - if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # subscribe to light topics to listen for lights on
        client.subscribe(cmd_topic(ps_topic))
    else:
        print("Failed connecting to Broker Check credentials and broker address")

# The callback for when a PUBLISH message is received from the server.


def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    if msg.topic == (cmd_topic(ps_topic)):
        ps_cmd_exe(msg, client, ps_pin)


# Create an MQTT client and attach our routines to it.
client = mqtt.Client()
if (username is not None and password is not None) or (
        username != "" and password != ""):
    client.username_pw_set(username, password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address, port, 60)

# start client loop on separate thread the main will continue
# disconnect and messages check handled independently
client.loop_start()

monitor_parking()
