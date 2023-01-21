
[![Arduino CI](https://github.com/RobTillaart/AGS02MA/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/AGS02MA/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/AGS02MA/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/AGS02MA/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/AGS02MA/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/AGS02MA/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/AGS02MA.svg?maxAge=3600)](https://github.com/RobTillaart/AGS02MA/releases)


# AGS02MA

Arduino library for AGS02MA TVOC sensor.

This library is still experimental, so please use with care.
Note the warning about the I2C low speed, the device works at max 30 KHz.
Since 0.3.1 this library uses 25 KHz.


## I2C

### PIN layout from left to right

|  Front L->R  |  Description  |
|:------------:|:--------------|
|   pin 1      |   VDD +       |
|   pin 2      |   SDA data    |
|   pin 3      |   GND         |
|   pin 4      |   SCL clock   |


#### WARNING - LOW SPEED

The sensor uses I2C at very low speed <= 30 KHz.
For an Arduino UNO the lowest speed supported is about 30.4KHz (TWBR = 255) which works.
First runs with Arduino UNO indicate 2 failed reads in > 500 Reads, so less than 1%

Tests with ESP32 / ESP8266 at 30 KHz look good,
tests with ESP32 at lower clock speeds are to be done but expected to work.

The library sets the clock speed to 30 KHz (for non AVR) during operation
and resets it default to 100 KHz after operation.
This is done to minimize interference with the communication of other devices.
The reset clock speed can be changed with **setI2CResetSpeed(speed)** e.g. to 200 or 400 KHz.


#### 0.3.1 fix.

Version 0.3.1 sets the **I2C prescaler TWSR** register of the Arduino UNO to 4 so the lowest
speed possible is reduced to about 8 KHz.
A test run 4 hours with 6000++ reads on an UNO at 25 KHz gave 0 errors.
So the communication speed will be set to 25 KHz, also for other boards, for stability.
After communication the clock (+ prescaler) is reset again as before.


## Version 118 problems

The library can request the version with **getSensorVersion()**.
My devices all report version 117 and this version is used to develop / test this library.
There are devices reported with version 118 which behave differently.


#### ugM3 not supported

See  - https://github.com/RobTillaart/AGS02MA/issues/11

The version 118 seems only to support the **PPB** and not the **ugM3** mode.
It is unclear if this is an incident, bug or a structural change in the firmware.

If you encounter similar problems with setting the mode (any version), please let me know.
That will help indicating if this is a "structural change" or incident.


#### Calibrate problem!

See - https://github.com/RobTillaart/AGS02MA/issues/13

In this issue a problem is reported with a version 118 sensor.
The problem exposed itself after running the calibration sketch (command).
The problem has been confirmed by a 2nd version 118 sensor.
Additional calibration runs did not fix the problem.
Version 117 seem to have no problems with calibration.

**Advice**: do **NOT** calibrate a version 118.

Note: the version 0.2.0 determines the version in the calibration function so
it won't calibrate any non 117 version.


#### Please report your experiences.

If you have a AGS20MA device, version 117 or 118 or other,
please let me know your experiences
with the sensor and this (or other) library.


## Interface


#### Constructor

- **AGS02MA(uint8_t deviceAddress = 26, TwoWire \*wire = &Wire)** constructor, with default address and default I2C interface.
- **bool begin(uint8_t sda, uint8_t scl)** begin for ESP32 and ESP8266.
- **bool begin()** initializer for Arduino UNO a.o.
- **bool isConnected()** returns true if device address can be seen on I2C.
- **void reset()** reset internal variables.


#### Timing

- **bool isHeated()** returns true if 2 minutes have passed after startup (call of **begin()** ).
Otherwise the device is not optimal ready.
According to the datasheet the preheating will improve the quality of the measurements.
- **uint32_t lastRead()** last time the device is read, timestamp is in milliseconds since start.
Returns 0 if **readPPB()** or **readUGM3()** is not called yet.
This function allows to implement sort of asynchronous wait.
One must keep reads at least 1.5 seconds but preferred 3 seconds apart according to the datasheet.


#### Administration

- **bool setAddress(const uint8_t deviceAddress)** sets a new address for the sensor.
If function succeeds the address changes immediately and will be persistent over a reboot.
- **uint8_t getAddress()** returns the set address. Default the function will return 26 or 0x1A.
- **uint8_t getSensorVersion()** reads sensor version from device.
If the version cannot be read the function will return 255.
(My test sensors all return version 117, version 118 is reported)
- **uint32_t getSensorDate()** (experimental) reads bytes from the sensor that seem to indicate the production date(?). This date is encoded in an uint32_t to minimize footprint as it is a debug function.

```cpp
uint32_t dd = sensor.getSensorDate();
Serial.println(dd, HEX);   //  prints YYYYMMDD e.g. 20210203
```


#### I2C clock speed

The library sets the clock speed to 25 KHz during operation
and resets it to 100 KHz after operation.
This is done to minimize interference with the communication of other devices.
The following function can change the I2C reset speed to e.g. 200 or 400 KHz.

- **void setI2CResetSpeed(uint32_t speed)** sets the I2C speed the library need to reset the I2C speed to.
- **uint32_t getI2CResetSpeed()** returns the value set. Default is 100 KHz.


#### setMode

The default mode at startup of the sensor is PPB = parts per billion.

- **bool setPPBMode()** sets device in PartPerBillion mode. Returns true on success.
- **bool setUGM3Mode()** sets device in micro gram per cubic meter mode. Returns true on success.
- **uint8_t getMode()** returns mode set. 0 = PPB, 1 = UGm3, 255 = not set.


#### PPB versus UGM3

There is no 1 to 1 relation between the PPB and the uG/m3 readings as this relation depends
on the weight of the individual molecules.
PPB is therefore an more an absolute indicator where uG/m3 is sort of relative indicator.
If the gas is unknown, PPB is in my opinion the preferred measurement.


From an unverified source the following formula:
M = molecular weight of the gas.

**μg/m3 = ppb \* M \* 12.187 / (273.15 + °C)**

Simplified formula for 1 atm @ 25°C:

**μg/m3 = ppb \* M \* 0.04087539829 μg/m3**

Some known gasses

|  gas   |  Common name        |  ratio  ppb-μg/m3     |  molecular weight M  |
|:-------|:--------------------|:----------------------|:--------------------:|
|  SO2   |  Sulphur dioxide    |  1 ppb = 2.62 μg/m3   |    64 gr/mol         |
|  NO2   |  Nitrogen dioxide   |  1 ppb = 1.88 μg/m3   |    46 gr/mol         |
|  NO    |  Nitrogen monoxide  |  1 ppb = 1.25 μg/m3   |    30 gr/mol         |
|  O3    |  Ozone              |  1 ppb = 2.00 μg/m3   |    48 gr/mol         |
|  CO    |  Carbon Monoxide    |  1 ppb = 1.145 μg/m3  |    28 gr/mol         |
|  C6H6  |  Benzene            |  1 ppb = 3.19 μg/m3   |    78 gr/mol         |


#### Read the sensor

WARNING: The datasheet advises to take 3 seconds between reads.
Tests gave stable results at 1.5 second intervals.
Use this faster rate at your own risk.

- **uint32_t readPPB()** reads PPB (parts per billion) from device.
Typical value should be between 1 .. 999999.
Returns **lastPPB()** value if failed so one does not get sudden jumps in graphs.
Check **lastStatus()** and **lastError()** to get more info about success.
Time needed is ~35 milliseconds.
- **uint32_t readUGM3()** reads UGM3 (microgram per cubic meter) current value from device.
Typical values depend on the molecular weight of the TVOC.
Returns **lastUGM3()** if failed so one does not get sudden jumps in graphs.
- **float readPPM()** returns parts per million (PPM).
This function is a wrapper around readPPB().
Typical value should be between 0.01 .. 999.99
- **float readMGM3()** returns milligram per cubic meter.
- **float readUGF3()** returns microgram per cubic feet.


#### Error Codes

|  ERROR_CODES                |  value  |
|:----------------------------|:-------:|
|  AGS02MA_OK                 |     0   |
|  AGS02MA_ERROR              |   -10   |
|  AGS02MA_ERROR_CRC          |   -11   |
|  AGS02MA_ERROR_READ         |   -12   |
|  AGS02MA_ERROR_NOT_READY    |   -13   |


#### Cached values

- **float lastPPM()** returns last readPPM (parts per million) value (cached).
- **uint32_t lastPPB()** returns last read PPB (parts per billion) value (cached). Should be between 1..999999.
- **uint32_t lastUGM3()** returns last read UGM3 (microgram per cubic meter) value (cached).


#### Calibration

- **bool zeroCalibration()** to be called after at least 5 minutes in fresh air.
See example sketch.
- **bool manualZeroCalibration(uint16_t value = 0)** Set the zero calibration value manually.
To be called after at least 5 minutes in fresh air.
  - For v117: 0-65535 = automatic calibration.
  - For v118: 0 = automatic calibration, 1-65535 manual calibration.
- **bool getZeroCalibrationData(ZeroCalibrationData &data)** fills a data struct with the current zero calibration status and value.
Returns true on success.


#### Other

- **bool readRegister(uint8_t address, RegisterData &reg)** fills a data struct with the chip's register data at that address.
Primarily intended for troubleshooting and analysis of the sensor. Not recommended to build applications on top of this method's raw data.
Returns true when the struct is filled, false when the data could not be read.
Note: unlike other public methods, CRC errors don't return false or show up in `lastError()`, instead the CRC result is stored in `RegisterData.crcValid`.
- **int lastError()** returns last error.
- **uint8_t lastStatus()** returns status byte from last read.
Read datasheet or table below for details. A new read is needed to update this.
- **uint8_t dataReady()** returns RDY bit from last read.


#### Status bits.

|  bit  |  description                        |  notes  |
|:-----:|:------------------------------------|:--------|
|  7-4  |  internal use                       |
|  3-1  |  000 = PPB  001 = uG/M3             |
|   0   |  RDY bit  0 = ready  1 = not ready  |  1 == busy


## Future

#### Must


#### Should

- improve documentation
  - add indicative table for PPB health zone (source)
- put the I2C speed code in 2 inline functions
  - less repeating conditional code places
  - setLowSpeed() + setNormalSpeed()
- check the mode bits of the status byte with internal \_mode.
  - maximize robustness of state
- test with hardware
  - different gasses ?

#### Could

- elaborate error handling.
- create an async interface for **readPPB()** if possible
  - delay(30) blocks performance ==> async version of **readRegister()**
- move code to .cpp?


#### Wont

