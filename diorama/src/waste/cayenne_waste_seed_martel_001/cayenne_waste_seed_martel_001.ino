// code for seed-martel-001

// Temp sensor on D5
// Fire sensor on D2
// Dist Sensor on D3
// Accelerometer on I2C

#include <CayenneLPP.h>
#include <LoRaWan.h>
CayenneLPP lpp(100);

#include <Wire.h>

#include "DHT.h"
#include "MMA7660.h"
#include "Ultrasonic.h"

#define DEBUG 1 // 1 to show debugging print in serial monitor, 0 to hide them

// accelerometer def
MMA7660 accelerometer;

// Temp&Hum def
#define DHTTYPE DHT22 // DHT 22  (AM2302)
#define DHTPIN 5      // what pin we're connected to（DHT10 and DHT20 don't need define it）
DHT dht(DHTPIN, DHTTYPE);

// digital pins def
#define FLAME_SENSOR 2 // connect SENSOR to digital pin1

Ultrasonic ultrasonic(3); // Distance Sensor

char buffer[256];

// Waste Container Statuses
#define ST_OK 0
#define ST_LID_OPEN 1
#define ST_DROPPED 2
#define ST_BURNING 5

// status
int status = ST_OK;

// filling level
const int FL_HEIGHT_CM = 10;

// MEASURE VALUES

// Temp&HumSensor
float temperature = 0, humidity = 0;

// Accelerometer
int8_t x = 0, y = 0, z = 0;
float ax = 0, ay = 0, az = 0;

// DistanceSensor
float fillingLevel = 0;

void setup(void) {
    SerialUSB.begin(115200);
    // while (!SerialUSB)
    //   ;

    // Lorawan Chip Config
    lorawanConfig();

    // Accelerometer
    accelerometerSetup();

    // Temperature & Humidity
    tempHumSetup();

    // FlameSensor
    pinMode(FLAME_SENSOR, INPUT);
}

void loop(void) {
    lpp.reset(); // reset lpp to send new message

    // Temp&HumSensor
    tempHumLoop();

    lpp.addTemperature(1, temperature);
    lpp.addRelativeHumidity(1, humidity);

    // FlameSensor
    lpp.addDigitalInput(1, isFlameDetected());

    // DistanceSensor
    distanceLoop();
    lpp.addDigitalInput(2, fillingLevel);

    // Accelerometer
    accelerometerLoop();

    lpp.addAccelerometer(1, x, y, z);
    lpp.addAccelerometer(2, ax, ay, az);

    getStatus();
    lpp.addDigitalInput(3, status);

    bool msg1 = lora.transferPacketWithConfirmed(lpp.getBuffer(), lpp.getSize(), 5);
    delay(4000);
}

// settings for the connection with the gateway
void lorawanConfig() {
    lora.init();

    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer);

    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);

    // void setId(char *DevAddr, char *DevEUI, char *AppEUI);
    lora.setId(NULL, "70B3D57ED00561EC", "8CF957200005727C");
    // setKey(char *NwkSKey, char *AppSKey, char *AppKey);
    lora.setKey(NULL, NULL, "51F58CAC3F5735E3D1F88DD3EADBE9C6");

    lora.setDeciveMode(LWOTAA);
    lora.setDataRate(DR0, EU868);

    lora.setChannel(0, 868.1);
    lora.setChannel(1, 868.3);
    lora.setChannel(2, 868.5);

    lora.setReceiceWindowFirst(0, 868.1);
    lora.setReceiceWindowSecond(869.5, DR3);

    lora.setDutyCycle(false);
    lora.setJoinDutyCycle(false);

    lora.setPower(14);

    while (!lora.setOTAAJoin(JOIN))
        ;
}

// FLAME SENSOR
bool isFlameDetected() {
    if (DEBUG){
      Serial.println("Flame:");
      Serial.println(!digitalRead(FLAME_SENSOR));
    }
    return !digitalRead(FLAME_SENSOR);
}

// ACCELEROMETER
void accelerometerSetup() {
    accelerometer.init();
    Serial.begin(9600);
}

void accelerometerLoop() {
    accelerometer.getXYZ(&x, &y, &z);

    if (DEBUG) {
        Serial.print("x = ");
        Serial.println(x);
        Serial.print("y = ");
        Serial.println(y);
        Serial.print("z = ");
        Serial.println(z);
    }

    accelerometer.getAcceleration(&ax, &ay, &az);

    if (DEBUG) {
        Serial.println("accleration of X/Y/Z: ");
        Serial.print(ax);
        Serial.println(" g");
        Serial.print(ay);
        Serial.println(" g");
        Serial.print(az);
        Serial.println(" g");
        Serial.println("*************");
    }
}

// TEMP & HUM
void tempHumSetup() {
    Wire.begin();
    dht.begin();
}

void tempHumLoop() {
    float temp_hum_val[2] = {0};
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

    if (!dht.readTempAndHumidity(temp_hum_val)) {
        temperature = temp_hum_val[1];
        humidity = temp_hum_val[0];
        if (DEBUG) {
            Serial.println("collected temp/hum");
            Serial.println(temperature);
            Serial.println(humidity);
        }
    } else {
        temperature = -1;
        humidity = -1;
        if (DEBUG) {
            Serial.println("failed to collect temp/hum");
            Serial.println(temperature);
            Serial.println(humidity);
        }
    }
}

// DISTANCE SENSOR
void distanceLoop() {
    fillingLevel = (FL_HEIGHT_CM - (int)ultrasonic.MeasureInCentimeters())/(float)FL_HEIGHT_CM;
    // RangeInCentimeters = (int)ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
    if (DEBUG) {
        Serial.println("Filling percentage: ");
        Serial.println(fillingLevel);
    }
}

void getStatus() {
    if (isFlameDetected()) {
        status = ST_BURNING;
    } else {
        if ((-0.20 < ax && ax < 0.20) && (-0.20 < ay && ay < 0.20) && (-1.20 < az && az < -0.80)) {
            status = ST_OK;
        } else if ((-1.20 < ax && ax < -0.80) && (-0.20 < ay && ay < 0.20) && (-0.20 < az && az < 0.20)) {
            status = ST_LID_OPEN;
        } else {
            status = ST_DROPPED;
        }
    }

    if (DEBUG) {
        Serial.println("Status: ");
        Serial.println(status);
    }
}