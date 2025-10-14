#include <Wire.h>
#include <digitalWriteFast.h>

//MOUSE
/*
MOUSE CODE
Code by Andrew R. Sass writted for MICROMOUSE: THE
GREAT CHEESE HUNT&quot;. AUTHOR grants permission to
use all or parts of this code if this section is included. Oct. 1,
2024

RED OFF, GREEN OFF-BEACON NOT DETECTED.
RED FLASHING, GREEN ON -BEACON DETECTED. MAY BE
TOO QUICK
TO SEE IF MOUSE NOT USING IR LED IN PIN 11 TO
QUENCH BEACON.
RED OFF, GREEN ON. BEACON EXTIGUISHED
*/

#define BEACON_PIN 11
#define IR_PIN 2
#define BEACON_ON_LIGHT_RED_PIN 7
#define BEACON_OFF_LIGHT_GREEN_PIN 5
#define PULSE_WIDTH 1000  // 40 kHz = 25 us period; 25 us X 1000 = 25 ms
bool FALLING_EDGE = 0;
bool RISING_EDGE = 1;  //other globals below already in Mouse Sim code.

int n = 0;

bool LOCK = HIGH;
bool PULSE = LOW;

bool BEACON_OFF = LOW;  //BEACON IS ON

bool MOUSE_1_STATE = LOW;  //'LOW' BEFORE BEACON DETECTED, 'HIGH' AFTER BEACON DETECTED

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(BEACON_ON_LIGHT_RED_PIN, OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN_PIN, OUTPUT);
  pinMode(BEACON_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), ir_interrupt, RISING);
}

void ir_interrupt() {
  if (PULSE == LOW) {
    LOCK = LOW;
  }
}

void loop() {
  if (CHECK_BEACON() == HIGH && BEACON_OFF == LOW) {
    BEACON_OFF = HIGH;
  }
  if (BEACON_OFF == HIGH && MOUSE_1_STATE == LOW) {
    Serial.print("BEACON_OFF = ");
    Serial.println(BEACON_OFF);
    digitalWrite(BEACON_OFF_LIGHT_GREEN_PIN, HIGH);
    MOUSE_1_STATE = HIGH;
    //Send beacon detected info to mouse
    Wire.beginTransmission(9);
    Wire.write(1);
    Wire.endTransmission();
  }
}

bool CHECK_BEACON(void) {
  digitalWrite(BEACON_ON_LIGHT_RED_PIN, LOW);
  if (LOCK == HIGH) {
    return LOW;
  }
  if (LOCK == LOW) {
    digitalWrite(BEACON_ON_LIGHT_RED_PIN, HIGH);
    /******************25 ms pulse, 40 kHz*****************************/
    while (n <= PULSE_WIDTH && LOCK == LOW) {  //generates approx 25 us square wave for 25 ms
      //locks out irpt
      PULSE = HIGH;
      n = n + 1;
      digitalWriteFast(BEACON_PIN, HIGH);
      delayMicroseconds(12);
      digitalWriteFast(BEACON_PIN, LOW);
      delayMicroseconds(12);
    }
    n = n / PULSE_WIDTH;
    PULSE = LOW;
    /***************************end 25 ms pulse*****************/
    LOCK = HIGH;
    return HIGH;
  }
}
