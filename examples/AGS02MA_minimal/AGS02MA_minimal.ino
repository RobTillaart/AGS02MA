//
//    FILE: AGS02MA_minimal.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: test application
//    DATE: 2021-08-14
//     URL: https://github.com/RobTillaart/AGS02MA
//

#include "AGS02MA.h"

AGS02MA AGS(26);

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(30000);
}


void loop()
{
  delay(3000);
  Wire.requestFrom(26, 5);
  for ( int i = 0; i < 5; i++)
  {
    uint8_t x = Wire.read();
    Serial.print(x, HEX);
    Serial.print('\t');
  }
  Serial.println();
}

// -- END OF FILE --
