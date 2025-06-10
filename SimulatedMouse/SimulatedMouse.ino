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

#define OPTION 1  //PREPROCESSOR SWITCH

#define BEACON_PIN 11
#define IR_SENSOR_PIN 2
#define BEACON_ON_LIGHT_RED_PIN 7
#define BEACON_OFF_LIGHT_GREEN_PIN 5
#define PULSE_WIDTH 1000  // 40 kHz = 25 us period; 25 us X 1000 = 25 ms

int n = 0;

int m = 0;

bool ZONE01 = LOW;
short int highZONE01 = 10;

bool LOCK = HIGH;
bool PULSE = LOW;

bool BEACON_OFF = LOW;  //BEACON IS ON

bool MOUSE_1_STATE = LOW;  //'LOW' BEFORE BEACON DETECTED, 'HIGH' AFTER BEACON DETECTED

short int MOUSE_2_STATE = 0;  // BEFORE BEACON DETECTED, DURING APPROACH TO BEACON,
// AFTER BEACON EXTINGUISHED
short int MOUSE_2_STATE_PREV = 0;

#if OPTION == 2

short int MOUSE_GOAL_2(void) {
  bool temp = CHECK_BEACON();
  //Serial.print("MOUSE 2 STATE PREV = ");
  //Serial.println(MOUSE_2_STATE_PREV);

  if (temp == HIGH) {
    m = m + 1;
    m = m % 10000;
    Serial.print("m =");
    Serial.println(m);
    if (m == highZONE01) {
      ZONE01 = HIGH;
    }
    Serial.println("GOAL FOUND");
    digitalWrite(BEACON_ON_LIGHT_RED, HIGH);
    MOUSE_2_STATE = 1;
    MOUSE_2_STATE_PREV = MOUSE_2_STATE;
    delay(1000);
    return MOUSE_2_STATE;
  }
  if (temp == LOW && MOUSE_2_STATE_PREV == 0) {
    Serial.println("MOUSE SEARCHING");
    digitalWrite(BEACON_ON_LIGHT_RED, LOW);
    digitalWrite(BEACON_OFF_LIGHT_GREEN, LOW);
    MOUSE_2_STATE = 0;
    delay(1000);
    return MOUSE_2_STATE;
  }

  if (temp == LOW && MOUSE_2_STATE_PREV == 1) {
    if (ZONE01 = HIGH) {
      Serial.println("GOAL TRIGGERED");
      digitalWrite(BEACON_ON_LIGHT_RED, LOW);
      digitalWrite(BEACON_OFF_LIGHT_GREEN, HIGH);
      MOUSE_2_STATE = 2;
      return MOUSE_2_STATE;
    }
    delay(2000);
    return MOUSE_2_STATE;
  }
  // delay(1000); // necessary delay for synchronization with beacon
  // return MOUSE_2_STATE;
}

#endif

#if OPTION == 1

bool MOUSE_GOAL_1(void) {
  //Serial.println(CHECK_BEACON());
  //Serial.print("MOUSE_STATE = ");
  //Serial.println(MOUSE_STATE);

  if (CHECK_BEACON() == HIGH && BEACON_OFF == LOW) {
    BEACON_OFF = HIGH;
  }
  if (BEACON_OFF == HIGH && MOUSE_1_STATE == LOW) {
    //Serial.print("BEACON_OFF = ");
    //Serial.println(BEACON_OFF);
    digitalWrite(BEACON_OFF_LIGHT_GREEN_PIN, HIGH);
    MOUSE_1_STATE = HIGH;
  }
  return MOUSE_1_STATE;
}

#endif

bool CHECK_BEACON(void) {
  digitalWrite(BEACON_ON_LIGHT_RED_PIN, LOW);

  if (LOCK == HIGH) {
    return LOW;
  }

  //delay(2000);

  if (LOCK == LOW) {  //
    digitalWrite(BEACON_ON_LIGHT_RED_PIN, HIGH);

#if OPTION == 1
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

#endif
    LOCK = HIGH;

    return HIGH;
  }  //END IF LOCK
}

void IR_triggerLatch() {  //irpt every other event
  if (PULSE == LOW) {
    LOCK = LOW;
    ///LATCH = LATCH_PREVIOUS ^ HIGH;
    ///LATCH_PREVIOUS = LATCH;
  }
}

void init_GPIO() {
  pinMode(BEACON_ON_LIGHT_RED_PIN, OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN_PIN, OUTPUT);
  pinMode(BEACON_PIN, OUTPUT);
  DDRB = B00001000;
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  init_GPIO();
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), IR_triggerLatch, RISING);  //IR SENSOR NEG PULSE
}

// the loop function runs over and over again forever
void loop() {

#if OPTION == 1
  Serial.println(MOUSE_GOAL_1());  //'LOW' BEACON NOT DETECTED, 'HIGH' BEACON EXTINGUISHED
#endif

#if OPTION == 2
  //MOUSE_GOAL_2();
  Serial.println(MOUSE_GOAL_2());  // BEACON NOT DETECTED, APPROACHING DETECTED BEACON

//BEACON EXTINGUISHED
#endif

}  //END LOOP