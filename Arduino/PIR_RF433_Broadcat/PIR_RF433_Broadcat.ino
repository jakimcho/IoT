
#include "LowPower.h"
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

#define interrupt_pin 2
#define led_pin 3

RH_ASK driver(2000, 4, 5, 0);
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2
// RH_ASK driver(2000, 3, 4, 0); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85), 

const char *msg = "{'zone': 01}";
boolean motion_detected;

void setup()
{
  Serial.begin(9600);	  // Debugging only
  
  pinMode(led_pin, OUTPUT);
  pinMode(interrupt_pin, INPUT);
  attachInterrupt(0, detect_motion, RISING);
  
  Serial.println("** Callibrating PIR for 1 min **");
  // Use watch dog to optimize this delay
  for (int i = 0; i < 10; i++){
    digitalWrite(led_pin, HIGH);
    delay (500);
    digitalWrite(led_pin, LOW);
    delay (500);
  }
  
  Serial.println("** PIR should be callibrated **");
  
  if (!driver.init())
   Serial.println("** RF init failed **");
}

void loop()
{
  motion_detected = false;
  Serial.println("** Go to sleep **");
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  Serial.println("** Walk up **");
     
  if ( motion_detected ) { 
    Serial.println("** Movement found **\n** Sending RF Message **");
    digitalWrite(led_pin, HIGH);
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    // Use watch dog to optimize this delay
    delay(1000);
    digitalWrite(led_pin, LOW);
    delay(5000);
  }
}
void detect_motion(){
  motion_detected = true;
}
