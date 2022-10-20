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
#           otherweise the sensor won't work. also leave port d5 free
#           to ensure the sensor works correctly

# Connect the Grove Infrared Distance Interrupt Sensor to digital port D5
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
protocol = "json"
attrs ="attrs"
service_api_key = "test_key"#str(config['Street 2']['service_api_key'])
cmd="cmd"

##ir Sensor data and functions
##Connect the ir sensor at port d5 (see initial note)
ir_pin = 6
grovepi.pinMode(ir_pin,"INPUT")
ir_id = "ir"
ir_topic = topic_constructor(protocol, service_api_key, ir_id)




def pub_counter(counter,  sensor_base_topic,  authentication, broker_address, port_number):
    ##create topic
    pub_topic = attr_topic(sensor_base_topic)
    #create payload object (attr)
    counter_obj = {"car_counter" : counter}
    #convert to json
    counter_obj = json.dumps(counter_obj)
    publish.single(pub_topic, payload=counter_obj, hostname=broker_address, port=port_number, auth=authentication)



def monitor_traffic(ir_pin,s1_car_counter):
    while True:
        try:
            # Sensor returns LOW and onboard LED lights up when the
            # received infrared light intensity exceeds the calibrated level
            if grovepi.digitalRead(ir_pin) == 0:
                ##increase counter if car found
              s1_car_counter += 1
                ## send signal to turn on lights
            else:
               print ("no car detectected")
            sleep(5) ##used to not overload pubblic mqtt server change this when personal mqtt is used 

        except IOError:
            print ("Error")

## Obstacle sensor data and function
## connect the obstacle sensor to port d8
obstacle_sensor = 8
s2_car_counter = 0





