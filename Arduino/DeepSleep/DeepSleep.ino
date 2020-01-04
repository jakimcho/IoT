#include "LowPower.h" //low power library

#define interrupt1_pin 2
#define interrupt_pin 3     // interrupt pin
#define led_pin 4  

void setup() {

  pinMode(interrupt_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  attachInterrupt(1, wake_routine, HIGH);
  attachInterrupt(0, wake_routine, RISING);
  digitalWrite(led_pin,LOW);
 
}

void loop() {
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  digitalWrite(led_pin,HIGH); // turn on led again
  delay(1000); // delay to allow reading
  digitalWrite(led_pin,LOW); // turn on led again
  delay(1000);
}

void wake_routine (){
 
}
