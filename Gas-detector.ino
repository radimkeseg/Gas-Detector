/**The MIT License (MIT)

Copyright (c) 2021 by Radim Keseg

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
// HOSTNAME for OTA update
#define HOSTNAME "GAS-DETECTOR"

#include "MQ2.h"
MQ2 mq2(A0, D5);

#include "Buzzer.h"
Buzzer buzzer(D6);

#include "Gauge3LED.h"
Gauge3LED gauge3LED(D1,D2,D3);

#include "MyThingSpeak.h"
MyThingSpeak myThingSpeak;

#include "MyWifi.h"
MyWifi myWifi;

#include "TimeClient.h"
TimeClient timeClient(1);

Interval measurementUpdate;
long update_interval = 10*1000; //internal update default 5 s

int lpg, co, smoke;
bool blink;

/* webserver handlers */
static String handle_data(){
  String res;
  res = "{\"lpg\":\"";
  res += lpg = mq2.readLPG();
  res += "\",\"co\":\"";
  res += co = mq2.readCO();
  res += "\",\"smoke\":\"";
  res += smoke = mq2.readSmoke();
  res += "\"}";
  return res;
}
 
void setup() {
  Serial.begin(9600);

  gauge3LED.setValue(Gauge3LED::GREEN + Gauge3LED::YELLOW);

  mq2.begin();  
  gauge3LED.begin();
  buzzer.begin();

  myWifi.setup(HOSTNAME,60); //1 min to configure the WIFI 
  myWifi.setDataHandler( handle_data );

  gauge3LED.setValue(Gauge3LED::GREEN + Gauge3LED::RED);

  myThingSpeak.begin(myWifi.getWifiClient());
  myThingSpeak.setup(myWifi.getCustomSettings().settings.TS_CHANNEL, myWifi.getCustomSettings().settings.TS_API_WRITE, 
                     myWifi.getCustomSettings().settings.TS_FIELD_LPG, myWifi.getCustomSettings().settings.TS_FIELD_CO, myWifi.getCustomSettings().settings.TS_FIELD_SMOKE, 
                     myWifi.getCustomSettings().settings.TS_UPDATE_INTERVAL);

  gauge3LED.setValue(Gauge3LED::GREEN);
}

void loop() {
  myWifi.handleClient();
  

  if(myWifi.getCustomSettings().settings.THINGSPEAK){ //write to thingspeak
    if(myWifi.isConnected()){
      myThingSpeak.write(lpg,co,smoke,false); //realy writes only if thingspeak timer expires

      if(mq2.getAlarm()){ 
        gauge3LED.setValue(Gauge3LED::YELLOW+Gauge3LED::RED);
        if(measurementUpdate.expired()){ //process if the update interval has expired only
           measurementUpdate.set(update_interval); // set new interval period 
           myThingSpeak.write(lpg,co,smoke,true);
        }
      }
    }
  }
  

/** your code here */

  /*read the values from the sensor, it returns
  *an array which contains 3 values.
  * 1 = LPG in ppm
  * 2 = CO in ppm
  * 3 = SMOKE in ppm
  */
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
  
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke(); 

/**
 * LPG
 * 
 * CO
 * dangerous >100ppm (parts per million) 
 * most gas alarms trigger by 70-80 ppm
 * life threat by 150-220 ppm 
 * 
 * smoke
 * 
 */  
  if(lpg>70 || co>70 || smoke>1000){
    gauge3LED.setValue(blink ? Gauge3LED::RED : Gauge3LED::NONE );
    mq2.setAlarm(blink);
    buzzer.play(blink ? NOTE_C3 : NOTE_F3 );
  }else if(lpg>35 || co>35 || smoke>500){
    gauge3LED.setValue(Gauge3LED::YELLOW);
    mq2.setAlarm(true);
    buzzer.silence();
  }else{
    gauge3LED.setValue(Gauge3LED::GREEN);
    mq2.setAlarm(false);
    buzzer.silence();
  }
/** end of your code */

  blink = !blink;
  delay(200);
}

