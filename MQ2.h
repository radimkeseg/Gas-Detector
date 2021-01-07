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
#pragma once

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MQ2 {
public: 
	MQ2(int pinAnalog, int pinDigital);
	float* read(bool print);
	float readLPG();
	float readCO();
	float readSmoke();
  bool setAlarm(bool state);
  bool getAlarm();
	void begin();
private:
	int _pinAnalog;
  int _pinDigital;
	int RL_VALUE = 5;     //define the load resistance on the board, in kilo ohms
	int RO_CLEAN_AIR_FACTOR = 9.83;  
	int CALIBARAION_SAMPLE_TIMES = 5; 
	int CALIBRATION_SAMPLE_INTERVAL = 50;
	int READ_SAMPLE_INTERVAL = 50;
	int READ_SAMPLE_TIMES = 5;

	float LPGCurve[3]  =  {2.3,0.21,-0.47}; 
	float COCurve[3]  =  {2.3,0.72,-0.34};   
	float SmokeCurve[3] = {2.3,0.53,-0.44};                                                       
	float Ro = 10;             
 
   int GAS_LPG = 0;
	int GAS_CO = 1;
	int GAS_SMOKE = 2;

	float lpg = 0;
	float co = 0;
	float smoke = 0;
  bool alarm=false;
	
	float MQRead();
	float MQGetGasPercentage(float rs_ro_ratio, int gas_id);
	int MQGetPercentage(float rs_ro_ratio, float *pcurve);
	float MQCalibration();
	float MQResistanceCalculation(int raw_adc);

	int lastReadTime = 0;
};

