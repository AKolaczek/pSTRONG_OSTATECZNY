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
#define PRZOD 0
#define TYL 1
#define SKRET_LEWO 2
#define SKRET_PRAWO 3 
#define speedMAX 255
#define speedLOW 180 
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
uint32_t minimalnaSrednia = 0, maksymalnaSrednia = 0, dzielnikWartosci = 0,dzielnikWartosci2 = 0, wartoscInkrementacji = 0, j = 0;
uint32_t readLineMIN = 5000, readLineMAX=0,odczyt,przedzialy=3,inkrementacja=0;


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
    for (uint16_t i = 0; i < 300; i++)
    {
        qtr.calibrate();
        odczyt = qtr.readLineBlack(sensorValues);
        if (readLineMAX < odczyt) {
            readLineMAX = odczyt;
        }
        if (readLineMIN > odczyt) {
            readLineMIN = odczyt;
        }
    }
    digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
    // print the calibration minimum values measured when emitters were on
    Serial.begin(9600);
    for (uint32_t i = 0; i < 8; i++)
    {
        Serial.print(qtr.calibrationOn.minimum[i]);
        Serial.print(' ');
        /*minimalnaSrednia = minimalnaSrednia + (1000 * i * qtr.calibrationOn.minimum[i]);
        dzielnikWartosci = dzielnikWartosci +qtr.calibrationOn.minimum[i];*/
    }
    Serial.println();
    /* WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE
    for (uint32_t i = 0; i < 8; i++) {
        minimalnaSrednia = minimalnaSrednia + (1000 * i * qtr.calibrationOn.minimum[i]);
        dzielnikWartosci =  dzielnikWartosci + (uint32_t) qtr.calibrationOn.minimum[i];
        Serial.print(minimalnaSrednia);
        Serial.print(' ');
    }
    Serial.println();
    Serial.print("minimalna wartosc licznika: ");
    Serial.println(minimalnaSrednia);
    Serial.print("minimalna wartosc dzielnika : ");
    Serial.println(dzielnikWartosci);

    minimalnaSrednia = (uint32_t) ((long double)(minimalnaSrednia) / (long double)(dzielnikWartosci));

    Serial.print("minimalna wartosc sredniezonej wa: ");
    Serial.println(minimalnaSrednia);
    Serial.println();
    */
    // print the calibration maximum values measured when emitters were on
    for (uint32_t i = 0; i < 8; i++)
    {
        Serial.print(qtr.calibrationOn.maximum[i]);
        Serial.print(' ');
        /*maksymalnaSrednia = maksymalnaSrednia +  (1000 * i * qtr.calibrationOn.maximum[i]);
        dzielnikWartosci = dzielnikWartosci + qtr.calibrationOn.minimum[i];*/
    }
    Serial.println();
    /* WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE WAZNE
    for (uint32_t i = 0; i < 8; i++) {
        maksymalnaSrednia = maksymalnaSrednia +  (1000 * i * qtr.calibrationOn.maximum[i]);
        dzielnikWartosci = dzielnikWartosci + (uint32_t) qtr.calibrationOn.minimum[i];
        Serial.print(maksymalnaSrednia);
        Serial.print(' ');
    }

    Serial.println();
    Serial.print("maksymalna wartosc licznika: ");
    Serial.println(maksymalnaSrednia);
    Serial.print("maksymalna wartosc dziekalnika: ");
    Serial.println(dzielnikWartosci);
    Serial.print("maksymalna wartosc sr wazonej: ");
    Serial.println(maksymalnaSrednia);
    maksymalnaSrednia =(uint32_t) ((long double)(maksymalnaSrednia) / (long double)(dzielnikWartosci));
    wartoscInkrementacji =(uint32_t) ((long double) (maksymalnaSrednia - minimalnaSrednia) / 3.);

    Serial.println();
    Serial.print("minimalna wartosc sredniej: ");
    Serial.println(minimalnaSrednia);
    Serial.print("maksymalna wartosc sredniej: ");
    Serial.println(maksymalnaSrednia);
    Serial.print("Inkrementacja: ");
    Serial.println(wartoscInkrementacji);
    */
    inkrementacja = (readLineMAX - readLineMIN) / przedzialy;

    Serial.print("readLineMIN: ");
    Serial.println(readLineMIN);
    Serial.print("readLineMAX: ");
    Serial.println(readLineMAX);
    Serial.print("inkrementacja: ");
    Serial.println(inkrementacja);
    
    for (uint8_t i = 0; i <= 10; i++) {
        miganie();
    }
    
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
    /*
        if(j == 0)
        {
            start(START);
            delay(150);
            ++j;
        }
    */

    if (position >= readLineMIN && position <= inkrementacja)
    {
        start(START);
        delay(20);
        start(SKRET_PRAWO);
    }
    else if (position > inkrementacja && position <= (2*inkrementacja))
    {
        start(START);
        delay(20);
        start(PRZOD);
    }
    else if (position > (2*inkrementacja) && position <= readLineMAX)
    {
        start(START);
        delay(20);
        start(SKRET_LEWO);
    }
    
    delay(500);
}

void start(int direction) {

    if (direction == PRZOD) {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedLOW);
        analogWrite(motor2.PWM, speedLOW);
       
    }
    else if (direction == SKRET_LEWO)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedLOW-50);
        analogWrite(motor2.PWM, speedLOW);
    }
    else if (direction == SKRET_PRAWO)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedLOW);
        analogWrite(motor2.PWM, speedLOW-50);
    }else if(direction == START)
    {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speedMAX);
        analogWrite(motor2.PWM, speedMAX);
    }
}
void miganie() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
}