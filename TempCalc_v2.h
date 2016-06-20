#ifndef TEMPCALC_2_H
#define TEMPCALC_2_H

#include <Arduino.h>
#include <math.h>
#include <SystemConstants.h>
#include <Probe_v3.h>											// probe class for storing relationship of sensor to items

class TempCalc_v2 {
public:
	/*determine whether or not a given item will cool within time/temperature constraints	
	@param failState: flag set if invalid data
	@param projected: pass projected temperature back from function
	@param ambient:  ambient remperature used in predicting cooling curve (const constraint. lets not mess this figure up)
	PRECONDITION: DATA LOG CONTAINS TWO READINGS. MUST CHECK FOR 'FULL' DATA LOG BEFORE CALLING THIS FUNCTION!
	PRECONDITION: PROBE HAS FALSE PREVIOUSCALCULATION FLAG
	PRECONDITION:  PROBE IS IN ACTIVE OR TRACKING STATE PRIOR TO THIS FUNCTION CALL*/
	static bool compliantEvaluation(Probe_v3 *probe, int &failState, double &projected,const double &ambient) {
		double initTemp = 0.0, curTemp = 0.0;											//hold time/temp readings from probe's dataLog
		unsigned long initMils = 0, curMils = 0;

		probe->getReadingOne(initTemp, initMils);										//gather time/temp readings from probe's dataLog
		probe->getReadingTwo(curTemp, curMils);
		
		if (initTemp > curTemp)																//proceed if valid data reading
		{
			double thresholdTemperature = 0.0;
			int minutesToThreshold = 0;

			getThresholdValuesForProbe(probe, thresholdTemperature, minutesToThreshold);	//get constraints for probe at current stage in cooling process

			double elapsedMins = (((double)(curMils - initMils) / 1000) / 60);				// get time elapsed in minutes
			
			/*now calculate the temperature that item will be at threshold time limit*/
			//step 1 ~ find the length of time we're working with
			switch (probe->getProbeState())													//calculate time based on items current cooling stage
			{
			case ACTIVE:																	//item has been assigned, but hasn't crossed the TDZ upper threshold
			{
				//minutes to upper threshold limit
				double timeToTDZupperLimit = getProjectedTimeToTargetTemp(initTemp, curTemp, ambient, elapsedMins, TDZ_UPPER_LIMIT);
				//add the two hours to complete first cooling stage
				double timeToSeventyDegreeThreshold = timeToTDZupperLimit + SEVENTY_DEGREE_WINDOW;
				
				//now get the temperature this item will be(predicted) after this amount of time elapses
				projected = getProjectedTempAtGivenTime(initTemp, curTemp, ambient, elapsedMins, timeToSeventyDegreeThreshold);
				/*
				Serial.println("------------------------------");
				Serial.println("compliantEvaluation()");
				Serial.println(probe->getItemName());
				Serial.println("------------------------------");
				Serial.print("Initial Temperature:\t");
				Serial.println(initTemp);
				Serial.print("Second  Temperature:\t");
				Serial.println(curTemp);
				Serial.print("t1: ");
				Serial.println(initMils);
				Serial.print("t2: ");
				Serial.println(curMils);
				Serial.print("Threshold: \t");
				Serial.println(thresholdTemperature);
				Serial.print("minutes between readings:\t");
				Serial.println(elapsedMins);
				Serial.println("calculating time to TDZ upper limit");
				Serial.print("Mins to TDZ:\t");
				Serial.println(timeToTDZupperLimit);
				Serial.print("Mins To Threshold:\t");
				Serial.println(timeToSeventyDegreeThreshold);
				Serial.print("Projected Temp:\t");
				Serial.println(projected);
				Serial.print("is compliant: ");
				Serial.println((projected <= (thresholdTemperature + ACCEPTABLE_TEMPERATURE_VARIANCE)));
				*/
				break;
			}
			}
			//now evaulate quality of result
			if (!isnan(projected)) {													//if valid result

				probe->setPredictiveFlag(true);											//good data, dont predict again on this set
				failState = GOOD;														//return good failBit, no errors
				if ((projected <= (thresholdTemperature + ACCEPTABLE_TEMPERATURE_VARIANCE))) {
					//temperature is predicted compliant!
					return true;
				}
				else {
					return false;
				}
			
			}
			else {																		//invalid result
				failState = INVALID_RESULT;
				return false;
			}
		}
		else {																				//no decrease in temperature, something is wrong
			failState = INVALID_DATA_SET;													//set failbit
			return false;
		}	
	}

	//get limits for current threshold from probe
	static void getThresholdValuesForProbe(Probe_v3 *probe, double &temperature, int &timeWindow) {

		int state = probe->getProbeState();
		switch (state) {

		case ACTIVE:
		case TRACKING_PHASE_ONE:
		{
			timeWindow = SEVENTY_DEGREE_WINDOW;		// two hours  
			temperature = TDZ_TWO_HR_THRESHOLD;	// 70 degrees

			break;
		}
		case TRACKING_PHASE_TWO: 
		{
			timeWindow = FORTY_DEGREE_WINDOW;		//six hours 
			temperature = TDZ_LOWER_LIMIT;			//41 degrees
			break;
		}
		}
	}

	static double getProjectedTimeToTargetTemp(double initialTemp, double elapsedTemp, double ambient, double elapsedMinutes, double targetTemp) {
		/*
		newtons law of cooling states that for
		T(o) = temperature at time zero
		T(a) = ambient temperature
		t = time
		T(t) = temperature at given time

		T(t) = T(a) + (T(o) - T(a))e^-kt
		*/

		//first solve for constant 'k'
		double rateOfChange = getRateOfChange(initialTemp, elapsedTemp, ambient, elapsedMinutes);

		//now find elapsed time it will take for target to hit desired temperature
		//temp@time = ambient  + (init temp - ambient)e ^-(rateOfChange)*time


		//double targetTime = ((log((_T0 - _Ta) / (_Tf - _Ta))) / rateOfChange);

		return  ((log((initialTemp - ambient) / (targetTemp - ambient))) / rateOfChange);
	}

	static double getRateOfChange(double initialTemp, double elapsedTemp, double ambient, double elapsedMinutes) {
		// newton's law of cooling
		//T(t) = T(a) + (T(o) - T(a))e^-kt
		//solve for constant 'K'
		double rateOfChange = log((initialTemp - ambient) / (elapsedTemp - ambient)) / elapsedMinutes;

		return rateOfChange;
	}

	static double getProjectedTempAtGivenTime(double initialTemp, double elapsedTemp, double ambient, double elapsedMinutes, double givenTime) {

		//first solve for constant 'k'
		double rateChangeConstant = getRateOfChange(initialTemp, elapsedTemp, ambient, elapsedMinutes);

		//	T(t) = T(a) + (T(o) - T(a))e^-kt

		return ambient + (initialTemp - ambient) * (pow(exp(1.0), (-(rateChangeConstant)* givenTime)));
	}
};

#endif