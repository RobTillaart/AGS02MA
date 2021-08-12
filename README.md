[![Arduino CI](https://github.com/RobTillaart/AGS02MA/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/AGS02MA/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/AGS02MA.svg?maxAge=3600)](https://github.com/RobTillaart/AGS02MA/releases)

# AGS02MA

Arduino library for AGS02MA TVOC sensor

Experimental, so please use with care.


## Interface

- **AGS02MA(uint8_t deviceAddress = 26, TwoWire \*wire = &Wire)** constructor, with default address and default I2C interface.
- **bool begin(uint8_t sda, uint8_t scl)** begin for ESP32 and ESP8266.
- **bool begin()** initializer for Arduino UNO a.o.
- **bool isConnected()** returns true if device address can be seen on I2C.


### Timing

- **bool isHeated()** returns true if 2 minutes have passed after startup. 
Otherwise the device is not ready.
- **uint32_t lastRead()** last time the device is read, timestamp in milliseconds since start.


### Administration

- **bool setAddress(const uint8_t deviceAddress)** Not implemented yet.
- **uint8_t getAddress()** returns set address.
- **uint8_t getSensorVersion()** reads sensor version from device.


### setMode

- **bool setPPBMode()** sets device in PartPerBillion mode. Returns true on success.
- **bool setUGM3Mode()** sets device in micro gram per cubic meter mode. Returns true on success.
- **uint8_t getMode()** returns mode set. 0 = PPB 1 = UGm3


### Reading

WARNING: Take at least 2 seconds between reads.

- ** uint32_t readPPB()** reads PPB from device. 
- ** uint32_t readUGM3()** reads current value from device. 


### Other

- **bool zeroCalibration()** to be called after at least 5 minutes in fresh air. 
See example sketch.
- **int lastError()** returns last error.


## Future

- improve documentation
- test test test ...
- add examples
- optimize code 
- buy a sensor
- elaborate error handling.


