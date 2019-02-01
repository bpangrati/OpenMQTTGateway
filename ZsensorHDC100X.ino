/*  
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation 

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
    HDC100X reading Addon
  
    
    Contributors:
    - 1technophile
  
    This file is part of OpenMQTTGateway.
    
    OpenMQTTGateway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenMQTTGateway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// Pinout
// HDC100X - Uno - Mega - NODEMCU
// SCL    - A5  - 21 - D1
// SDA    - A4  - 20 - D2
//

#ifdef ZsensorHDC100X

#include <Wire.h>
#include "Adafruit_HDC1000.h"

const int HDC100X_ADDR = 0x40;

Adafruit_HDC1000 hdc = Adafruit_HDC1000();

//Time used to wait for an interval before resending temp and hum
unsigned long timeHDC100X = 0;

void setupHDC100X() {
  Wire.begin();   

  hdc.begin();
}

void MeasureHDC100X(){
    if (millis() > (timeHDC100X + TimeBetweenReadingHDC100X)) {
        timeHDC100X = millis();
        trc(F("Creating HDC100X buffer"));
        StaticJsonBuffer<JSON_MSG_BUFFER> jsonBuffer;
        JsonObject& HDC100Xdata = jsonBuffer.createObject();
        // Topic on which we will send data
        trc(F("Retrieving data"));
        // Bus Spannung, read-only, 16Bit, 0...40.96V max., LSB 1.25mV
        trc(F(" Temp: "));
        float temp = hdc.readTemperature();
        trc(temp);
        trc(F(" C, Humidity: "));
        float humi = hdc.readHumidity();
        trc(humi);

        char temp_c[7];
        char humi_c[7];
        dtostrf(temp,6,3,temp_c);
        dtostrf(humi,6,3,humi_c);
        HDC100Xdata.set("temp", (char *)temp_c);
        HDC100Xdata.set("humi", (char *)humi_c);
        pub(subjectHDC100XtoMQTT, HDC100Xdata);
    }
}

static void writeRegister(byte reg, word value) {
  Wire.beginTransmission(HDC100X_ADDR);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

static word readRegister(byte reg) {
  word res = 0x0000;
  Wire.beginTransmission(HDC100X_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission() == 0) {
    if (Wire.requestFrom(HDC100X_ADDR, 2) >= 2) {
      res  = Wire.read() * 256;
      res += Wire.read();
    }
  }
  return res;
}

#endif
