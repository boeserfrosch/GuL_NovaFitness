#include <Arduino.h>
#include <SDS011.h>

GuL::SDS011 sds(Serial2);

#define RX2 1
#define TX2 38

std::string outputFormat = "PM1 (STD) \t= % 6d µg/µ3 \n"
                           "PM10 (STD) \t= % 6d µg/µ3 \n"
                           "\n";

void setup()
{
  Serial.begin(9600);

  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);

  sds.setToPassiveReporting();
}

void loop()
{
  pms.poll();
  delay(20);
  pms.read();

  Serial.printf(outputFormat.c_str(),
                sds.getPM1(),
                sds.getPM10());
  delay(1000);
}