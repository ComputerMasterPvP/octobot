// FILE VECCHIO - ALTAMENTE SCHIFOSO E DA RIFARE

#include <NewPing.h>
/*
  motA | motB  | Evento
  -----|-------|----------------------
  LOW  | LOW   | fermo
  LOW  | HIGH  | Movimento in un senso
  HIGH | LOW   | Movimento senso opposto
  HIGH | HIGH  | Fermo
  */

//SET PIN
static int onPin = 2;
static int triggerPin = 6;
static int echoPin = 7;
static int ledPin = 5;
static int sensorPin = A4;

//SET VARIABILI
static int velSx = 190; //190
static int velDx = 255; //255

//PIN MOTORE SINISTRO
static int motsxa= 12;
static int motsxb = 13;
static int pinVelsx = 11;

//PIN MOTORE DESTRO
static int motdxa = 8;
static int motdxb = 10;
static int pinVeldx = 9;

//CREAZIONE VARIABILI
int distanza;
int luminosita;
int triggerLed;

NewPing sonar (triggerPin, echoPin, 300);

void setup() {
  Serial.begin(9600);
  pinMode(onPin, OUTPUT);
  digitalWrite(onPin, HIGH);
  pinMode(ledPin,OUTPUT);
  pinMode(motsxa, OUTPUT);
  pinMode(motsxb, OUTPUT);
  pinMode(pinVelsx, OUTPUT);
  pinMode(motdxa, OUTPUT);
  pinMode(motdxb, OUTPUT);
  pinMode(pinVeldx, OUTPUT);
  
  luminosita = 0;
  for (int i=0; i<10; i++) {
    luminosita = luminosita + analogRead(sensorPin);
    delay(500);
    }
    int media = luminosita/10;
    triggerLed = media - 50;
}
 
 
void loop() {
  //CONTROLLO LED
  luminosita = analogRead(sensorPin);
  if (luminosita < triggerLed)
    digitalWrite(ledPin,HIGH);
    else digitalWrite(ledPin,LOW);
  
//CONTROLLO MOTORI
  distanza = sonar.ping_cm();
  if (distanza > 15 || distanza == 0) {
    runForward(velSx,velDx);
  }
    
  if(distanza < 15 && distanza != 0) {
    runBackward(velSx,velDx);
    delay(1000);
    stopAllMotors();
    turnLeft();
    delay(560);
    runForward(velSx,velDx);
  }
}

//CODICI LINK

void stopAllMotors() {
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, LOW);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, LOW);
}

void runForward(int sxVel, int dxVel) {
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, HIGH);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, HIGH);
  analogWrite(pinVelsx, sxVel);
  analogWrite(pinVeldx, dxVel);
}

void runBackward(int sxVel, int dxVel) {
  digitalWrite(motsxa, HIGH);
  digitalWrite(motsxb, LOW);
  digitalWrite(motdxa, HIGH);
  digitalWrite(motdxb, LOW);
  analogWrite(pinVelsx, sxVel);
  analogWrite(pinVeldx, dxVel);
}

void turnLeft() {
  digitalWrite(motsxa, HIGH);
  digitalWrite(motsxb, LOW);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, HIGH);
  analogWrite(pinVelsx, 150);
  analogWrite(pinVeldx, 150);
}
