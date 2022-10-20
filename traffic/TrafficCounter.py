import paho.mqtt.publish as publish
import grovepi
import json
from time import sleep
from configparser import ConfigParser 



# NOTE:
# The sensitivity can be adjusted by the onboard potentiometer
# Calibration
#       Hold the sensor facing an empty dark area
#       Adjust the potentiometer until the onboard LED switches off
#       When the LED switches off the sensor is calibrated
#       If not, adjust the potentiometer again
#IMPORTANT: When assigning the port to the sensor assign the next port
#           from where the sensor has been connected
#           e.g.: if sensor connectet to d4 assign the sensor to port d5
#           otherweise the sensor won't work. also leave port d5 and d3 free
#           to ensure the sensor works correctly

# Connect the Grove Infrared Distance Interrupt Sensor (ir) to digital port D5
# (see above note)

##read values from config ini   
file = 'config.ini'
config = ConfigParser()
config.read(file)


##topic constructor
def topic_constructor(protocol, service_api_key, sensor_id):
   return ("/" + protocol + "/" + service_api_key + "/" + sensor_id)

def attr_topic(topic):
    return (topic + "/attrs")

def cmd_topic(topic):
    return (topic + "/cmd")

def ack_topic(cmd_topic):
    return (cmd_topic + "exe")


#config connection
broker_address=str(config['connection']['broker_address'])
port = int(config['connection']['port'])
username = str(config['connection']['username'])
password = str(config['connection']['password'])
##authentication object needed to publish on topic
auth = {'username':username, 'password':password}
##topic base config
protocol = "json" ## str(config['connection']['protocol'])
service_api_key = "test_key"#str(config['Street 2']['service_api_key'])

##ir Sensor data and functions
##Connect the ir sensor at port d6 NOT d7 (see initial note)
ir_pin = 7 #str(config['Street 1']['ir_pin'])
grovepi.pinMode(ir_pin,"INPUT")
ir_id = "ir" #str(config['Street 1']['ir_id'])
ir_topic = topic_constructor(protocol, service_api_key, ir_id)
# ir_car_counter = 0




## Obstacle sensor data and function
## connect the obstacle sensor to port d8
os_pin = 8
grovepi.pinMode(os_pin,"INPUT")
os_id = "os" #str(config['Street 2']['os_id'])
os_topic = topic_constructor(protocol, service_api_key, os_id)
# os_car_counter = 0

##update sensor counter
def update_counter(sensor_pin, counter):
    if sensor_pin == ir_pin:
        try:
            # Sensor returns LOW and onboard LED lights up when the
            # received infrared light intensity exceeds the calibrated level
            if grovepi.digitalRead(ir_pin) == 1:
              #increase counter if car found
              counter += 1
            else:
               print ("ir sensor: no car detectected")
        except IOError:
            print ("IO Error")
    elif sensor_pin == os_pin:
        try:
             ## obstacle sensor no obstacle:   
            if grovepi.digitalRead(os_pin) == 1:
                print ("os_sendor: no car detected")
            ###obstacle:
            else:
                counter += 1
        except IOError:
            print ("IO Error")
    else:
        print("Passed invalid sensor pin: " + str(sensor_pin) + ", valid pins: " + str(os_pin) + ", " + str(ir_pin) + "\n counter update failed returnin passed counter:" )
    return counter




##publish sensor counter
def pub_counter(counter,  sensor_base_topic,  authentication, broker_address, port_number):
    ##create topic
    pub_topic = attr_topic(sensor_base_topic)
    #create payload object (attr)
    counter_obj = {"car_counter" : counter}
    #convert to json
    counter_obj = json.dumps(counter_obj)
    publish.single(pub_topic, payload=counter_obj, hostname=broker_address, port=port_number, auth=authentication)
    print("published: " +str(counter) + " on topic: " + pub_topic)


def monitor_traffic(ir_pin, os_pin):
    ir_car_counter = 0
    os_car_counter = 0
    while True:
        ir_car_counter = update_counter(ir_pin, ir_car_counter)
        os_car_counter = update_counter(os_pin, os_car_counter)
        #publish counter
        pub_counter(ir_car_counter, ir_topic, None, broker_address, port) #authentication not used for now change none with auth param when using private broker
        pub_counter(os_car_counter, os_topic, None, broker_address, port) #authentication not used for now change none with auth param when using private broker
        sleep(2) #used to limit traffic to public mqtt server
            


##start monitoring

monitor_traffic(ir_pin, os_pin)