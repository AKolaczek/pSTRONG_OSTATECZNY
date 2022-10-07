// https://pololu.github.io/qtr-sensors-arduino/class_q_t_r_sensors.html opis biblioteki
// https://pololu.github.io/qtr-sensors-arduino/md_usage.html zastosowanie

#include <QTRSensors.h>

QTRSensors qtr;

#define motor1_1 6
#define motor1_2 7
#define motor1_pwm 8
#define motor2_1 9
#define motor2_2 10
#define motor2_pwm 11

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
double KP = 0.1;
int16_t KD = 5;
int16_t PWM = 255;

void silnik_set() {
    pinMode(motor1_1, OUTPUT);
    pinMode(motor1_2, OUTPUT);
    pinMode(motor1_pwm, OUTPUT);
    pinMode(motor2_1, OUTPUT);
    pinMode(motor2_2, OUTPUT);
    pinMode(motor2_pwm, OUTPUT);

}
void silnik_sterowanie(int16_t pwm_lewo,int16_t pwm_prawo) {
    
    digitalWrite(motor1_1, HIGH);
    digitalWrite(motor1_2, LOW);
    digitalWrite(motor2_1, HIGH);
    digitalWrite(motor2_2, LOW);
    analogWrite(motor1_pwm, pwm_lewo);
    analogWrite(motor2_pwm, pwm_prawo);
}
void qtrsensor_set() {

    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]) { A0, A1, A2, A3, A4, A5, A6, A7 }, SensorCount);
   
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("KALIBRACJA");
    for (uint16_t i = 0; i < 400; i++)
    {
        qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW);

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
}

void setup()
{
   
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    silnik_set();

    qtrsensor_set();

    Serial.println("ROZPOCZECIE JAZDY");
    delay(100);
}
void loop()
{
    static uint16_t lastError = 0;
    // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
    uint16_t position = qtr.readLineBlack(sensorValues);

    // print the sensor values as numbers from 0 to 1000, where 0 means maximum
    // reflectance and 1000 means minimum reflectance, followed by the line
    // position
    
    int16_t error = position - 4500;

    int16_t motorSpeed = KP * error + KD * (error - lastError);

    lastError = error;

    int16_t m1Speed =  PWM + motorSpeed;
    int16_t m2Speed = PWM - motorSpeed;

    if (m1Speed < 0)  m1Speed = 0; 
    if (m2Speed < 0)  m2Speed = 0;
    if (m1Speed < 100)  m1Speed = 100;
    if (m2Speed < 100)  m2Speed = 100;

    silnik_sterowanie(m1Speed, m2Speed);

    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.print("POZ ");
    Serial.print(position);
    Serial.print(" PWM_P ");
    Serial.print(m1Speed);
    Serial.print(" PWM_L ");
    Serial.println(m2Speed);

    delay(10);
    
}



