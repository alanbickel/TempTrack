#include "Probe_v3.h"

//CTOR
Probe_v3::Probe_v3(DeviceAddress &a, DallasTemperature * sensor)
{
	address = &a;                                                                       //address of this probe
    sensorGroup = sensor;                                                               //access to physical probe group

	itemName = "";                                                                      //name of item being cooled
	employeeName = "";                                                                  //name of employee initializing cooling
    sessionID = "";                                                                     //unique identifier for this cooling session
	probeState = INITIALIZE;                                                            //initial state of probe
	calibrationOffset = 0;                                                              //calibration adjustment from EEPROM
    predictiveHasRun = false;

	dataLog = new CircularQueue<DataSet>(MAX_TEMP_READINGS);                            //circular queue to store data readings
	resetWriteCount();		                                                        // initialize SD write counter to 0
	resetReadCount();                                                                 // initialize read counter
}

Probe_v3::~Probe_v3()
{
    delete dataLog;                                                                     //free memory
}

//SET ITEM NAME
void Probe_v3::setItemName(String name)
{
    itemName = name;
}

//SET EMPLOYEE NAME
void Probe_v3::setEmployeeName(String name)
{
    employeeName = name;
}

//SET UNIQUE SESSION ID
void Probe_v3::setSessionID(String session)
{
    sessionID = session;
}

//NOTE INITIAL TEMP AND SYSTEM TIME (IN MILLIS)
void Probe_v3::setInitialTemperature()
{
    	setCurrentTemperature();					                                            // store current temp
    	setActiveStartTime(millis());                                                   //note time this probe made active
		initialTemperature = currentTemperature;										//store most current temperature value
}

//UPDATE THIS PROBE'S TEMPERATURE VALUE
void Probe_v3::setCurrentTemperature()
{
    sensorGroup->requestTemperatures();                                                 //pull fresh reading from probes
   
	double tempC = sensorGroup->getTempC(*address);                                     // get current temperature
    double tempF = DallasTemperature::toFahrenheit(tempC);                              //convert to Fahrenheit
	
	currentTemperature = tempF + (calibrationOffset / 1000.00);                                //adjust calibration & store in member
	
}

//NOTE THE SYSTEM TIME (in millis) WHEN ITEM WAS ASSIGNED
void Probe_v3::setActiveStartTime(unsigned long time)
{
    aciveStartTime = time;
}

//NOTE THE SYSTEM TIME (in millis) WHEN ITEM CROSSES TDZ UPPER THRESHOLD
void Probe_v3::setTrackingStartTime(unsigned long time)
{
    trackingStartTime = time;
}

//CHANGE THE VALUE OF 'state' FLAG (pass ENUM values from systemConstants.h)
void Probe_v3::setProbeState(int state)
{
    probeState = state;
}

//CHANGE THE VALUE OF 'tracking' FLAG (which phase of tracking are we in ?) (pass ENUM values from systemConstants.h)
void Probe_v3::setTrackingState(int state)
{
    trackingState = state;
}

//RESET 'timer' FOR TAKING TEMPERATURE READINGS
void Probe_v3::resetReadCount()
{
    readCounter = 0;
}

//INCREMENT VALUE OF READ 'timer'
void Probe_v3::incrementReadCount()
{
    readCounter++;
}

//RESET 'timer' FOR MEMORY CARD WRITE
void Probe_v3::resetWriteCount()
{
    writeCounter = 0;
}

//INCREMENT VALUE OF WRITE 'timer'
void Probe_v3::incrementWriteCount()
{
    writeCounter++;
}

//INITIAL UPDATE OF CALIBRATION VALUE (during initialization routine), ARG VALUES ARE PULLED FROM EEPROM
void Probe_v3::setCalibration(int value)
{
    calibrationOffset = value;
}

//UPDATE CALIBRATION VALUE, CONVERT TO INT FOR CONSISTENT EEPROM STORAGE
void Probe_v3::adjustCalibrationOffset(double value)
{

	int adjust = value * 1000;                                                          //fractional values to be stored as whole int
	calibrationOffset += adjust;                                                        //update member
}

//SET-CLEAR FLAG MARKING WHETHER PREDICTIVE ALG. HAS BEEN RUN ON ANY GIVEN DATA SET
void Probe_v3::setPredictiveFlag(bool value)
{
    predictiveHasRun = value;
}


//RETRIEVE ITEM NAME
String Probe_v3::getItemName()
{
    return itemName;
}

