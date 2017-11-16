#include "Arduino.h"
#include <Sodaq_RN2483.h>
#include <RTCZero.h>

#define loraSerial Serial1
#define BEE_VCC 20

// ABP - Activation by Personalization
const uint8_t devAddr[4] = { 0x00, 0x00, 0x00, 0x00 };
const uint8_t appSKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t nwkSKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// OTAA
uint8_t DevEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

RTCZero zerortc;

void setupLoRaABP() {
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, false))
  {
    // all good!
  }
  else
  {
    // failed!
  }
}
void initSleep()
{
  // tell the LoRa-Module to sleep!
  // loraSerial.begin(LoRaBee.getDefaultBaudRate());
  loraSerial.println("sys sleep 30000");
  delay(100);

  // Set the sleep mode
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

// do nothing just wake up
void alarmMatch(void) {}

void resetAlarm(void) {
  // hours, minutes, seconds
  zerortc.setTime(0, 0, 0);
  // day, month, year
  zerortc.setDate(1, 1, 1);

  // alarmHours, alarmMinutes, alarmSeconds
  zerortc.setAlarmTime(0, 0, 31);
  zerortc.enableAlarm(zerortc.MATCH_HHMMSS);
}

void setupLoRa() {
  // ABP
  setupLoRaABP();

  LoRaBee.setSpreadingFactor(9);
}

void sendPacket(String packet) {
  switch (LoRaBee.send(1, (uint8_t*)packet.c_str(), packet.length()))
  {
    case NoError:
      // Successful transmission
      break;
    case NoResponse:
      setupLoRa();
      break;
    case Timeout:
      delay(20000);
      break;
    case PayloadSizeError:
      // The size of the payload is greater than allowed. Transmission failed!
      break;
    case InternalError:
      // Oh No! This shouldn't happen. Something is really wrong! Try restarting the device! The network connection will reset.
      setupLoRa();
      break;
    case Busy:
      // The device is busy. Sleeping for 10 extra seconds.
      delay(10000);
      break;
    case NetworkFatalError:
      // There is a non-recoverable error with the network connection. You should re-connect The network connection will reset.
      setupLoRa();
      break;
    case NotConnected:
      // The device is not connected to the network. Please connect to the network before attempting to send data. The network connection will reset.
      setupLoRa();
      break;
    case NoAcknowledgment:
      // There was no acknowledgment sent back!
      // When you this message you are probaly out of range of the network.
      break;
    default:
      break;
  }
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
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  //connect to the LoRa Network
  setupLoRa();

  zerortc.begin(); // Set up clocks and such
  resetAlarm();  // Set alarm
  zerortc.attachInterrupt(alarmMatch); // Set up a handler for the alarm
  delay(100);
  lightNoLED();
}

// ##################################################################################
void loop() {
  // deep sleep 30 seconds -- then do something and go to sleep again...
  lightGreenLED();
  initSleep();
  lightNoLED();
  // call deep sleep and wait for interrupt of real time clock! (set to 30 seconds)
  __WFI();
  delay(200);
  // .. I woke up ...

  // do something here ...
  String packet = "SODAQ";
  lightRedLED();
  sendPacket(packet);
  lightBlueLED();
  delay(50);
  resetAlarm();  // Reset alarm before returning to sleep
}

// ##################################################################################

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


