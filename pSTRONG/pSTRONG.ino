
#include <QTRSensors.h>

#define PRZOD 0
#define TYL 1
#define SKRET_LEWO 2
#define SKRET_PRAWO 3 
#define speedMAX 255
#define speedLOW 255
#define START 4
#define MAX_SPEED_ENGINE_1 255
#define MAX_SPEED_ENGINE_2 255

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


    delay(500);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    for (uint16_t i = 0; i < 400; i++)
    {
        qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW); 

   
    Serial.begin(9600);
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(qtr.calibrationOn.minimum[i]);
        Serial.print(' ');
    }
    Serial.println();

    
    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(qtr.calibrationOn.maximum[i]);
        Serial.print(' ');
    }
    Serial.println();
    Serial.println();
    delay(5000);
    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, HIGH);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, HIGH);
    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
}
/* STEROWANIE PID'em */
void loop()
{
    static uint16_t lastError = 0;
   //dlaczego to jest statyczne?
    uint16_t position = qtr.readLineBlack(sensorValues);
    int16_t error = position - 3500;

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
    
    
    if (m1Speed < 0) { m1Speed = 0; }
    if (m2Speed < 0) { m2Speed = 0; }
    if (m1Speed > MAX_SPEED_ENGINE_1) { m1Speed = MAX_SPEED_ENGINE_1; }
    if (m2Speed > MAX_SPEED_ENGINE_2) { m2Speed = MAX_SPEED_ENGINE_2; }
    analogWrite(motor1.PWM, m1Speed);
    analogWrite(motor2.PWM, m2Speed);
    delay(100);
}

/*  STEROWANIE BEZ PID'a INSTRUKCJAMI IF{}

void loop()
{
    int16_t position = qtr.readLineBlack(sensorValues);
    if ((sensorValues[0] > 850) && (sensorValues[1] > 850) && (sensorValues[2] > 850) && (sensorValues[3] > 850) && (sensorValues[4] > 850) && (sensorValues[5] > 850) && (sensorValues[6] > 850))
    {
        analogWrite(motor1.PWM, 0);
        analogWrite(motor2.PWM, 0);
        return;
    }
    
    int16_t error = position - 3500;
    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, HIGH);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, HIGH);
    analogWrite(motor1.PWM, 255);
    analogWrite(motor2.PWM, 255);
    delay(30);
    
    if (error < -500)
    {
        
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, HIGH);
        digitalWrite(motor2.output2, LOW);
        analogWrite(motor1.PWM, 255);
        analogWrite(motor2.PWM, 255);  
    }
    if (error > 500)
    {
        
        digitalWrite(motor1.output1, HIGH);
        digitalWrite(motor1.output2, LOW);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);
        analogWrite(motor1.PWM, 255);
        analogWrite(motor2.PWM, 255);
          
    }
    delay(30);
    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
    
}
*/


