#include <Wire.h>

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
#define BEACON_ON_LIGHT_RED_PIN 7
#define BEACON_OFF_LIGHT_GREEN_PIN 5
#define PULSE_WIDTH 1000  // 40 kHz = 25 us period; 25 us X 1000 = 25 ms
bool FALLING_EDGE = 0;
bool RISING_EDGE = 1;  //other globals below already in Mouse Sim code.

int n = 0;

int m = 0;

bool ZONE01 = LOW;
short int highZONE01 = 10;

bool LOCK = HIGH;
bool PULSE = LOW;

bool BEACON_OFF = LOW;  //BEACON IS ON

bool MOUSE_1_STATE = LOW;  //'LOW' BEFORE BEACON DETECTED, 'HIGH' AFTER BEACON DETECTED

bool BEACON_SEEN = LOW;

bool CHECK_BEACON(void) {
  digitalWrite(BEACON_ON_LIGHT_RED_PIN, LOW);

  if (LOCK == HIGH) {
    return LOW;
  }

  //delay(2000);

  if (LOCK == LOW) {  //
    digitalWrite(BEACON_ON_LIGHT_RED_PIN, HIGH);
    BEACON_SEEN = HIGH;

    /******************25 ms pulse, 40 kHz*****************************/
    while (n <= PULSE_WIDTH && LOCK == LOW) {  //generates approx 25 us square wave for 25 ms
      //locks out irpt
      PULSE = HIGH;
      n = n + 1;
      PORTB |= B00001000;  //pin 11 hi
      delayMicroseconds(12);
      PORTB &= B11110111;  //pin 11 lo
      delayMicroseconds(12);
    }  //END WHILE

    n = n / PULSE_WIDTH;
    PULSE = LOW;
    /***************************end 25 ms pulse*****************/

    LOCK = HIGH;

    return HIGH;
  }  //END IF LOCK
}

void init_GPIO() {
  pinMode(BEACON_ON_LIGHT_RED_PIN, OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN_PIN, OUTPUT);
  pinMode(BEACON_PIN, OUTPUT);
  DDRB = B00001000;
}

ISR(PCINT0_vect) {
  if ((PINB & B00000100) && RISING_EDGE) {
    if (PULSE == LOW) {
      LOCK = LOW;
    }
    RISING_EDGE = 0;
  }

  if (!(PINB & B00000100) && !RISING_EDGE) {
    RISING_EDGE = 1;
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Wire.begin();
  init_GPIO();
  PCICR |= B00000001;   //GRP 0
  PCMSK0 |= B00000100;  //PIN D10
}

// the loop function runs over and over again forever
void loop() {

  if (CHECK_BEACON() == HIGH && BEACON_OFF == LOW) {
    BEACON_OFF = HIGH;
  }
  if (BEACON_OFF == HIGH && MOUSE_1_STATE == LOW) {
    //Serial.print("BEACON_OFF = ");
    //Serial.println(BEACON_OFF);
    digitalWrite(BEACON_OFF_LIGHT_GREEN_PIN, HIGH);
    MOUSE_1_STATE = HIGH;
  }

/*

  if (MOUSE_1_STATE == HIGH) {
    //Beacon has been extinnguished
    Wire.beginTransmission(9);  // transmit to device #9
    Wire.write(2);              // sends x
    Wire.endTransmission();     // stop transmitting
  } else if (BEACON_SEEN == HIGH) {
    //Beacons has been detected
    Wire.beginTransmission(1);  // transmit to device #9
    Wire.write(x);              // sends x
    Wire.endTransmission();     // stop transmitting
  } else {
    Wire.beginTransmission(9);  // transmit to device #9
    Wire.write(0);              // sends x
    Wire.endTransmission();     // stop transmitting
  }
  */

}  //END LOOP