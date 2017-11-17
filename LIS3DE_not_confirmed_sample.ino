#include <Arduino.h>
#include <Wire.h>
#include "Sodaq_RN2483.h"
#include "Sodaq_wdt.h"
#include "LIS3DE.h"
#include "RTCTimer.h"
#include "RTCZero.h"
#define debugSerial SerialUSB
#define loraSerial Serial1
#define MAX_RTC_EPOCH_OFFSET 25
RTCZero rtc;
RTCTimer timer;
LIS3DE accelerometer;
double x, y, z;
String xDoble, yDouble, zDouble;
volatile bool isOnTheMoveActivated;
volatile uint32_t lastOnTheMoveActivationTimestamp;
uint32_t getNow();
void initRtc();
void rtcAlarmHandler();
void resetRtcTimerEvents();
void initSleep();
void accelerometerInt1Handler();
volatile bool minuteFlag;
static bool isOnTheMoveInitialized;
void setAccelerometerTempSensorActive(bool on);
void setup() {
  //Power up the LoRaBEE
  pinMode(ENABLE_PIN_IO, OUTPUT); // ONE
  digitalWrite(ENABLE_PIN_IO, HIGH); // ONE
  delay(3000);
  Wire.begin();
  while ((!SerialUSB) && (millis() < 10000)){
    // Wait 10 seconds for the Serial Monitor
  }
  //Set baud rate
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());
 debugSerial.println("Starting...");
 isOnTheMoveInitialized = true;
  // Activate interrupt on the move
  initOnTheMove();
}
void loop(){
  /*
  x = accelerometer.getGsFromScaledValue(.readRegister(LIS3DE::OUT_X));
  y = accelerometer.getGsFromScaledValue(readRegister(LIS3DE::OUT_Y));
  z = accelerometer.getGsFromScaledValue(a.readRegister(LIS3DE::OUT_Z));
  */
  x=accelerometer.getX();
  y=accelerometer.getY();
  z=accelerometer.getZ();
  debugSerial.print("X: "); debugSerial.println(x);
  debugSerial.print("Y: "); debugSerial.println(y);
  debugSerial.print("Z: "); debugSerial.println(z);
}
/*
/**
 * Returns the board temperature.
int8_t getBoardTemperature()
{
    setAccelerometerTempSensorActive(true);




int8_t temp = params.getTemperatureSensorOffset() + accelerometer.getTemperatureDelta();




setAccelerometerTempSensorActive(false);




return temp;
}
*/
/**
* Initializes the on-the-move functionality (interrupt on acceleration).
*/
void initOnTheMove()
{
    pinMode(ACCEL_INT1, INPUT);

attachInterrupt(ACCEL_INT1, accelerometerInt1Handler, CHANGE);




// Configure EIC to use GCLK1 which uses XOSC32K, XOSC32K is already running in standby

// This has to be done after the first call to attachInterrupt()

GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_EIC) |

    GCLK_CLKCTRL_GEN_GCLK1 |

    GCLK_CLKCTRL_CLKEN;




accelerometer.enable(true, LIS3DE::NormalLowPower10Hz, LIS3DE::XYZ, LIS3DE::Scale4g, true);

sodaq_wdt_safe_delay(10);




accelerometer.enableInterrupt1(

    LIS3DE::XHigh | LIS3DE::XLow | LIS3DE::YHigh | LIS3DE::YLow | LIS3DE::ZHigh | LIS3DE::ZLow,

    18.75 * 8.0 / 100.0,

    100,

    LIS3DE::MovementRecognition);
}
/**
 * Runs every time acceleration is over the limits
 * set by the user (if enabled).
*/
void accelerometerInt1Handler()
{
    if (digitalRead(ACCEL_INT1)) {

   




    // debugPrintln("On-the-move is triggered");




    isOnTheMoveActivated = true;

    lastOnTheMoveActivationTimestamp = getNow();

}
}
/**
 * Initializes the accelerometer or puts it in power-down mode
 * for the purpose of reading its temperature delta.
*/
void setAccelerometerTempSensorActive(bool on)
{
    // if on-the-move is initialized then the accelerometer is enabled anyway

if (isOnTheMoveInitialized) {

    return;

}




if (on) {

    accelerometer.enable(false, LIS3DE::NormalLowPower100Hz, LIS3DE::XYZ, LIS3DE::Scale2g, true);

    sodaq_wdt_safe_delay(30); // should be enough for initilization and 2 measurement periods

}

else {

    accelerometer.disable();

}
}
/**
 * Returns the current datetime (seconds since unix epoch).
 */
uint32_t getNow()
{
    return rtc.getEpoch();
}
void initRtc()
{
    rtc.begin();
    // Schedule the wakeup interrupt for every minute
// Alarm is triggered 1 cycle after match
rtc.setAlarmSeconds(59);
rtc.enableAlarm(RTCZero::MATCH_SS); // alarm every minute

// Attach handler
rtc.attachInterrupt(rtcAlarmHandler);

// This sets it to 2000-01-01
rtc.setEpoch(0);
}
/**
 * Runs every minute by the rtc alarm.
*/
void rtcAlarmHandler()
{
    minuteFlag = true;
}
/**
 * Initializes the RTC Timer and schedules the default events.
 */
void initRtcTimer()
{
    timer.setNowCallback(getNow); // set how to get the current time
    timer.allowMultipleEvents();
    resetRtcTimerEvents();
}