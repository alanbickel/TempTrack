#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>
#include <SdFat.h>                // SD card library
#include <Message.h>              //display messages
#include <RTClib.h>					//date time library
#include <SystemConstants.h>
#include <Probe_v3.h>

class Util {

public:

	/********************************QUEUE STRUCTS AND OVERLOADS********************************************/
	struct QueNode {
		int switchCode;		// error or triggering code, determines display options for warning/violation display mode
		Probe_v3 *probe;		// gather detailed information directly from probe
		double temperature;	// store temp at time of violation
		String timeStamp;	// time that event occured.  

		bool operator==(QueNode &right) {
			if (probe->getItemName().equals(right.probe->getItemName())) {
				return true;
			}
			else {
				return false;
			}
		}
	};

	/*simple ASCII addition encrypt/decrypt method. */
	static String Crypt(String input) {

		String output = input;

		for (int i = 0; i < input.length(); i++)
			output[i] = input[i] + '0';		

		return output;

	}
	static String DeCrypt(String input) {

		String output = input;

		for (int i = 0; i < input.length(); i++)
			output[i] = input[i] - '0';		

		return output;

	}

	//write string of text to SD card
	static void encryptAndWriteToSD(SdFat sd, String text, String fileName) {

		File dataFile = sd.open(fileName.c_str(), FILE_WRITE);

		if (dataFile) {
			/*TODO:  ENCRYPT WRITE DATA! CURRENTLY NO CRYPT FOR DEBUG!*/
			String crypted = text; //Crypt(text);

			dataFile.print(crypted);
			dataFile.print('\n');

			dataFile.close();
		}
		else {
		}
	}

	//wrapper to derive data from warning node and format string to write to SD card.  
	static void logWarningToSD(SdFat sd, QueNode &node, String transactionID, DateTime *date) {
		
		String logText = String(node.switchCode);							// switch code determines action
		logText += ",";
		logText += node.probe->getSessionID();				// item identifier to group loggings in database
		logText += ",";
		logText += transactionID;										//unique trancaction ID based on system time
		logText += ",";
		logText += geteCurrentDateString(*date);					//date
		logText += ",";
		logText += node.probe->getItemName();							// name
		logText += ",";
		logText += node.temperature;									//temperature
		logText += ",";
		logText += node.timeStamp;									//timestamp

		encryptAndWriteToSD(sd, logText, DATA_LOG);					//encrypt and write to SD card
	}

