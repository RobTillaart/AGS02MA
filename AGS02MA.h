#pragma once
//
//    FILE: AGS02MA.h
//  AUTHOR: Rob Tillaart
//    DATE: 2021-08-12
// VERSION: 0.1.0
// PURPOSE: Arduino library for AGS02MA TVOC
//     URL: https://github.com/RobTillaart/AGS02MA
//


#include "Arduino.h"
#include "Wire.h"


#define AGS02MA_LIB_VERSION         (F("0.1.0"))

#define AGS02MA_OK                  0
#define AGS02MA_ERROR               -1


#define AGS02MA_I2C_CLOCK           25000




class AGS02MA
{
public:
  explicit AGS02MA(const uint8_t deviceAddress = 26, TwoWire *wire = &Wire);

#if defined (ESP8266) || defined(ESP32) 
  bool     begin(uint8_t sda, uint8_t scl);
#endif
  bool     begin();
  bool     isConnected();
  bool     isHeated() { return (millis() - _startTime) > 120000UL; };

  uint32_t lastRead() { return _lastRead; };

  bool     setAddress(const uint8_t deviceAddress);
  uint8_t  getAddress() { return _address; };

  uint8_t  getSensorVersion();

  bool     setPPBMode();
  bool     setUGM3Mode();
  uint8_t  getMode() { return _mode; };

  uint32_t readPPB();
  uint32_t readUGM3();

  // to be called after at least 5 minutes in fresh air.
  bool     zeroCalibration();

  int      lastError();


private:
  bool     _readRegister(uint8_t reg);
  bool     _writeRegister(uint8_t reg);

  uint32_t _startTime  = 0;
  uint32_t _lastRead   = 0;
  uint8_t  _address    = 0;
  uint8_t  _mode       = 255;
  uint8_t  _buffer[5];

  int      _error = AGS02MA_OK;

  TwoWire*  _wire;
};


// -- END OF FILE --
