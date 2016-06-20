#ifndef SYSTEM_CONSTANTS_H
#define SYSTEM_CONSTANTS_H


#include <Arduino.h>

/******************************HARDWARE DEFINES*******************************************/

/*TFT controller board detection*/
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3

#elif defined(__AVR_ATmega32U4__) // leonardo
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 18   // can be a digital pin, this is A0
#define XP 21   // can be a digital pin, this is A3

#else //168, 328, something else
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3

#endif

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

//SD PIN SELECT
enum SD_chip { CHIP_SELECT = 53};   //TfT_READER CS = 4,  ALT READER SLOT CS = 53
//PIEZO PIN SELECT
static int PIEZO_PIN = 15;

/**************************************END HARDWARE DEFINES******************************************************/


/**************************************CONSTANT VALUES******************************************************/

								/*probe constants*/
static const int NUMBER_PROBES = 6;							//number expected probes
static const int PROBE_ARRAY_MAX = NUMBER_PROBES - 1;		//max index for a probe array
static const int MAX_TEMP_READINGS = 2;						// max number readings for each probe's datalog

															/*container to store calibration values read from EEPROM*/
struct Calibration {										// hold array of integers to store calibration offset values permanently in EEPROM
	int calValues[NUMBER_PROBES];
};

								/*EEPROM constants*/
static const int INITIAL_VALUE = 0;
static const int EEPROM_DALLAS_ADDRESS_SIZE = (sizeof(uint8_t) * 8);		//8 byte array holds device address
static const int EEPROM_BEGIN_ADDRESS = 0;
static const int EEPROM_DALLAS_ADDRESS_BEGIN = (sizeof(int) * NUMBER_PROBES);
static const int AMBIENT_PROBE_INDEX = 0;           // always keep ambient cooler temperature probe at head
static const int FIRST_PROBE_INDEX = 1;             // index of first live probe in array
static const int AMBIENT_PROBE_READING = -1;

							/*DALLAS TEMPERATURE CONSTANTS*/
#define ONE_WIRE_BUS 18            // pin for Ardionno Mega2560 data line for probes
#define TEMPERATURE_PRECISION 9

							/*DEBUG CONST VALUES*/
static const int DEBUG_THRESHOLD_ONE = 5;
static const int DEBUG_THRESHOLD_TWO = 10;
/*TIME REPRESENTATIONS*/
static const int DEBUG_INTERVAL = 10;			//shorten loop cycle for debug
static const int ONE_MINUTE = 60;				//minute equivalent in seconds
static const int FIVE_MINUTE = 300;				//minute equivalent in seconds
static const int TEN_MINUTE = 600;				//minute equivalent in seconds

				/*TMEPERATURE DANGER ZONE THRESHOLDS (in degrees farenheit)*/
const static int TDZ_UPPER_LIMIT = 140;
const static int TDZ_TWO_HR_THRESHOLD = 70;
const static int TDZ_LOWER_LIMIT = 41;

/*COOLING TIME LENGTHS*/
static const int SEVENTY_DEGREE_WINDOW = 120;								// allowed 2 hours from 140 to 70 degrees cooling
static const int FORTY_DEGREE_WINDOW = 360;									// additional 4 hours allowed for 70 to 40 degrees

static const int SD_WRITE_INTEVAL = 10;				//time (in minutes) to write each probe
static const int READ_TIME_LIMIT = 5;				// set time between temp reads for prediction

								/*ACCEPTABLE TEMPERATURE VARIANCE*/
static const double ACCEPTABLE_TEMPERATURE_VARIANCE = 0.5;

/*timer representation of 5 minutes in millis()*/
static const unsigned long FIVE_MIN = 300000;

/*ENCRYPTION CYPHER FOR SECURE DATA WRITE*/
static const char ENCRYPT_CYPHER = 'x';

/**************************************END CONSTANT VALUES******************************************************/


/**************************************STRING LITERAL DEFINES******************************************************/
/*ERROR MESSAGE CONSTANTS*/
static const String ERR_1 = "See Console";
static const String ERR_2 = "'Error Help'";
static const String ERR_3 = "For More Details";

