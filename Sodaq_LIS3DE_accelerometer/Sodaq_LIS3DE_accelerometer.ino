// This sample code was executed on SODAQ ONE Version 2
// Roger Rabbit took this informations from (2017-11-17)...
// https://github.com/SodaqMoja/Sodaq_LIS3DE/blob/master/Readme.md
#include <Sodaq_LIS3DE.h>
#include <Wire.h>
#define debugSerial SerialUSB

// Constructor with optional Wire instance and address parameters.
// Sodaq_LIS3DE(TwoWire& wire = Wire, uint8_t address = LIS3DE_ADDRESS)
Sodaq_LIS3DE accelerometer;

void setup()
{
  debugSerial.begin(57600);
  Wire.begin();

  // Enables the sensor with the default values or the given data rate, axes and scale.
  accelerometer.enable();
  // optinal: Resets all the registers of the sensor.
  accelerometer.reboot();
}

void loop()
{
  // Returns the temperature delta as read from the sensor (8-bit int).
  uint8_t tempDelta = accelerometer.getTemperatureDelta();
  debugSerial.println("temp: " + String(tempDelta));
  
  double x = accelerometer.getX(); // Returns a measurement of the X axis.
  double y = accelerometer.getY(); // Returns a measurement of the Y axis.
  double z = accelerometer.getZ(); // Returns a measurement of the Z axis.
  
  debugSerial.println("x: " + String(x));
  debugSerial.println("y: " + String(y));
  debugSerial.println("z: " + String(z));
  
  delay(100);
}
