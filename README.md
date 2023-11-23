# GuL NovaFitness
**GuL NovaFitness** is a library for the Arduino-Framework to work with the novafitness particulate matter sensors like SDS011 or SDS198



## Installation

1. [Arduino Library Manager (Recommended)](https://www.arduino.cc/en/Guide/Libraries)  
2. [Download or clone this repository into your arduino libraries directory](https://help.github.com/articles/cloning-a-repository/)  


## Usage
1. Include module  
   
   ```cpp
   #include <SDS011.h> // Or one of the other moduls
   ```
   
2. Create a instance with the serial port it uses
```cpp
  //The modul uses the namespace GuL. Instead of Serial1 you can use Serial, Serial2, or own instantiated HardwareSerial or SoftwareSerial
   GuL::SDS011 sds(Serial1); 
```
   
3. In `setup()`, basic setup of the modul.  
   
   ```cpp
   void setup() {
     sds.setToPassiveReporting();
     // Or
     // sds.setToActiveReporting();

     // For power saving reasons, not neccessary
     sds.sleep();
   }
   ```
   
4. In `loop()` of the sketch, run the object's **loop()** method.  
   
   ```cpp
   void loop() {
      // If in passive reporting mode
      sds.poll();
      // A bit delay, so the sensor can respond, not really neccessary
      delay(20);

      // Read the actual data
      sds.read();

      // Fetch the specific data channel, eg.
      int pm10 = sds.getPM10();
   }
   ```   



## APIs
### Constructors

   ```cpp
   SDS011(Stream& stream);
   ```

### methods

  ```cpp
std::string getSensorName() { return _name; }
  ```
  Return the device name (Like SDS011 or SDS198)


  ```cpp
    bool poll();
  ```
  Polls the next dataframe from the sensor, the call is only neccessary if the sensor is in passive reporting mode

```cpp
        bool pollFirmwareVersion();
  ```
  Polls the firmware version from the sensor

 ```cpp
        bool pollReportingMode();
  ```
  Polls the reporting mode from the sensor


 ```cpp
        bool pollWorkingMode();
  ```
  Polls the working mode from the sensor


 ```cpp
        bool pollWorkingPeriod();
  ```
  Polls the working period from the sensor


  ```cpp
    virtual bool read();
  ```
  Reads the incomming data frame, validates and unpack

  ```cpp
    bool sleep();
  ```
  Switch the sensor to sleep mode (Saves power consumption)

  ```cpp
    bool wakeup();
  ```
  Wake the sensor up (also wakes up if there is a poll() call)

  ```cpp
    bool setToActiveReporting();
  ```
  Like tha name says, set the sensor in active reporting mode, so each second a data frame will be send automatically

  ```cpp
    bool setToPassiveReporting();
  ```
  Set the sensor in passive reporting mode, each data frame have do be polled

  ```cpp
    float getPM2_5_STD();
  ```
  Get the last received PM2.5 (µg/m³) concentration under the assumption of a standarized particle. Returns a negative number if there is no data

  ```cpp
    float getPM10_STD();
  ```
  Get the last received PM10 (µg/m³) concentration under the assumption of a standarized particle. Returns a negative number if there is no data

  ```cpp
    float getPM100();
  ```
  Get the last received PM10, (µg/m³) concentration under the assumption of a standarized particle. Returns a negative number if there is no data

  

  ```cpp
    NovaFitness::FirmwareVersion getVersion();
  ```
  Get the last received sensor firmware version. Returns a negative number if there is no data. (Only newer devices return a version)

  ```cpp
    uint16_t getID();
  ```
  Get the last received sensor id

  
  ```cpp
  NovaFitness::ReportingMode getReportingMode();
  ```
  Get the actual reporting mode

  ```cpp
  NovaFitness::WorkMode getWorkMode();
  ```
  Get the actual working mode




## What's Next

- Write tests
