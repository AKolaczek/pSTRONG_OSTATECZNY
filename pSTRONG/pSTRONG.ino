// https://pololu.github.io/qtr-sensors-arduino/class_q_t_r_sensors.html opis biblioteki
// https://pololu.github.io/qtr-sensors-arduino/md_usage.html zastosowanie
#include <IRremote.hpp>
#include <QTRSensors.h>



#define PRZOD 0
#define TYL 1
#define SKRET_LEWO 2
#define SKRET_PRAWO 3 
#define START 4
#define PREDKOSC_MAX 255
#define IRremote 7

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
double KP = 0.1; //stala proporcjonalna
double KD = 5; //stala rozniczkowalna

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
    /*
    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, HIGH);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, HIGH);
    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
    */
    //PROBA IR
    //irremote();
}
/* STEROWANIE PID'em */
void loop()
{
    static uint16_t lastError = 0; //zmienna statyczna pamieta swoja wartosc pomiedzy wywolaniami funkcji
  
    uint16_t position = qtr.readLineBlack(sensorValues);
    int16_t error = position - 3500;

    for (uint8_t i = 0; i < SensorCount; i++)
    {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.print("Pozycja= ");
    Serial.print(position);
    Serial.print("  lastError= ");
    Serial.print(lastError);


    int16_t motorSpeed = KP * error + KD * (error - lastError); //obliczanie pwm
    lastError = error;
    int16_t m1Speed = PREDKOSC_MAX + motorSpeed; 
    int16_t m2Speed = PREDKOSC_MAX - motorSpeed;

    if (m1Speed < 0) { m1Speed = 0; } //zabezpieczenie przed ujemnymi wartosciami pwm, trzeba by bylo inne funkcje tworzyc
    if (m2Speed < 0) { m2Speed = 0; }
    if (m1Speed > PREDKOSC_MAX) { m1Speed = PREDKOSC_MAX; } //zabezpieczenie przed ponad pwm>255
    if (m2Speed > PREDKOSC_MAX) { m2Speed = PREDKOSC_MAX; }
    //analogWrite(motor1.PWM, m1Speed);
    //analogWrite(motor2.PWM, m2Speed);
    Serial.print("m1Speed= ");
    Serial.print(m1Speed);
    Serial.print("  m2Speed= ");
    Serial.println(m2Speed);
        
    ruchsilnika(m1Speed, m2Speed); //wysyla pwm na silniki

    delay(100);
}

void ruchsilnika(int16_t predkosc1, int16_t predkosc2) {

    digitalWrite(motor1.output1, LOW);
    digitalWrite(motor1.output2, HIGH);
    digitalWrite(motor2.output1, LOW);
    digitalWrite(motor2.output2, HIGH);
    analogWrite(motor1.PWM, predkosc1);
    analogWrite(motor2.PWM, predkosc2);

    Serial.print("Predkosc silnika1: ");
    Serial.print(predkosc1);
    Serial.print("  Predkosc silnika2: ");
    Serial.println(predkosc2);

}
void irremote() {
    IRrecv irrecv(IRremote); //definiowanie odbiornika IR
    decode_results results; //definiowanie wyniku
    //IRsend irsend; //tworzenie obiektu nadajnika
    irrecv.enableIRIn(); //aktywowanie odbiornika IR
    irrecv.blink13(true);
    while (1) {
        if (irrecv.decode(&results)) {
            Serial.println(results.value, HEX);
            irrecv.resume();
        }
    }

        
}
void testsilnikow() {
    int czas = 10; //to jest 10 * 500(to co w delayu) czyli aktualnie 5sekund
    int i = 0;
    int PWM = PREDKOSC_MAX - 100;
    int j = PWM / czas;

    Serial.println("Ruchy do przodu, stale PWM");
    delay(200);
    Serial.print("SILNIK PRAWY PWM= ");
    Serial.println(PWM);

    for (i = 0; i <= czas; i++) {
        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        analogWrite(motor1.PWM, PWM);

        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    silnikistop();

    Serial.print("SILNIK LEWY PWM= ");
    Serial.println (PWM);

    for (i = 0; i <= czas; i++) {
        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor2.output1, LOW);
        digitalWrite(motor2.output2, HIGH);
        analogWrite(motor2.PWM, PWM);

        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    silnikistop();

    Serial.println("Ruchy do przodu, zmienne PWM");
    Serial.print("SILINIK PRAWY");
    
    for (i = 0; i <= czas; i++) {
        
        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        analogWrite(motor1.PWM, j);
        
        Serial.print("PWM= ");
        Serial.println(j);

        j += j;
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    silnikistop();
    for (i = 0; i <= czas; i++) {

        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        analogWrite(motor1.PWM, PWM-j);

        Serial.print("PWM= ");
        Serial.println(PWM-j);

        j += j;
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    Serial.print("SILINIK LEWY");

    for (i = 0; i <= czas; i++) {

        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        analogWrite(motor1.PWM, j);

        Serial.print("PWM= ");
        Serial.println(j);

        j += j;
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    silnikistop();
    for (i = 0; i <= czas; i++) {

        digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(motor1.output1, LOW);
        digitalWrite(motor1.output2, HIGH);
        analogWrite(motor1.PWM, PWM - j);

        Serial.print("PWM= ");
        Serial.println(PWM - j);

        j += j;
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    Serial.println("Test silnikow zakonczony");
}
void kalibracja() {
    
}
void silnikistop() {
    analogWrite(motor1.PWM, 0);
    analogWrite(motor2.PWM, 0);
}
void rozruch() {
    analogWrite(motor1.PWM, PREDKOSC_MAX);
    analogWrite(motor2.PWM, PREDKOSC_MAX);
    delay(50);
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


