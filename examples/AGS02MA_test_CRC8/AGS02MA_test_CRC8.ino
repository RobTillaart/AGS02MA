//
//    FILE: AGS02MA_test_CRC8.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: test application
//    DATE: 2021-08-14
//     URL: https://github.com/RobTillaart/AGS02MA
//

// NOTE: to have this example to work, 
// one need to make the _CRC8() and _buffer[]
// public in the AGS02MA.h file.

#include "AGS02MA.h"


AGS02MA AGS(26);


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  Wire.setClock(30000);
#if defined(__AVR__)
  Serial.print("TWBR:\t");
  Serial.println(TWBR);
#endif

  Serial.print("AGS02MA_LIB_VERSION: ");
  Serial.println(AGS02MA_LIB_VERSION);
  Serial.println();

  bool b = AGS.begin();
  Serial.print("BEGIN:\t");
  Serial.println(b);

  Serial.println("\nWarming up (120 seconds = 24 dots)");
  while (AGS.isHeated() == false)
  {
    delay(5000);
    Serial.print(".");
  }
  Serial.println();


  uint8_t version = AGS.getSensorVersion();
  Serial.print("VERS:\t");
  Serial.println(version);
  dump("getSensorVersion");
}


void loop()
{
  delay(3000);
  AGS.setPPBMode();
  dump("MODE0");
  AGS.readPPB();
  dump("PPB");

  delay(3000);
  AGS.setUGM3Mode();
  dump("MODE1");
  AGS.readUGM3();
  dump("UGM3");
}


void dump(char * str)
{
  Serial.print(str);
  for (int i = 0; i < 5; i++)
  {
    Serial.print("\t");
    Serial.print(AGS._buffer[i], HEX);
  }
  Serial.print('\t');
  Serial.print(AGS._CRC8(AGS._buffer, 4), HEX);
  Serial.println();
}

// -- END OF FILE --
