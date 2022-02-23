/*
 Name:		sterowanieQTR.ino
 Created:	2/20/2022 11:25:06 AM
 Author:	hp
*/

#include <QTRSensors.h>

#define PRZOD 0
#define TYL 1
#define SKRET_LEWO 2
#define SKRET_PRAWO 3 
#define speedMAX 255
#define speedLOW 255
#define START 4

struct motor
{
    int output1;
    int output2;
    int PWM;
};
struct motor motor1;
struct motor motor2;
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];


void setup()
{
    // configure the sensors
    motor1.PWM = 5;
    motor1.output1 = 7;
    motor1.output2 = 8;
    motor2.PWM = 6;
    motor2.output1 = 4;
    motor2.output2 = 3;
    pinMode(motor1.PWM, OUTPUT);
    pinMode(motor1.output1, OUTPUT);
    pinMode(motor1.output2, OUTPUT);
    pinMode(motor2.PWM, OUTPUT);
    pinMode(motor2.output1, OUTPUT);
    pinMode(motor2.output2, OUTPUT);
    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]) { A0, A1, A2, A3, A4, A5, A6, A7 }, SensorCount);
    //qtr.setEmitterPin(2);

    delay(500);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode

    // analogRead() takes about 0.1 ms on an AVR.
    // 0.1 ms per sensor * 4 samples per sensor read (default) * 6 sensors
    // * 10 reads per calibrate() call = ~24 ms per calibrate() call.
    // Call calibrate() 400 times to make calibration take about 10 seconds.
    for (uint16_t i = 0; i < 400; i++)
    {
        qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration

    // print the calibration minimum values measured when emitters were on
    Serial.begin(9600);
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(qtr.calibrationOn.minimum[i]);
        Serial.print(' ');
    }
    Serial.println();

    // print the calibration maximum values measured when emitters were on
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(qtr.calibrationOn.maximum[i]);
        Serial.print(' ');
    }
    Serial.println();
    Serial.println();
    delay(1000);
}

void loop()
{
    // read calibrated sensor values and obtain a measure of the line position
    // from 0 to 5000 (for a white line, use readLineWhite() instead)
    uint16_t position = qtr.readLineBlack(sensorValues);

    // print the sensor values as numbers from 0 to 1000, where 0 means maximum
    // reflectance and 1000 means minimum reflectance, followed by the line
    // position
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.println(position);
    int16_t error = position - 3500;
    start(PRZOD);
    if (error < -1000)
    {
        // the line is on the left
        start(SKRET_LEWO);  // turn left
    }
    else if (error > 1000)
    {
        // the line is on the right
        start(SKRET_PRAWO);  // turn right
    }
    // set motor speeds using the two motor speed variables above
}


void start(int direction) {

    if (direction == PRZOD) {
        digitalWrite(motor1.output1, HIGH);
        digitalWrite(motor1.output2, LOW);
        digitalWrite(motor2.output1, HIGH);
        digitalWrite(motor2.output2, LOW);

        analogWrite(motor1.PWM, speedLOW);
        analogWrite(motor2.PWM, speedLOW);



    }
    else if (direction == SKRET_LEWO)
    {
        digitalWrite(motor1.output1, HIGH);
        digitalWrite(motor1.output2, LOW);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedMAX);
        analogWrite(motor2.PWM, 0);

    }
    else if (direction == SKRET_PRAWO)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, HIGH);
        digitalWrite(motor2.output2, LOW);

        analogWrite(motor1.PWM, 0);
        analogWrite(motor2.PWM, speedMAX);

    }
    else if (direction == START)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedMAX);
        analogWrite(motor2.PWM, speedMAX);
    }
    delay(40);
    stop();
}

void stop()
{
    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, HIGH);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, HIGH);

    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
}