//RETRIEVE EMPLOYEE NAME
String Probe_v3::getEmployeeName()
{
    return employeeName;
}

//RETRIEVE SESSION ID
String Probe_v3::getSessionID()
{
    return sessionID;
}

//RETRIEVE TEMPERATURE AT POINT WHEN ITEM WAS SET TO MONITOR
double Probe_v3::getInitialTemperature()
{
    return initialTemperature;
}

//RETRIEVE MOST CURRENT TEMPERATURE VALUE
double Probe_v3::getCurrentTemperature()
{
    return currentTemperature;
}

//RETRIEVE THE SYSTEM TIME WHEN ITEM WAS FIRST ASSIGNED
unsigned long Probe_v3::getActiveStartTime()
{
    return aciveStartTime;
}

//RETRIEVE THE SYSTEM TIME WHEN ITEM CROSSED THE TDZ THRESHOLD
unsigned long Probe_v3::getTrackingStartTime()
{
    return trackingStartTime;
}

//RETRIEVE CURRENT PROBE STATE
int Probe_v3::getProbeState()
{
    return probeState;
}

//RETRIEVE CURRENT COOLING PHASE (only used when item is within the TDZ)
int Probe_v3::getTrackingState()
{
    trackingState;
}

//RETRIEVE READ COUNTER
int Probe_v3::getReadCounter()
{
    return readCounter;
}

//RETRIEVE WRITE COUNTER
int Probe_v3::getWriteCounter()
{
    return writeCounter;
}

//RETRIEVE CURRENT CALIBRATION VALUE (used to update EEPROM storage)
int Probe_v3::getCalibrationOffset()
{
    return calibrationOffset;
}

//GET VALUE OF FLAG TO FETERMINE WHETHER OR NOT PREDICTIVE ALG. HAS BEEN RUN ON A DATA SET
bool Probe_v3::getPredictiveFlag()
{
    return predictiveHasRun;
}
//GET DEVICE ADDRESS
DeviceAddress* Probe_v3::getAddress() {
	return address;
}


//STORE NEW READING IN QUEUE, AUTO-POPULATE SYSTEM TIME
void Probe_v3::logReading(double currentTemp)
{
    DataSet newReading;                                                                 //new container
	newReading.temperature = currentTemp;                                               //store temperature
	newReading.milliseconds = millis();	                                                // mark milliseconds elapsed
	dataLog->enque(newReading);                                                         //insert into queue

}

//GET INITIAL READING VALUES, PASS TO REFERENCE VARS
void Probe_v3::getReadingOne(double& temp, unsigned long& time)
{
    DataSet reading;                                                                    //new container
	dataLog->top(reading);                                                              //look at values in 1st queue slot
	temp = reading.temperature;                                                         //pass values to reference vars
	time = reading.milliseconds;
}

//GET SECONDARY READING VALUES, PASS TO REFERENCE VARS
void Probe_v3::getReadingTwo(double& temp, unsigned long& time)
{
    DataSet reading;                                                                    //new container
	dataLog->bottom(reading);                                                           //look at values in 2nd queue slot
	temp = reading.temperature;                                                         //pass values to reference vars
	time = reading.milliseconds;
}

//IS THE QUQUE FULL?
bool Probe_v3::logIsFull()
{
    return dataLog->isFull();
}

//IS THE QUEUE EMPTY?
bool Probe_v3::logIsEmpty()
{
    return dataLog->isEmpty();
}

//MAKE THE QUEUE EMPTY!
void Probe_v3::flushLog()
{
    dataLog->makeEmpty();
}

//CLEAR ALL DATA FROM PROBE (call when item complete and probe is able to be reassigned.)
void Probe_v3::resetProbe()
{
    itemName = "";                                                                      //clear item name
	employeeName = "";                                                                  //clear employee name
    sessionID = "";                                                                     //clear session id
	probeState = AVAILABLE;                                                             //make probe available
	trackingState = INITIALIZE;                                                         //reset the cooling phase flag

	flushLog();                                                                         //clear the queue
	resetWriteCount();		                                                        //reset counters
	resetReadCount();

	initialTemperature = 0.0;
	currentTemperature = 0.0;
	aciveStartTime = 0;
	trackingStartTime = 0;

	predictiveHasRun = false;
}

//COMPARE TWO PROBES, BASE EQUALITY ON ITEM NAMES ASSIGNED
bool Probe_v3::operator==(Probe_v3& right)
{
    if (itemName.equals(right.itemName)){
		return true;
	}
	else {
		return false;
	}
}
