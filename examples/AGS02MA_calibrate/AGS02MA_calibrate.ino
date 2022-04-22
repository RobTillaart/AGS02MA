//
//    FILE: AGS02MA_calibrate.ino
//  AUTHOR: Rob Tillaart, Beanow
// VERSION: 0.2.0
// PURPOSE: test application
//    DATE: 2021-08-12
//     URL: https://github.com/RobTillaart/AGS02MA
//

#include "AGS02MA.h"

// You can decrease/disable warmup when you're certain the chip already warmed up.
#define WARMUP_MINUTES 6
#define READ_INTERVAL 3000


uint32_t start, stop;
u_int8_t version;

AGS02MA AGS(26);


void setup()
{
  // ESP devices typically mis the first serial log lines after flashing.
  // Delay somewhat to include all output.
  delay(1000);

  Serial.begin(115200);
  Serial.println(__FILE__);

  Serial.print("AGS02MA_LIB_VERSION: ");
  Serial.println(AGS02MA_LIB_VERSION);
  Serial.println();

  Serial.print("WARMUP:\t\t");
  Serial.println(WARMUP_MINUTES);
  Serial.print("INTERVAL:\t");
  Serial.println(READ_INTERVAL);

  Wire.begin();

  bool b = AGS.begin();
  Serial.print("BEGIN:\t\t");
  Serial.println(b);

  Serial.print("VERSION:\t");
  version = AGS.getSensorVersion();
  Serial.println(version);

  b = AGS.setPPBMode();
  uint8_t m = AGS.getMode();
  Serial.print("MODE:\t\t");
  Serial.print(b);
  Serial.print("\t");
  Serial.println(m);

  Serial.println();
  Serial.print("Place the device outside in open air for ");
  Serial.print(WARMUP_MINUTES);
  Serial.println(" minute(s).");
  Serial.println("Make sure your device has warmed up sufficiently for the best results.");
  Serial.println("The PPB values should be stable (may include noise) not constantly decreasing.");
  Serial.println();

  start = millis();
  stop = WARMUP_MINUTES * 60000UL;
  while(millis() - start < stop)
  {
    Serial.print("[PRE ]\t");
    printPPB();
    delay(READ_INTERVAL);
  }

  Serial.println();
  Serial.println("About to perform calibration now. Your previous calibration data was:");

  auto zc = AGS.getZeroCalibration();
  Serial.print("Status:\t");
  Serial.println(zc.status);
  Serial.print("Value:\t");
  Serial.println(zc.value);

  delay(1000);

  // returns 1 if successful written
  b = AGS.zeroCalibration();
  Serial.println();
  Serial.print("CALIB:\t");
  Serial.println(b);
  Serial.println();

  Serial.println("Calibration done, your new calibration data is:");
  zc = AGS.getZeroCalibration();
  Serial.print("Status:\t");
  Serial.println(zc.status);
  Serial.print("Value:\t");
  Serial.println(zc.value);

  Serial.println();
  Serial.println("Showing what PPB values look like post calibration.");
  if (version == 118 || zc.status == 125)
  {
    Serial.println("NOTICE: v118 sensors are known to give different results after powering off!");
    Serial.println("You may need to manually set your calibration value every time power was lost.");
  }
  Serial.println();
}


void loop()
{
  Serial.print("[POST]\t");
  printPPB();
  delay(READ_INTERVAL);
}


void printPPB()
{
  uint32_t value = AGS.readPPB();
  Serial.print("PPB:\t");
  Serial.print(value);
  Serial.print("\t");
  Serial.print(AGS.lastStatus(), HEX);
  Serial.print("\t");
  Serial.print(AGS.lastError(), HEX);
  Serial.println();
}

// -- END OF FILE --
