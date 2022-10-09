    /******************************************************************************/
 
#define FLAME_SENSOR 2 //connect SENSOR to digital pin2
#define LED 3 //connect Grove - LED to pin3
#define DEBUG 0 //1 to show debugging print in serial monitor, 0 to hide them
 
void setup()
{
    pinsInit();
}
void loop()
{
    if(isFlameDetected())
    turnOnLED();
    else turnOffLED();
    delay(1000);
}
    /********************************/
void pinsInit()
{
    Serial.begin(9600);
    pinMode(FLAME_SENSOR, INPUT);
    // pinMode(LED,OUTPUT);
    // digitalWrite(LED,LOW);
}
void turnOnLED()
{
    //digitalWrite(LED,HIGH);
    Serial.println("flame");
}
void turnOffLED()
{
    //digitalWrite(LED,LOW);
    Serial.println("nothing");
}
bool isFlameDetected()
{
    if(DEBUG) Serial.println(!digitalRead(FLAME_SENSOR));
    return !digitalRead(FLAME_SENSOR);
}
