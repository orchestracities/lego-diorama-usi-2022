# MQTT Client demo
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'
 
import paho.mqtt.client as mqtt
from grovepi import *
from time import sleep
from configparser import ConfigParser 

file = 'config.ini'
config = ConfigParser()
config.read(file)
#config connection
broker_address=str(config['connection']['broker_address'])
port = int(config['connection']['port'])
username = str(config['connection']['username'])
password = str(config['connection']['password'])
#######street1 config
#street led 1 topic
vcln_topic=str(config['Street 1']['vcln_topic'])
sl1_topic = str(config['Street 1']['lt'])
vcln_pin = int(config['Street 1']['vcln_pin'])
#led pins
l1_pin = int(config['Street 1']['l1_pin'])
l2_pin = int(config['Street 1']['l2_pin'])
l3_pin = int(config['Street 1']['l3_pin'])
l4_pin = int(config['Street 1']['l4_pin'])
#######street2 config
#street led 1 topic
os_topic = str(config['Street 2']['os_topic'])
sl2_topic= str(config['Street 2']['lt'])
os_pin = int(config['Street 2']['os_pin'])
l5_pin = int(config['Street 2']['l1_pin'])
l6_pin = int(config['Street 2']['l2_pin'])
l7_pin = int(config['Street 2']['l3_pin'])
l8_pin = int(config['Street 2']['l4_pin'])

##flag to see when connection active
connected = False
##keep car count
car_counter = 0

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
        if rc == 0:
                print("Client successfully connected to broker with result code "+str(rc))
                # Subscribing in on_connect() - if we lose the connection and
                # reconnect then subscriptions will be renewed.
                #subscribe to light topics to listen for lights on
                client.subscribe(sl1_topic) 
                client.subscribe(sl2_topic) 
                connected = True
        else:
                print("Failed connecting to Broker Check credentials and broker address")
                connected = False 

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

    if msg.payload == "turn on light" and msg.topic == sl1_topic:
        print("turning on lights on street 1 ...")
        l1 = digitalWrite(l1_pin,1)
        l2 = digitalWrite(l2_pin,1)
        l3 = digitalWrite(l3_pin,1)
        l4 = digitalWrite(l4_pin,1)
        if l1==l2 and l2==l3 and l3==l4 and l4==1 :
            client.publish(sl1_topic, "light on")
        ### turn off light after 5 sec
            sleep(5)
            l1 = digitalWrite(l1_pin,0)
            l2 = digitalWrite(l2_pin,0)
            l3 = digitalWrite(l3_pin,0)
            l4 = digitalWrite(l4_pin,0)

    if msg.payload == "turn on light" and msg.topic == sl2_topic:
        print("turning on lights on street 1 ...")
        l5 = digitalWrite(l5_pin,1)
        l6 = digitalWrite(l6_pin,1)
        l7 = digitalWrite(l7_pin,1)
        l8 = digitalWrite(l8_pin,1)
        if l5==l6 and l6==l7 and l7==l8 and l8==1 :
            client.publish(sl2_topic, "light on")
            ### turn off light after 5 sec
            sleep(5)
            l5 = digitalWrite(l5_pin,0)
            l6 = digitalWrite(l6_pin,0)
            l7 = digitalWrite(l7_pin,0)
            l8 = digitalWrite(l8_pin,0)

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


