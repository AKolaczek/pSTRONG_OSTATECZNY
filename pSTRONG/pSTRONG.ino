/*
 Name:		sterowanieQTR.ino
 Created:	2/20/2022 11:25:06 AM
 Author:	hp
*/

#include <QTRSensors.h>

// This example is designed for use with six analog QTR sensors. These
// reflectance sensors should be connected to analog pins A0 to A5. The
// sensors' emitter control pin (CTRL or LEDON) can optionally be connected to
// digital pin 2, or you can leave it disconnected and remove the call to
// setEmitterPin().
//
// The setup phase of this example calibrates the sensors for ten seconds and
// turns on the Arduino's LED (usually on pin 13) while calibration is going
// on. During this phase, you should expose each reflectance sensor to the
// lightest and darkest readings they will encounter. For example, if you are
// making a line follower, you should slide the sensors across the line during
// the calibration phase so that each sensor can get a reading of how dark the
// line is and how light the ground is. Improper calibration will result in
// poor readings.
//
// The main loop of the example reads the calibrated sensor values and uses
// them to estimate the position of a line. You can test this by taping a piece
// of 3/4" black electrical tape to a piece of white paper and sliding the
// sensor across it. It prints the sensor values to the serial monitor as
// numbers from 0 (maximum reflectance) to 1000 (minimum reflectance) followed
// by the estimated location of the line as a number from 0 to 5000. 1000 means
// the line is directly under sensor 1, 2000 means directly under sensor 2,
// etc. 0 means the line is directly under sensor 0 or was last seen by sensor
// 0 before being lost. 5000 means the line is directly under sensor 5 or was
// last seen by sensor 5 before being lost.
#define przod 0
#define tyl 1
#define SKRET_LEWO 2
#define SKRET_PRAWO 3 
#define speedMAX 128
#define speedLOW 75 

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
int minimalnaSrednia, maksymalnaSrednia, dzielnikWartosci, zakres, wartoscInkrementacji;

void setup()
{
    //configure the motors
    // pinMode(LED_BUILTIN, OUTPUT);
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
    // configure the sensors
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
        minimalnaSrednia += 1000 * i * qtr.calibrationOn.minimum[i];
        dzielnikWartosci += qtr.calibrationOn.minimum[i];
    }
    minimalnaSrednia = minimalnaSrednia / dzielnikWartosci;
    Serial.println();

    // print the calibration maximum values measured when emitters were on
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(qtr.calibrationOn.maximum[i]);
        Serial.print(' ');
        maksymalnaSrednia += 1000 * i * qtr.calibrationOn.maximum[i];
        dzielnikWartosci += qtr.calibrationOn.minimum[i];
    }
    maksymalnaSrednia = maksymalnaSrednia / dzielnikWartosci;
    zakres = maksymalnaSrednia - minimalnaSrednia;
    wartoscInkrementacji = zakres / 3;
    Serial.println();
    Serial.println();
    delay(10000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5000);
    digitalWrite(LED_BUILTIN, LOW);
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

    if (position >= minimalnaSrednia && position <= (minimalnaSrednia + wartoscInkrementacji))
    {
        start(2);
    }
    else if (position >= (minimalnaSrednia + wartoscInkrementacji) <= (minimalnaSrednia + (2 * wartoscInkrementacji)))
    {
        start(0);
    }
    else if (position >= (minimalnaSrednia + (2 * wartoscInkrementacji)) <= (minimalnaSrednia + (3 * wartoscInkrementacji)))
    {
        start(3);
    }

    delay(250);
}
void start(bool direction) {

    if (direction == 0) {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedMAX);
        analogWrite(motor2.PWM, speedMAX);
    }
    else if (direction == SKRET_LEWO)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedLOW);
        analogWrite(motor2.PWM, speedMAX);
    }
    else if (direction == SKRET_PRAWO)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedMAX);
        analogWrite(motor2.PWM, speedLOW);
    }
}