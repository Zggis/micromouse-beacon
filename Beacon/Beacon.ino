//BEACON CODE
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#define LED_PIN 6
#define LED_COUNT 8
#define BRIGHTNESS 50  // Set BRIGHTNESS to about 1/5 (max = 255)

/*
40 kHz output modulated by 25 ms wide pulse. pulse duty cycle, 525 ms
DIAGRAM CORRECT AFTER COPYING INTO ARDUINO IDE
<------------525 ms----------->
25 ms
***** *****
* * * *
**** ************| |************* *****
*
Signal can be extinguished by interrupts from a proximity sensor or an ir sensor.

Code by Andrew R. Sass writted for MICROMOUSE: THE GREAT CHEESE HUNT.
AUTHOR grants permission to use
code if this section is included. Oct. 1, 2024

*/

#define BEACON_PIN 11
#define PROX_SENSOR_PIN 3
#define IR_SENSOR_PIN 2
#define SERVO_PIN 10
#define PULSE_WIDTH 1000  // 40 kHz = 25 us period; 25 us X 1000 = 25 ms

unsigned int n = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Servo myservo;  // create servo object to control a servo

bool LATCH = LOW;
bool PULSE = LOW;
bool TRIGGER_TYPE = 0;  //0 = proximity, 1 = IR

void triggerLatch_LockOn() {
  LATCH = HIGH;
}

void IR_triggerLatch_LockOn() {
  if (PULSE == LOW) {
    LATCH = HIGH;
    TRIGGER_TYPE = 1;
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 10 to the servo object
  myservo.write(90);
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  pinMode(BEACON_PIN, OUTPUT);
  pinMode(PROX_SENSOR_PIN, INPUT);                                                         //INPUT is default but this is a reminder
  attachInterrupt(digitalPinToInterrupt(PROX_SENSOR_PIN), triggerLatch_LockOn, RISING);    //PROX SENSOR POS PULSE
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), IR_triggerLatch_LockOn, FALLING);  //IR SENSOR NEG PULSE
}

void loop() {
  Serial.println("BEACON OPERATIMG");
  colorWipe(strip.Color(255, 0, 0), 100);  // Red
  strip.show();
  while (LATCH == LOW) {
    while (n <= PULSE_WIDTH && LATCH == LOW) {  //generates approx 25 us square wave
      PULSE = HIGH;
      n = n + 1;
      PORTB |= B00001000;  //pin 11 hi
      delayMicroseconds(12);
      PORTB &= B11110111;  //pin 11 lo
      delayMicroseconds(12);
    }
    PULSE = LOW;
    n = n / PULSE_WIDTH;
    delay(500);
  }
  colorWipe(strip.Color(0, 255, 0), 100);  // Green
  myservo.write(0);
  Serial.println("BEACON EXTINGUISHED");
  delay(2000);
  while (true) {
    //Flash a Blue or Red LED depedning on the trigger type (IR or Proximity)
    if (TRIGGER_TYPE == 0) {
      strip.setPixelColor(7, strip.Color(255, 0, 0));
    } else {
      strip.setPixelColor(0, strip.Color(0, 0, 255));
    }
    strip.show();
    delay(500);
    if (TRIGGER_TYPE == 0) {
      strip.setPixelColor(7, strip.Color(0, 255, 0));
    } else {
      strip.setPixelColor(0, strip.Color(0, 255, 0));
    }
    strip.show();
    delay(500);
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
    delay(wait);                                 //  Pause for a moment
  }
}