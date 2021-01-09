/**The MIT License (MIT)

Copyright (c) 2019 by Radim Keseg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MyThingSpeak.h"

void MyThingSpeak::write_internal(float lpg, float co, float smoke){
  if (isnan(lpg) || isnan(co) || isnan(smoke))
  {
    Serial.println("Failed to read from sensors!");
    return;
  }

  Serial.println("ThingsSpeak write");
  Serial.println("=================");
  Serial.println("-----------------");
  Serial.print("lpg: ");
  Serial.print(lpg);
  Serial.println(" ppm");
  Serial.print("co: ");
  Serial.print(co);
  Serial.println(" ppm");
  Serial.print("smoke: ");
  Serial.print(smoke);
  Serial.println(" ppm");
  Serial.println("-----------------");

  //send data to thingSpeak
  ThingSpeak.setField(field_lpg,lpg);
  ThingSpeak.setField(field_co,co);
  ThingSpeak.setField(field_smoke,smoke);
  ThingSpeak.writeFields(channelNumber, writeAPIKey);        
}

void MyThingSpeak::setup(unsigned long channelNumber, const char *writeAPIKey, unsigned int field_lpg, unsigned int field_co, unsigned int field_smoke, unsigned int update_interval){
   this->channelNumber = channelNumber;
   strncpy(this->writeAPIKey, String(writeAPIKey).c_str(), 17); 
   this->field_lpg = field_lpg;
   this->field_co = field_co;
   this->field_smoke = field_smoke;
   this->update_interval = update_interval;
}

void MyThingSpeak::write(float lpg, float co, float smoke){
  write(lpg, co, smoke, false);
}

void MyThingSpeak::write(float lpg, float co, float smoke, bool force=false){

  if (thingSpeakUpdate.expired() || force) {
    thingSpeakUpdate.set(update_interval*1000); // set new interval period
    write_internal(lpg,co,smoke);
  }

}