/*FILE ACCESS CONSTANTS*/
static const String ITEM_FILE = "itm.csv";
static const String EMPLOYEE_FILE = "emp.csv";
static const String ACTION_FILE = "act.csv";
static const String DATA_LOG = "log.dat";

/**************************************END STRING DEFINES******************************************************/



/**************************************ENUMS***************************************************/

 enum MenuStates	{						/*Touch screen states*/
						SD_INIT_ERROR,
						MAIN_MENU,
						ASSIGN_PROBE,
						ASSIGN_EMPLOYEE,
						CALIBRATE,
						DISPLAY_TT_WARNING,
						DISPLAY_TT_VIOLATION,
						DISPLAY_AMBIENT_TEMP_VIOLATION,
						DISPLAY_INVALID_DATA_WARNING,
						DISPLAY_PREDICTIVE_COMPLIANCE,
						CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING,
						CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING,
						CONFIRM_EMPLOYEE_ACKNOWLEDGE_INVALID_DATA_WARNING,
						DISPLAY_ITEM_COMPLETE
					};

 /*PROBE STATES*/
 enum ProbeStates { INITIALIZE, AVAILABLE, ACTIVE, TRACKING_PHASE_ONE, TRACKING_PHASE_TWO, AMBIENT_PROBE };

 /*FAIL BITS*/
 enum FailStates {GOOD, INVALID_DATA_SET, INVALID_RESULT};

 /*ENUM VALUE FLAG FOR EEPROM CALIBRATION ADJUSTMENT*/
 enum { CALIBRATION_FLAG = 1 };

/*SWITCH CODES DEFINE PROGRAM BEHAVIOR, ORGANIZE sd WRITES FOR EASY DATABASE IMPORTS*/
enum SwitchCodes{
	//system level errors
	PREP_LIST_NOT_LOADED =				101,
	EMPLOYEE_LIST_NOT_LOADED =			102,
	ACTION_LIST_NOT_LOADED =			103,
	SD_CARD_NOT_INITIALIZED =			110,
	TWO_HOUR_VIOLATION =				301,
	SIX_HOUR_VIOLATION =				302,
	PROBE_MISCOUNT_ERROR =				401,	// number of probes found not equal to number expected
	PROBE_NOT_LOCATED =					402,	// invalid probe address supplied to setup routine (probe missing?)
	INVALID_DATA_SET_ERROR =			501,
	/*******************************
	switch codes for logging actions
	********************************/
	LOG_NORMAL_READING =				1001,	//standard reading from item to be logged
	LOG_ITEM_COMPLETION =				1101,	//item successfully completed 
	LOG_INITIAL_READING =				1111,	//initial recording of item monitoring
	ITEM_PREDICTIVE_WARNING =			2001,	// predictive warning thrown when cooling curve is projected to be non-compliant
	THIRTY_MINUTE_STAGE_I_WARNING =		2101,	// predicted overage at first thirty minute warning
	THIRTY_MINUTE_STAGE_II_WARNING =	2201,	// predicted overage at second thirty minute warning
	AMBIENT_TEMP_OUT_OF_COMPLIANCE =	2301,	// ambient temperature overage
	TWO_HOUR_CRITICAL_VIOLATION =		3001,	// critical violation
	SIX_HOUR_CRITICAL_VIOLATION =		3101,	// critical violation
	PREDICTIVE_CALC_COMPLIANT =			4101,	// forecasted cooling curve is compliant
	/*
	append data to previous stream, 9000 family
	*/
	APPEND_CODE_THRESHOLD =				9000,
	ITEM_PREDICTIVE_WARNING_APPEND =	9201,	// append data to predictive warning
	THIRTY_MINUTE_STAGE_I_APPEND =		9211,	// append data to first thirty minute warning
	THIRTY_MINUTE_STAGE_II_APPEND =		9221,	// append data to second thirty minute warning
	AMBIENT_TEMP_OOC_APPEND_DATA =		9231,	// append information to ambient overage log
	TWO_HOUR_CRITICAL_APPENND =			9301,	// append additional information to critical violation
	SIX_HOUR_CRITICAL_APPEND =			9311,   // append additional information to critical violation
	INVALID_DATA_ERROR_APPEND =			9501,	// append additional information to invalid data warning
	PREDICTIVE_CALC_COMPLIANT_APPEND =  9601

};
# endif
