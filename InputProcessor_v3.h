#ifndef INPUTPROCESSOR_V3_H
#define INPUTPROCESSOR_V3_H

#include <TouchScreen.h>                                                    //get input from user
#include <TFTv2.h>                                                          // output to screen
#include <SdFat.h>                                                          // SD card library
#include "DataWrapper.h"                                                    //storage from SD card lists
#include "SystemConstants.h"                                                //system-wide values
#include "GraphicConstants.h"                                               //rendering values (button locations, etc)
#include "Draw.h"                                                           //render graphic menus
#include "Message.h"                                                        // display text alerts
#include "Probe_v3.h"                                                       //probes (logical)
#include "Util.h"                                                           //utility functions
#include <RTC_DS1307.h>                                                     //real time clock
#include <RTClib.h>												            //	http://macetech.com/store/index.php?main_page=index&cPath=5

class InputProcessor_v3
{
    public:
        //CTOR
        InputProcessor_v3(TouchScreen &ts,                                    //get user input
                          TFT &t,                                           //output to screen functionality
                          Draw &sketchObj,                                  //render menus
                          DataWrapper *prepObj,                             // prep items list
					      DataWrapper *employeeObj,                         // employee list
					      DataWrapper *actionObj,                           //corrective cation list
					      SdFat *sd,                                        // SD card for write
					      RTC_DS1307 *clockObj);                           //real time clock


        //INPUT PROCESSING    
		void setProbeArray(Probe_v3 *probeList[]);							// access probe objects
        int processUserTouch();												//get user input, return value to adjust probe calibration
        void doMainMenu(Point &p);
        void doProbeMenu(Point &p);
        void doEmployeeMenu(Point &p);
        void doEmployeeConfirmationOfWarningScreen(Point p);

        int doCalibrationMenu(Point &p);void warningScreenAcknowledgement(Point &p);
        void doCriticalViolationScreen(Point &p);

        //AUTOMATED BEHAVIOR
        void initializeProbeTracking(Probe_v3 *&p, String employee);
        void stepUpItemSelectionList(DataWrapper &source);
        void stepDownItemSelectionList(DataWrapper &source);
        void checkUpDownButtons(Point &p);
        bool checkOkCancelButtons(Point &p, int &trigger);


        //TOGGLE INPUT RECOGNITION FROM USER
        void enableInput();
        void disableInput();
        bool inputIsEnabled();

        //SET/GET MENU STATE
        void setCurrentMenu(int currentMenu);								// set which menu is being displayed
        int getCurrentMenu();												// get which menu is being displayed

        //MANIPULATE TRANSACTION ID
        void setTransactionID(String id);
        String getTransactionId();

    private:

    bool enabled;												//toggle allow user input
	TouchScreen *touchScreen;									// get touch object to detect user input
	TFT *screen;												// screen reference to paint menus
	DataWrapper *items;											// information pulled form SD card.
	DataWrapper *employees;
	DataWrapper *actions;
	Draw *sketch;												// pointer to object to write menus to screen
	Probe_v3 *probeArray[NUMBER_PROBES];							            // hold probe objects to store data about cooling items
	int currentMenu;											//program state
	int currentProbeIndex;										// store selected probe number
	int currentItemSelectionIndex;								// hold selected item index
	int staticRefreshIndex;										// hold a static value of selected index if moving up or down list without shifting the items displayed
	int currentListPosition;									// determine which line to 'highlight' on display of a list

	String transactionID;						                // store unique ID of data log to facilitate multi-state writing of transaction details

	SdFat *sdCard;												//SD memory card access
	RTC_DS1307 *clock;											//real time clock access
};

#endif // INPUTPROCESSOR_V3_H
