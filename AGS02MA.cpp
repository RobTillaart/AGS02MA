//
//    FILE: AGS02MA.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2021-08-12
// VERSION: 0.1.0
// PURPOSE: Arduino library for AGS02MA TVOC
//     URL: https://github.com/RobTillaart/AGS02MA


#include "AGS02MA.h"


// REGISTERS
#define AGS02MA_DATA                  0x00
#define AGS02MA_CALIBRATION           0x01
#define AGS02MA_VERSION               0x11
#define AGS02MA_SLAVE_ADDRESS         0x21




AGS02MA::AGS02MA(const uint8_t deviceAddress, TwoWire *wire)
{
  _address    = deviceAddress;
  _wire       = wire;
  _error      = AGS02MA_OK;
}


#if defined (ESP8266) || defined(ESP32)
bool AGS02MA::begin(uint8_t dataPin, uint8_t clockPin)
{
  _startTime = millis();  // PREHEAT
  _wire      = &Wire;
  if ((dataPin < 255) && (clockPin < 255))
  {
    _wire->begin(dataPin, clockPin);
  } else {
    _wire->begin();
  }
  return isConnected();
}
#endif


bool AGS02MA::begin()
{
  _startTime = millis();  // PREHEAT TIMING
  _wire->begin();
  return isConnected();
}


bool AGS02MA::isConnected()
{
  _wire->setClock(AGS02MA_I2C_CLOCK);
  _wire->beginTransmission(_address);
  bool rv =  ( _wire->endTransmission() == 0);
  _wire->setClock(400000);
  return rv;
}


bool AGS02MA::setAddress(const uint8_t deviceAddress)
{
  _address = deviceAddress;
  return false;
  // TODO... 
  // low prio
  if (_writeRegister(AGS02MA_SLAVE_ADDRESS))
  {
    _address = deviceAddress;
  }
  return isConnected();
}


uint8_t AGS02MA::getSensorVersion()
{
  _readRegister(AGS02MA_VERSION);
  uint8_t version = _buffer[3];
  return version;
}


bool AGS02MA::setPPBMode()
{
  _buffer[0] = 0x00;
  _buffer[1] = 0xFF;
  _buffer[2] = 0x00;
  _buffer[3] = 0xFF;
  _buffer[4] = 0x30;
  if (_writeRegister(AGS02MA_DATA))
  {
    _mode = 0;
    return true;
  }
  return false;
}


bool AGS02MA::setUGM3Mode()
{
  _buffer[0] = 0x02;
  _buffer[1] = 0xFD;
  _buffer[2] = 0x02;
  _buffer[3] = 0xFD;
  _buffer[4] = 0x00;
  if (_writeRegister(AGS02MA_DATA))
  {
    _mode = 1;
    return true;
  }
  return false;
}


uint32_t AGS02MA::readPPB()
{
  uint32_t val = 0xFFFFFFFF;
  _lastRead = millis();
  if (_readRegister(AGS02MA_DATA))
  {
    _status = _buffer[0];
    val =  _buffer[1] * 65536UL;
    val += _buffer[2] * 256;
    val += _buffer[3];
    // TODO CRC
  }
  return val;
}


uint32_t AGS02MA::readUGM3()
{
  uint32_t val = 0xFFFFFFFF;
  _lastRead = millis();
  if (_readRegister(AGS02MA_DATA))
  {
    _status = _buffer[0];
    val = _buffer[1] * 65536UL;
    val += _buffer[2] * 256;
    val += _buffer[3];
    // TODO STATUS
    // TODO CRC
  }
  return val;
}


bool AGS02MA::zeroCalibration()
{
  _buffer[0] = 0x00;
  _buffer[1] = 0x0C;
  _buffer[2] = 0xFF;
  _buffer[3] = 0xF3;
  _buffer[4] = 0xFC;
  return _writeRegister(AGS02MA_CALIBRATION);
}


int AGS02MA::lastError()
{
  int e = _error;
  _error = AGS02MA_OK;  // reset error after read
  return e;
}



/////////////////////////////////////////////////////////
//
// PRIVATE
//
bool AGS02MA::_readRegister(uint8_t reg)
{
  _wire->setClock(AGS02MA_I2C_CLOCK);
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _error = _wire->endTransmission();

  if (_wire->requestFrom(_address, (uint8_t)5) != 5)
  {
    _error = AGS02MA_ERROR;
    _wire->setClock(400000);
    return false;
  }
  for (int i = 0; i < 5; i++)
  {
    _buffer[i] = _wire->read();
  }
  _wire->setClock(400000);
  return true;
}


bool AGS02MA::_writeRegister(uint8_t reg)
{
  _wire->setClock(AGS02MA_I2C_CLOCK);
  _wire->beginTransmission(_address);
  _wire->write(reg);
  for (int i = 0; i < 5; i++)
  {
    _wire->write(_buffer[i]);
  }
  _error = _wire->endTransmission();
  _wire->setClock(400000);
  return (_error == 0);
}


// -- END OF FILE --
