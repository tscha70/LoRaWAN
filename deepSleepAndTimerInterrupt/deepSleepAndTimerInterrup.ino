#include "Arduino.h"
#include <Sodaq_RN2483.h>
#include <RTCZero.h>

RTCZero zerortc;

void initSleep()
{ 
  // tell the LoRa-Module to sleep!
  Serial1.begin(LoRaBee.getDefaultBaudRate());
  Serial1.println("sys sleep 30000");
  delay(200);
  
  // Set the sleep mode
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

// do nothing just wake up
void alarmMatch(void){} 

void resetAlarm(void) {
  // hours, minutes, seconds
  zerortc.setTime(0, 0, 0);
  // day, month, year
  zerortc.setDate(1, 1, 1);

  // alarmHours, alarmMinutes, alarmSeconds
  zerortc.setAlarmTime(0, 0, 36);
  zerortc.enableAlarm(zerortc.MATCH_HHMMSS);
}

void setup() {
  // inspired by...
  // http://forum.sodaq.com/t/sodaq-one-power-consumption/346/7
  pinMode(ENABLE_PIN_IO, OUTPUT);
  digitalWrite(ENABLE_PIN_IO, HIGH);

  pinMode(GPS_ENABLE, OUTPUT);
  digitalWrite(GPS_ENABLE, LOW);

  // RGB LED:
  initalizeLEDs();
  lightBlueLED();
  
  delay(5000);
  
  zerortc.begin(); // Set up clocks and such
  resetAlarm();  // Set alarm
  zerortc.attachInterrupt(alarmMatch); // Set up a handler for the alarm
  delay(100);
  lightNoLED();
}

void loop() {
  lightRedLED();
  initSleep();
  lightNoLED();
  // call deep sleep and wait for interrupt which is never occuring
  __WFI();

  delay(200);
  resetAlarm();  // Reset alarm before returning to sleep  
}

void lightRedLED() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void lightGreenLED() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void lightBlueLED() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

void lightNoLED() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void initalizeLEDs()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}


