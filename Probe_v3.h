#ifndef PROBE_V3_H
#define PROBE_V3_H

#include <Arduino.h>
#include <DallasTemperature.h>                          // wrapper class for oneWire probe
#include <SystemConstants.h>                            //system-wide values
#include <RTC_DS1307.h>				                    //access to clock
#include <CircularQueue.h>
//store readings in queue

struct DataSet{
	double temperature;				                    // temperature
	unsigned long milliseconds;					        // time of reading
};

class Probe_v3
{
    public:
        Probe_v3(DeviceAddress &a, DallasTemperature * sensor);
        virtual ~Probe_v3();

        //member variable setters
        void setItemName(String name);
        void setEmployeeName(String name);
        void setSessionID(String session);
        void setInitialTemperature();
        void setCurrentTemperature();
        void setActiveStartTime(unsigned long time);
        void setTrackingStartTime(unsigned long time);
        void setProbeState(int state);
        void setTrackingState(int state);
        void resetReadCount();
        void incrementReadCount();
        void resetWriteCount();
        void incrementWriteCount();
        void setCalibration(int value);
        void adjustCalibrationOffset(double value);
        void setPredictiveFlag(bool value);

        //member variable getters
        String getItemName();
        String getEmployeeName();
        String getSessionID();
        double getInitialTemperature();
        double getCurrentTemperature();
        unsigned long getActiveStartTime();
        unsigned long getTrackingStartTime();
        int getProbeState();
        int getTrackingState();
        int getReadCounter();
        int getWriteCounter();
        int getCalibrationOffset();
        bool getPredictiveFlag();
		DeviceAddress* getAddress();

        //data log manipulation
        void logReading(double currentTemp);                        //place value in queue
        void getReadingOne(double &temp, unsigned long &time);      //get first reading
        void getReadingTwo(double &temp, unsigned long &time);      //get second reading
        bool logIsFull();                                           //return whether or not queue has been filled
        bool logIsEmpty();                                          //return whether or not queue has been emptied
        void flushLog();                                            //clear all data from data log

        //class utility functions
        void resetProbe();                                          //clear all member vars, reset to available state.
        bool operator==(Probe_v3 &right);                            //compare probes based on item name assigned.

    private:
        String itemName;                                //name of item being tracked
        String employeeName;                            // name of employee assigning item
        String sessionID;                               //unique ID for each cooling session

        double currentTemperature;                      //most recent temperature reading
        double initialTemperature;                      //temp when item assigned

        unsigned long aciveStartTime;                   //system-time based start of monitoring
        unsigned long trackingStartTime;                //system-time based start of tracking (in TDZ)

        int probeState;                                 //ENUM-based flag for controlling behavior (see systemConstants.h)
        int trackingState;                              //ENUM-based flag to determine which phase of cooling-tracking item is in
        int readCounter;                                //facilitate 'timed' control of temperature reads
        int writeCounter;                               //facilitate 'timed' control of memory card writes
        int calibrationOffset;                          //store calibration value retrieved from EEPROM

        bool predictiveHasRun;                          //remember whether a predictive calculation has been run for any given data set.

        CircularQueue<DataSet> * dataLog;               //pointer to storage/retrieval of data reading
        DeviceAddress *address;				            //pointer to physical probe address
        DallasTemperature * sensorGroup;	            // ref pointer for probe to access it's own temperature

};

#endif // PROBE_V3_H
