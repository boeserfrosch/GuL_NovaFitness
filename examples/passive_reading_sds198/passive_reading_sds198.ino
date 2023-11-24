#include <Arduino.h>
#include <SDS198.h>

GuL::SDS198 sds(Serial2);

#define RX2 1
#define TX2 38

std::string outputFormat = "PM100 (STD) \t= % 6.2f µg/µ3 \n"
                           "Firmware Version \t= %d.%d.%d \n"
                           "\n";

void setup()
{
  Serial.begin(9600);

  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);

  sds.setToPassiveReporting();
  sds.pollFirmwareVersion();
}

void loop()
{
  sds.poll();
  delay(20);
  sds.read();

  Serial.printf(outputFormat.c_str(),
                sds.getPM100(),
                sds.getVersion().year,
                sds.getVersion().month,
                sds.getVersion().day);
  delay(1000);
}