	// pass pointer to message for  warning to user, PRECONDITION: program state has been verified, node has been dequeued form queue
static void buildAmbientWarning(QueNode &node, Message &target) {

		String errCode(node.switchCode);
		String line1("Error Code: ");
		line1 += errCode;

		String line2 = "Ambient Temp: ";
		String temp(node.temperature);
		line2 += temp;
		String line4("Address Immediately.");
		String line5("Touch Screen");
		String line6("To Acknowledge");

		target.setLine(line1, 1);
		target.setLine(line2, 2);
		target.setLine(node.timeStamp, 3);
		target.setLine(line4, 4);
		target.setLine(line5, 5);
		target.setLine(line6, 6);

		
	}
// pass pointer to message for  warning to user, PRECONDITION: program state has been verified, node has been dequeued form queue
static void buildPredictiveWarning(QueNode &node, Message &target) {
	String errCode(node.switchCode);
	String line1("Error Code: ");
	line1 += errCode;

	String line3(node.probe->getItemName());
	String line4 = "Forecast Non Compliance";
	String line5("Touch Screen");
	String line6("To Acknowledge");

	target.setLine(line1, 1);
	target.setLine(node.timeStamp, 2);
	target.setLine(line3, 3);
	target.setLine(line4, 4);
	target.setLine(line5, 5);
	target.setLine(line6, 6);
}

static void buildInvalidDataWarning(QueNode &node, Message &target) {
	String errCode(node.switchCode);
	String line1("Error Code: ");
	line1 += errCode;
	String line3(node.probe->getItemName());
	String line4 = "Invalid Data Set ";
	String line5("SEE CONSOLE ERROR HELP");
	String line6("Touch to acknowledge");

	target.setLine(line1, 1);
	target.setLine(node.timeStamp, 2);
	target.setLine(line3, 3);
	target.setLine(line4, 4);
	target.setLine(line5, 5);
	target.setLine(line6, 6);
}

//show user which cooling phase is forecasted to be compliant
static void buildCompliantConfirmation(QueNode &node, Message &target) {
	int coolingPhase = node.probe->getProbeState();
	String thresholdTemp;
	if ((coolingPhase == ACTIVE) || (coolingPhase == TRACKING_PHASE_ONE)) {
		thresholdTemp = String(TDZ_TWO_HR_THRESHOLD);
	}
	else if (coolingPhase == TRACKING_PHASE_TWO) {
		thresholdTemp = String(TDZ_LOWER_LIMIT);
	}
	thresholdTemp += " Degrees F";

	String line1(node.probe->getItemName());
	String line3 = "Forecasted cooling to";
	String line5 = "Is Compliant";
	String line6 = "Touch To Dismiss";

	target.setLine(line1, 1);
	target.setLine(node.timeStamp, 2);
	target.setLine(line3, 3);
	target.setLine(thresholdTemp, 4);
	target.setLine(line5, 5);
	target.setLine(line6, 6);

}

static String getTransactionID(DateTime now) {

	String uniqueId(now.hour());
	uniqueId += String(now.minute());
	uniqueId += String(now.second());
	uniqueId += String(now.month());
	uniqueId += String(now.day());
	return uniqueId;
}

static String geteCurrentDateString(DateTime now) {
	String date = String(now.year());
	date += String(now.month());
	date += String(now.day());

	return date;
}


/*give multiple attempts to initialize SD card,
switch default pin locations*/
static bool custom_SD_load(int chipSelect, SdFat &sd) {

	for (int i = 0; i < 5; i++) {
		if (!(sd.begin(chipSelect, SPI_QUARTER_SPEED))) {

			if (chipSelect == CHIP_SELECT)
				chipSelect = 4;
			else
				chipSelect = CHIP_SELECT;
		}
		else
			return true;
	}
	return false;
}

/*convert int representation of minutes to human readable HH:MM format*/
static String minutesToTimeString(int minutes) {
	int hours = 0;
	String timeStamp = "";

	while (minutes >= 60) {
		minutes -= 60;
		hours++;
	}

	timeStamp += hours;
	timeStamp += ":";

	if (minutes < 10) {
		timeStamp += "0";
	}
	timeStamp += minutes;

	return timeStamp;
}

//build string time stamp from DateTime object
static String timeStamp(DateTime time) {

	const int len = 32;
	static char buf[len];

	String timeStamp(time.toString(buf, len));

	return timeStamp;
}

/*CONTROL PIEZO MODULE*/
static void firePiezoWarning() {
	tone(PIEZO_PIN, 314);      // Almost any value can be used except 0 and 255
	//frequency 'just off' of a proper note produces a much more irritating tone :)
	//good when you want someon to not be able to ignore it. 
	//314 is 'just shy' of F (349 Hz)


}

static void firePiezoCompliantAlert() {
	tone(PIEZO_PIN, 1109);      // Almost any value can be used except 0 and 255
}

static void killPiezo() {
	noTone(PIEZO_PIN);
}


static void logInitialTrackingInformation(SdFat sd, Probe_v3 *probe, DateTime *date, String employeeName) {
	String logText = String(LOG_INITIAL_READING);				//switchcode
	logText += ",";
	logText += probe->getSessionID();							// session code for grouping in db
	logText += ",";
	logText += probe->getItemName();							// name
	logText += ",";
	logText += probe->getCurrentTemperature();							//temperature
	logText += ",";
	logText += timeStamp(*date);								//timestamp
	logText += ",";
	logText += employeeName;									//employee

	Util::encryptAndWriteToSD(sd, logText, DATA_LOG);
}

static void printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		// zero pad the address if necessary
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}
};


#endif // UTIL_H
