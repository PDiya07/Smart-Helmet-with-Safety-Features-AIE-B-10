#include <RH_ASK.h>
#include <SPI.h>

int IRsensor = 2;
int alcoholSensor = A0;
int vibrationSensor = 3;
int ultrasonicTrig = 4;
int ultrasonicEcho = 5;
int buzzer = 7;
int RF_TX = 11;

RH_ASK rf_driver;

void setup() {
  pinMode(IRsensor, INPUT);
  pinMode(alcoholSensor, INPUT);
  pinMode(vibrationSensor, INPUT);
  pinMode(ultrasonicTrig, OUTPUT);
  pinMode(ultrasonicEcho, INPUT);
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
  rf_driver.init();
  Serial.println("Transmitter ready");
}

void loop() {
  int helmetWorn = !digitalRead(IRsensor);
  int alcoholLevel = analogRead(alcoholSensor);
  int accidentDetected = digitalRead(vibrationSensor);

  // 🚘 Obstacle Detection
  digitalWrite(ultrasonicTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrig, LOW);
  int duration = pulseIn(ultrasonicEcho, HIGH);
  int distance = duration * 0.034 / 2;

  // 🚨 Crash or Obstacle Detection - Buzzer ON
  if (accidentDetected == HIGH) {
    digitalWrite(buzzer, HIGH);
    Serial.println("⚠️ CRASH DETECTED! Buzzer ON");
    delay(2000);
  } 
  else if (distance > 2 && distance < 30) {
    digitalWrite(buzzer, HIGH);
    Serial.println("⚠️ WARNING: Obstacle Too Close!");
    delay(1000);
  } 
  else {
    digitalWrite(buzzer, LOW);
  }

  // 📡 Send Data to Bike Unit
  char message[30];
  sprintf(message, "H:%d A:%d D:%d C:%d", helmetWorn, alcoholLevel, distance, accidentDetected);
  rf_driver.send((uint8_t *)message, strlen(message));
  rf_driver.waitPacketSent(); 

  Serial.println(message);
  delay(1000);
}



