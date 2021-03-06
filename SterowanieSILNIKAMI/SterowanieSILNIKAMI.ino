/*
 Name:		sterowanie2silniki.ino
 Created:	2/19/2022 3:15:09 PM
 Author:	pSTRONG v.1
*/
#include <stdio.h>


#define przod 0
#define tyl 1
#define speedMAX 255

struct motor
{
    int output1;
    int output2;
    int PWM;
};
struct motor motor1;
struct motor motor2;

void setup() {
    // pinMode(5, OUTPUT);
    // pinMode(6, OUTPUT);
    // pinMode(7, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
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
    for (int i = 0; i <= 5; i++)
    {
        miganie();
    }
    
   // delay(5000000);
}

void loop() {
    // put your main code here, to run repeatedly:
    start(przod, speedMAX);
    delay(50);
    start(przod, speedMAX - 150);
    delay(2000);
    stop();
    delay(1000);
    start(tyl, speedMAX);
    delay(50);
    start(tyl, speedMAX - 150);
    delay(2000);
    stop();
    delay(1000);

    miganie();
    /*
    start(tyl, speedMAX);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5000);
    stop();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    */
}

void start(bool direction, int speed) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (direction) {
        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);

        analogWrite(motor1.PWM, speed);
        analogWrite(motor2.PWM, speed);
    }
    else
    {
        digitalWrite(motor1.output1, HIGH);
        digitalWrite(motor1.output2, LOW);
        digitalWrite(motor2.output1, HIGH);
        digitalWrite(motor2.output2, LOW);

        analogWrite(motor1.PWM, speed);
        analogWrite(motor2.PWM, speed);
    }
    digitalWrite(LED_BUILTIN, LOW);
}

void stop() {
    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, LOW);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, LOW);
    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
}
void miganie() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
}