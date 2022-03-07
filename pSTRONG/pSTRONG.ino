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
#define MAX_SPEED_ENGINE_1 200
#define MAX_SPEED_ENGINE_2 200
//teraz moj komentarz
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
double KP = 0.1;
double KD = 5;

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
    delay(5000);
}

void loop()
{
    static uint16_t lastError = 0;
    // read calibrated sensor values and obtain a measure of the line position
    // from 0 to 5000 (for a white line, use readLineWhite() instead)
    uint16_t position = qtr.readLineBlack(sensorValues);
    int16_t error = position - 3500;

    // print the sensor values as numbers from 0 to 1000, where 0 means maximum
    // reflectance and 1000 means minimum reflectance, followed by the line
    // position
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.println(position);
    int16_t motorSpeed = KP * error + KD * (error - lastError);
    lastError = error;
    int16_t m1Speed = MAX_SPEED_ENGINE_1 + motorSpeed;
    int16_t m2Speed = MAX_SPEED_ENGINE_2 - motorSpeed;
    // It might help to keep the speeds positive (this is optional). You might
    // also want to add a similar check to keep the speeds from exceeding a
    // maximum limit.
    if (m1Speed < 0) { m1Speed = 0; }
    if (m2Speed < 0) { m2Speed = 0; }
    // set motor speeds using the two motor speed variables above
    if (m1Speed < 0) { m1Speed = 0; }
    if (m2Speed < 0) { m2Speed = 0; }
    if (m1Speed > MAX_SPEED_ENGINE_1) { m1Speed = MAX_SPEED_ENGINE_1; }
    if (m2Speed > MAX_SPEED_ENGINE_2) { m2Speed = MAX_SPEED_ENGINE_2; }
}


