#include "InputProcessor_v3.h"

//CTOR
InputProcessor_v3::InputProcessor_v3(TouchScreen& ts, TFT& t, Draw& sketchObj, DataWrapper* prepObj, DataWrapper* employeeObj, DataWrapper* actionObj, SdFat* sd, RTC_DS1307* clockObj)
{
    touchScreen = &ts;															        //get user touch coords
    screen = &t;																        // graphic display / visuals
    items = prepObj;															        // prep items
    employees = employeeObj;													        // employees
    actions = actionObj;														        // corrective actions
    sketch = &sketchObj;														        //draw menus / warnings
    enabled = false;															        // toggle allow user input
    currentMenu = 0;															        // default main menu
    clock = clockObj;															        //pointer to Real Time Clock
    sdCard = sd;																        // assign pointer to SD volume

    /*default index values to display array names in item selection lists*/
    currentItemSelectionIndex = 0;
    currentListPosition = 1;
    currentProbeIndex = -1;														        // flag to find unused probe. reset when done with it, please...
    transactionID = "";															        //concurrent logging through different program states
}
//DTOR


void InputProcessor_v3::setProbeArray(Probe_v3 *probeList[]) {
	for (int i = 0; i < NUMBER_PROBES; i++)
		probeArray[i] = probeList[i];               // store probe pointers
}

// PROCCESS  TOUCH COORDS BASED ON PROGRAM STATE
int InputProcessor_v3::processUserTouch()
{
    int calibrationAdjustRequired = 0;

    if (inputIsEnabled()) 														        // dont bother if input disabled
    {

        Point p = touchScreen->getPoint();										        //get coords of user touch
        p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
        p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

        if (p.z > __PRESURE)                                                            // only execute when user has touched screen
        {
            switch (getCurrentMenu())                                                   // action controlled by 'do()' methods for each program state
            {
            case MAIN_MENU:
            {
                doMainMenu(p);                                          // action based on button location for main menu.
                break;
            }
            case ASSIGN_PROBE:
            {
                doProbeMenu(p);
                break;
            }
            case ASSIGN_EMPLOYEE:
            {
                doEmployeeMenu(p);
                break;
            }
            case CALIBRATE:
            {
                for (int i = 0; i < NUMBER_PROBES; i++)
                {
                    Probe_v3 *probe = probeArray[i];                                    // reference each probe
					probe->setCurrentTemperature();
                    probe->getCurrentTemperature();		                                //update each temperature
					
                }
                // return value acts as flag for  'loop()' to determine if updates to EEPROM are necessary
                calibrationAdjustRequired = doCalibrationMenu(p);        // show calibration menu
                break;
            }
            case DISPLAY_TT_WARNING:                                                    // predicted time of cooling item is higher than compliant threshold
            {
                Util::killPiezo();											            //silence alarm
                screen->fillScreen();											        // clear screen
                sketch->employeeAcknowledgeWarning(*employees, "Forecast");			    // draw menu to select employee
                setCurrentMenu(CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING);	            // update program state
                break;
            }
            case DISPLAY_AMBIENT_TEMP_VIOLATION: 								        // ambient temperature in violation of temperature ceiling
            {
                Util::killPiezo();											            //silence alarm
                screen->fillScreen();											        // clear screen
                sketch->employeeAcknowledgeWarning(*employees, "Ambient");			    // draw menu to select employee
                setCurrentMenu(CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING);	        // update program state
                break;
            }
            case DISPLAY_INVALID_DATA_WARNING:  		                                //bad data set, data readings have been reset
            {
                Util::killPiezo();											            //silence alarm
                screen->fillScreen();                                                   // clear screen
                sketch->employeeAcknowledgeWarning(*employees, "Bad Data");			    // draw menu to select employee
                setCurrentMenu(CONFIRM_EMPLOYEE_ACKNOWLEDGE_INVALID_DATA_WARNING);	    // update program state
                break;
            }
            case DISPLAY_PREDICTIVE_COMPLIANCE:                                         //ALL GOOD, LET USER KNOW
            {
                Util::killPiezo();											            //silence alarm
                screen->fillScreen();                                                   //clear screen
                setCurrentMenu(MAIN_MENU);                                              //update program state
                break;
            }
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING :					        //cooler temp
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING:						        // predicted violation
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_INVALID_DATA_WARNING:				        // invalid data set
            {
                doEmployeeConfirmationOfWarningScreen(p);                               //process input for employee selection of warning confirmation
                break;
            }
            }
        }
    }
    return calibrationAdjustRequired;
}

//EVALUATE USER BUTTON SELECTION FROM MAIN MENU DISPLAY
void InputProcessor_v3::doMainMenu(Point& p)
{
    //reset all data members
    currentProbeIndex = -1;						                                        // reset probe to 'none selected'
    currentItemSelectionIndex = 0;				                                        // reset to default value
    staticRefreshIndex = 0;						                                        // reset to default value
    currentListPosition = 1;					                                        // reset to default value

    if ((p.x > LEFT) && (p.x < (LEFT + WIDTH)))                                         // if we're within left/right boundaries of menu buttons
    {
        int buttonUpperLimit = BUTTON_1_UPPER;                                          // initial vertical constraints of first button
        int buttonLowerLimit = buttonUpperLimit + HEIGHT;

        /*find button pressed by stepping through vertical position of each button*/
        for (int i = 1; i < 5; i++)
        {
            if ((p.y > buttonUpperLimit) && (p.y < buttonLowerLimit))                   // if we're within vertical boundaries of current button
            {
                sketch->mainMenu(i);                                                    // repaint menu to show selection
                delay(250);                                                             //slight delay so user can see

                switch (i)
                {
                case 1:             /*-----BEGIN TRACKING ITEM, MOVE TO PROBE ASSIGNMENT---------*/
                {
                    setCurrentMenu(ASSIGN_PROBE);                                       // set menu marker to 'assign probe'
                    screen->fillScreen();                                               // clear screen

                    /*PAINT ITEM SELECTION MENU, USING DATA FROM DataWrapper MEMBER*/
                    sketch->itemTrackingMenu(*items);                                   // use info from prep items list
                    break;
                }
                case 2:             /*--------------------Quick Temp Check--------------------------*/
                {
                    break;
                }
                case 3:             /*----------------------Item Progress---------------------------*/
                {
                    break;
                }
                case 4:             /*------------------Calibration of Probes-----------------------*/
                {

                    setCurrentMenu(CALIBRATE);                                          // set menu marker
                    screen->fillScreen();                                               // clear screen
                    currentProbeIndex = 0;                                              //set ambient temp probe as default to display

                    for (int i = 0; i < NUMBER_PROBES; i++)
                    {
                        Probe_v3 *probe = probeArray[i];                                // reference each probe

						probe->setCurrentTemperature();
                    }
                    double temperature = probeArray[0]->getCurrentTemperature();                // get ambient temp as default
                    sketch->calibrationScreen(AMBIENT_PROBE_READING, temperature);      // display cooler temp to user
                    break;
                }
                }
            }
            buttonUpperLimit += (HEIGHT + PADDING_SPACE);                               // increment button boundaries to look for next button
            buttonLowerLimit += (HEIGHT + PADDING_SPACE);
        }
    }
}

//EVALUATE USER BUTTON SELECTION FROM PROBE MENU DISPLAY
void InputProcessor_v3::doProbeMenu(Point& p)
{
    /*CHECK VERTICAL CONSTRAINTS OF PROBE BUTTONS*/
    if ((p.y > PROBE_BUTTON_HEIGHT_OFFSET) && (p.y < (PROBE_BUTTON_HEIGHT_OFFSET + PROBE_BUTTON_HEIGHT)))
    {
        // index of usable probes begins at 1. index 0 always occupied by ambient temperature probe
        for (int i = 1; i < 6; i++)                                                     /*FIND WHICH PROBE BUTTON SELECTED*/
        {
            int button_left = (SMALL_PAD * i) + (PROBE_BUTTON_WIDTH * (i - 1)),         // left boundary of button
                button_right = button_left + PROBE_BUTTON_WIDTH;                        // right boundary of button

            if ((p.x > button_left) && (p.x < button_right))                            // if user touch within boundaries
            {
                if ((probeArray[i]->getProbeState() == AVAILABLE) && (i <= PROBE_ARRAY_MAX))             // if probe isn't taken and is a valid index
                {
                    Probe_v3 *probe = probeArray[i];                                    // set probe reference

                    /*we're not assigning any data to probe object yet, we just want to remember which
                    probe has been selected, so after we confirm item name and employee, we can easily assign the data*/
                    currentProbeIndex = i;                                              //set current selection to data member for later access
                    sketch->probeButtons(i);                                            /*repaint buttons so user sees their selection*/
                }
                else                                                                    // probe is not available, warn user
                {
                    disableInput();
                    String pNum = String(i);
                    String lineTwo = "Probe " + pNum + " Occupied.";
                    Message warning(5, *screen);
                    warning.setLine("Error:", 1);
                    warning.setLine(lineTwo, 2);
                    warning.setLine("Please Choose", 3);
                    warning.setLine("Another Probe", 4);
                    warning.showMessage();
                    delay(500);
                    screen->fillScreen();                                               // clear screen
                    sketch->itemTrackingMenu(*items);                                   // repaint item selection list
                    enableInput();
                }
            }
        }
    }
    /*--------------------END PROBE NUMBER DETECTION-----------------------------*/
    /*-------------NOW CHECK UP AND DOWN BUTTONS FOR ACTIVITY--------------------*/
    checkUpDownButtons(p);
    /*-----------------NOW CHECK OK & CANCEL BUTTON PRESS------------------------*/
    int okCancelTrigger = 0;                                                            // check for button activity, default none
    bool confirm = checkOkCancelButtons(p, okCancelTrigger);
    if (okCancelTrigger == 1)                                                           // if button was pressed
    {
        if (confirm)
        {
            if (currentProbeIndex >= 0)                                                 // if we've got a legit probe array index
            {
                disableInput();                                                         //lock out user while we're busy
                String *names = items->getNameArray();                                  //get name array from DataWrapper
                String itemName = names[currentItemSelectionIndex];                     //get current selected name
                probeArray[currentProbeIndex]->setItemName(itemName);                   //set name to probe

                currentItemSelectionIndex = 0;                                          /*refresh data members*/
                staticRefreshIndex = 0;
                currentListPosition = 1;

                setCurrentMenu(ASSIGN_EMPLOYEE);                                        // set menu marker to 'assign employee'
                screen->fillScreen();                                                   // clear screen
                sketch->employeeSelectMenu(*employees);                                 //paint menu to select employee
                enableInput();                                                          //re-enable input

            }
            else                                                                        // no probe selected
            {
                disableInput();                                                         //lock out user while we're busy
                Message warning("Please Select Probe", *screen);
                delay(750);
                screen->fillScreen();                                                   // clear screen
                sketch->itemTrackingMenu(*items);                                       // redraw menu
                enableInput();                                                          //re-enable input
            }
        }
        else                                                                            // user has canceled
        {
            probeArray[currentProbeIndex]->setItemName("");                             // free probe
            disableInput();
            setCurrentMenu(MAIN_MENU);                                                  // set menu marker to 'main menu'

            screen->fillScreen();                                                       // clear screen
            sketch->mainMenu(0);                                                        // draw main menu
            enableInput();                                                              //re-enable input
        }
    }
}
//EVALUATE USER BUTTON SELECTION FROM EMPLOYEE SELECTION MENU DISPLAY
void InputProcessor_v3::doEmployeeMenu(Point& p)
{
    checkUpDownButtons(p);                                                              /*up/down button logic*

    /*look for confirm/cancel activity*/
    int employeeTrigger = 0;                                                            // check for button activity, default none
    bool confirm = checkOkCancelButtons(p, employeeTrigger);

    if (employeeTrigger == 1)                                                           // if button activity detected
    {
        if (confirm)                                                                    // employee selection confirmed
        {
            Probe_v3 *probe = probeArray[currentProbeIndex];                            // get probe
            String *names = employees->getNameArray();                                  //get list of employee names
            String employeeName = names[currentItemSelectionIndex];                     //get selected employee name

            initializeProbeTracking(probe, employeeName);                               //solidify probe relationship, begin tracking item

            Message msg(4, *screen);                                                    //build message to show user
            String probeLine = "Probe ";
            probeLine = probeLine + (currentProbeIndex);
            msg.setLine(probeLine, 1);
            msg.setLine(probe->getItemName(), 2);
            msg.setLine("Tracking", 3);
            msg.setLine("Initialized", 4);
            msg.blankBox();
            msg.showLine(1);
            msg.showLine(2);
            msg.showLine(3);
            msg.showLine(4);
            delay(750);

            disableInput();                                                             //disable input while we're busy
            setCurrentMenu(MAIN_MENU);                                                  // set menu marker to 'main menu'
            screen->fillScreen();                                                       // clear screen
            sketch->mainMenu(0);                                                        // draw main menu
            enableInput();                                                              //re-enable user input
        }
        else                                                                            // user has canceled
        {
            probeArray[currentProbeIndex]->setItemName("");                             // free probe/item association
            currentProbeIndex = -1;                                                     // clear selected probe index
        }
    }
}
//LOG EMPLOYEE ACKNOWLEDGEMENT OF WARNING TO SD CARD, DISPLAY WARNING TO USER, RETUEN TO MAIN MENU
void InputProcessor_v3::doEmployeeConfirmationOfWarningScreen(Point p)
{
    checkUpDownButtons(p);                                                              /*up/down button logic*/


    if ((p.x > (DONE_BUTTON_LEFT)) && (p.x < DONE_BUTTON_LEFT + DONE_BUTTON_WIDTH))     //check for 'done' button press
    {
        if ((p.y > DONE_BUTTON_HEIGHT_OFFSET) && (p.y < (DONE_BUTTON_HEIGHT_OFFSET + DONE_BUTTON_HEIGHT)))
        {
            disableInput();						                                        // prevent user action fwhile calculating
            sketch->bigDoneButton(true);		                                        //redraw to show action to user
            delay(250);

            String *names = employees->getNameArray();					                /*get employee name*/
            String employeeName = names[currentItemSelectionIndex];

            currentItemSelectionIndex = 0;								                /*reset data members*/
            staticRefreshIndex = 0;
            currentListPosition = 1;
            String line1 = "";											                //variable text, depends on prog. state
            String logLine = "";
            Message message(4, *screen);                                                //message construct

            switch (getCurrentMenu())  									                // set vars based on current state
            {

            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING:
            {
                message.setLine("Ambient Warning", 1);					                // warning type for display
                logLine = String(AMBIENT_TEMP_OOC_APPEND_DATA);			                // set switch code
                break;
            }
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING:
            {
                message.setLine("Fore-casted Violation", 1);					        // warning type for display
                logLine = String(ITEM_PREDICTIVE_WARNING_APPEND);			            //set switch code
                break;
            }
            }
            // log data to SD, use transaction ID to sync multi-part record
            // use append code as switchCode (first value in log string)
            logLine += ",";
            logLine += transactionID;
            logLine += ",";
            logLine += employeeName;

            Util::encryptAndWriteToSD(*sdCard, logLine, DATA_LOG);		                //write to SD

            message.setLine("Acknowledged By", 2);						                //finish building message
            message.setLine(employeeName, 3);
            message.setLine("Returning To Menu", 4);
            message.showMessage();									                    //display confirmation to user
            delay(750);

            setCurrentMenu(MAIN_MENU); 									                //reset program state
            screen->fillScreen();										                // clear screen
            enableInput();
            sketch->mainMenu(0);										                // redraw main menu
        }
    }
}
//PROCESS USER INPUT FROM CALIBRATION MENU SCREEN
int InputProcessor_v3::doCalibrationMenu(Point& p)
{
    // set to "1" if update required
    int calibrationUpdateRequired = 0;                                                  // return val to flag 'loop()' function to update EEPROM

    // step 1. determine if any probe selection buttons pressed, set currentSelectedIndex to remember which probe we're working on

    // check for 'ambient cooler' button selected
    if ((p.x > AMBIENT_BUTTON_LEFT) && (p.x < AMBIENT_BUTTON_LEFT + AMBIENT_BUTTON_WIDTH))                      // check horizontal constraints
    {
        if ((p.y > AMBIENT_BUTTON_HEIGHT_OFFSET) && (p.y < AMBIENT_BUTTON_HEIGHT_OFFSET + PROBE_BUTTON_HEIGHT)) // now check vertical constraints
        {
            sketch->probeButtons(0);                                                    // clear potential 'selection' highlight from probe button
            sketch->ambientProbeButton(true);
            Probe_v3 *coolerProbe = probeArray[0];
            currentProbeIndex = 0;

            double temperature = coolerProbe->getCurrentTemperature();                         // get current temp
            sketch->temperatureString(temperature);                                     // display to user
        }
    }
    //check for a probe button being pressed
    if ((p.y > PROBE_BUTTON_HEIGHT_OFFSET) && (p.y < (PROBE_BUTTON_HEIGHT_OFFSET + PROBE_BUTTON_HEIGHT)))       // get vertical constraints of probe buttons
    {
        /*----FIND WHICH PROBE BUTTON SELECTED----*/
        for (int i = 1; i < 6; i++)                                                     // iterate through probe buttons(human numbered)
        {

            int   button_left = (SMALL_PAD * i) + (PROBE_BUTTON_WIDTH * (i - 1)),       // left boundary of button
                  button_right = button_left + PROBE_BUTTON_WIDTH;                      // right boundary of button

            if ((p.x > button_left) && (p.x < button_right))                            // if user touch within boundaries
            {
                sketch->ambientProbeButton(false);                                      // clear potential 'selected' highlight from cooler button
                Probe_v3 *probe = probeArray[i];                                        // set probe reference to appropriate reference of global probe array
                currentProbeIndex = i;                                                  //set current selection to data member for later access
                sketch->probeButtons(i);                                                /*repaint buttons so user sees their selection*/

                double temperature = probe->getCurrentTemperature();                    // get current temperature of probe
                sketch->temperatureString(temperature);                                 //draw new probe temperature
            }
        }
    }
    /*----NOW WE CHECK UP&DOWN (+-) BUTTON PRESS----*/
    if ((p.x > (SCROLL_UP_LEFT)) && (p.x < SCROLL_UP_LEFT + SCROLL_BUTTON_WIDTH))
    {

        Probe_v3 *probe = probeArray[currentProbeIndex];                            // get the probe to work with

        if ((p.y > SCROLL_UP_HEIGHT_OFFSET) && (p.y < (SCROLL_UP_HEIGHT_OFFSET + SCROLL_BUTTON_HEIGHT)))        //'+' button pressed
        {
            double newCalibrationAdjust = 0.01;                                         //  ADJUSTMENTS +- .01 DEGREE.

            probe->adjustCalibrationOffset(newCalibrationAdjust);                             // set calibration adjustment for this probe
            probe->setCurrentTemperature();                                                    // update temperature with new calibration
            double newTemp = probe->getCurrentTemperature();                                   // new temp to display to user
            sketch->temperatureString(newTemp);
            calibrationUpdateRequired = 1;                                              // flag to update calibration offset in EEPROM
        }
        else if ((p.y > SCROLL_DN_HEIGHT_OFFSET) && (p.y < (SCROLL_DN_HEIGHT_OFFSET + SCROLL_BUTTON_HEIGHT))) // '-' button pressed
        {
            double newCalibrationAdjust = -0.01;                                        //  ADJUSTMENTS +- .01 DEGREE.
            probe->adjustCalibrationOffset(newCalibrationAdjust);                             // set calibration adjustment for this probe
            probe->setCurrentTemperature();                                                // update temperature with new calibration
            double newTemp = probe->getCurrentTemperature();                                   // new temp to display to user
            sketch->temperatureString(newTemp);
            calibrationUpdateRequired = 1;
        }
    }
    //check for 'done' button press
    if ((p.x > (DONE_BUTTON_LEFT)) && (p.x < DONE_BUTTON_LEFT + DONE_BUTTON_WIDTH))
    {
        if ((p.y > DONE_BUTTON_HEIGHT_OFFSET) && (p.y < (DONE_BUTTON_HEIGHT_OFFSET + DONE_BUTTON_HEIGHT)))
        {

            disableInput();
            sketch->bigDoneButton(true);                                                // user has selected 'done'
            delay(250);
            currentProbeIndex = -1;                                                     // clear associated probe
            setCurrentMenu(MAIN_MENU);                                                  // set program to main menu
            screen->fillScreen();                                                       // clear screen
            sketch->mainMenu(0);                                                        // redraw main menu
            enableInput();
        }
    }
    return calibrationUpdateRequired;
}

void InputProcessor_v3::doCriticalViolationScreen(Point& p)
{

}

//INITIALIZE PROBE TRACKING, FINALLIZE PROBE/ITEM/EMPLOYEE RELATIONSHIP
void InputProcessor_v3::initializeProbeTracking(Probe_v3*& probe, String employee)
{
    DateTime tempDate = clock->now();
    Util::logInitialTrackingInformation(*sdCard, probe, &tempDate, employee );          //log initial data to SD card
    probe->setProbeState(ACTIVE);                                                            /*mark probe as taken*/
    probe->setEmployeeName(employee);                                                       //set employee name
    probe->setCurrentTemperature();                                                            //update current temperature of probe

    String sessId = Util::getTransactionID(tempDate);                                   //get a new session ID for this probe
    probe->setSessionID(sessId);                                                        //set unique ID for this cooling session
	
    probe->setInitialTemperature();                                                     // set initial temperature for item tracking
    probe->logReading(probe->getCurrentTemperature());                                         //populate initial reading in data log
}


/*stepUpItemSelectionList() assesses current position within item array,
and decrements value to 'move up' the displayed list of items
works with data members 'currentItemSelectionIndex',
which holds the currently selected index of the item list that has been highlighted,
and 'staticRefreshIndex' which holds the beginning index to display when there is no change in the items displayed
(updated when shift to new set of items occurs)
builds an integer array of indexes to display, in alphabetical order, automatically wrapping from top of array to the bottom*/
void InputProcessor_v3::stepUpItemSelectionList(DataWrapper &source)
{

    int numberItems = source.getNumberItems();				                            //number of items in the list
    int arrayMax = numberItems - 1;							                            //number of indicies in the array
    bool doShift = false;									                            // flag for shifting the displayed names to next grouping
    int lowIndex, highIndex;								                            // hold values of lowest and highest indicies to display
    int indexArray[ITEM_SELECT_NUMBER_SHOWN];				                            // hold each line's index value to retrieve text if we're shifting display
    int displayMax = ITEM_SELECT_NUMBER_SHOWN - 1;			                            // max index of the temp display array


    if (currentItemSelectionIndex == 0)/*decrement selected array index, wrap to end of array if already at beginning*/
    {
        currentItemSelectionIndex = arrayMax;
    }
    else
    {
        currentItemSelectionIndex--;
    }
    if (currentListPosition <= 1)                                                       // now update display position value
    {
        currentListPosition = ITEM_SELECT_NUMBER_SHOWN;
        /*wrapping  to bottom of display, now need to update the list of items that are being displayed*/
        doShift = true;
    }
    else
    {
        currentListPosition--;
    }
    /*for each line of display, evaulate its relationship to the current list position
    to determine what array index values to display before and after. store values in array to pass to
    item rendering function*/
    if (doShift)
    {
        if ((currentItemSelectionIndex - displayMax) < 0)// start at nth up from the top of the last screen, wrap to array max if fall below floor
        {

            int offset = (displayMax - currentItemSelectionIndex) - 1; //difference of shift minus current index, less 1 for shift from bottom of array to the top

            lowIndex = arrayMax - offset;	// get the leftover from below array floor and subtract from top of array
            // add 1 to adjust for the step from 0 to max
            highIndex = currentItemSelectionIndex;
        }
        else
        {
            lowIndex = currentItemSelectionIndex - (ITEM_SELECT_NUMBER_SHOWN - 1);
            highIndex = currentItemSelectionIndex;

        }

        /*set lowest index to display, this only changes if we're shifting the set of items shown */
        staticRefreshIndex = lowIndex; // update the placeholder lowest index for writing items on screen (for rewrite with no shift up the item list)

        int variableIndicies = ITEM_SELECT_NUMBER_SHOWN - 2;	// get number of lines to be filled with indicies between high and low position

        indexArray[0] = lowIndex;								// hold lowest index in lowest index (top of display)

        indexArray[displayMax] = highIndex;						//hold temp new index in highest index (bottom row of display)

        int index_decremeter = highIndex - 1;						// reference to index just before currently selected

        for (int i = variableIndicies; i > 0; i--) 					// step backward through all indicies to be filled and set index values to display
        {

            if (index_decremeter < 0)
            {

                index_decremeter = arrayMax;
            }
            indexArray[i] = index_decremeter;
            index_decremeter--;
        }
        /*repaint items display with new set of names*/
        sketch->itemSelection(staticRefreshIndex, ITEM_SELECT_NUMBER_SHOWN, currentListPosition, source, indexArray);

    }
    else     // no shift of displayed names required, simply redraw same items with different highlight
    {
        sketch->itemSelection(staticRefreshIndex, ITEM_SELECT_NUMBER_SHOWN, currentListPosition, source);
    }
}

/*function assesses current position within item array, and increment value to move through the displayed list of items
works with data members 'currentItemSelectionIndex', which holds the currently selected index of the item list that has been highlighted,
and 'staticRefreshIndex' which holds the beginning index to display when there is no change in the items displayed
(updated when shift to new set of items occurs)
builds an integer array of indexes to display, in alphabetical order, automatically wrapping from top of array to the bottom*/
void InputProcessor_v3::stepDownItemSelectionList(DataWrapper &source)
{
    int numberItems = source.getNumberItems();				//number of items in the list
    int arrayMax = numberItems - 1;							//number of indicies in the array
    bool doShift = false;									// flag for shifting the displayed names to next grouping
    int lowIndex, highIndex;								// hold values of lowest and highest indicies to display
    int indexArray[ITEM_SELECT_NUMBER_SHOWN];				// hold each line's index value to retrieve text if we're shifting display
    int displayMax = ITEM_SELECT_NUMBER_SHOWN - 1;			// max index of the temp display array

    /*increment selected array index, wrap to beginning of array if at end*/
    if (currentItemSelectionIndex == arrayMax)
    {
        currentItemSelectionIndex = 0;
    }
    else
    {
        currentItemSelectionIndex++;
    }

    // now update display position value
    if (currentListPosition >= 3)
    {

        currentListPosition = 1;
        /*wrapping  to top of display, now need to update the list of items that are being displayed*/
        doShift = true;

    }
    else
    {
        currentListPosition++;
    }

    /*for each line of display, evaulate its relationship to the current list position
    to determine what array index values to display before and after. store values in array to pass to
    item rendering function*/
    if (doShift)
    {

        if ((currentItemSelectionIndex + displayMax) > arrayMax) 	// accumulate the indicies to display, wrap to array beginning if overstep max index
        {

            int offset = (currentItemSelectionIndex + displayMax) - (arrayMax + 1); //subtract array max index form total value of itemindex + number line displayed

            lowIndex = currentItemSelectionIndex;	//first item to display on new screen is selected item

            highIndex = offset;

        }
        else
        {

            highIndex = currentItemSelectionIndex + displayMax;
            lowIndex = currentItemSelectionIndex;

        }

        /*set lowest index to display, this only changes if we're shifting the set of items shown */
        staticRefreshIndex = lowIndex; // update the placeholder lowest index for writing items on screen (for rewrite with no shift up the item list)

        int variableIndicies = ITEM_SELECT_NUMBER_SHOWN - 2;	// get number of lines to be filled with indicies between high and low position

        indexArray[0] = lowIndex;								// hold lowest index in lowest index (top of display)

        indexArray[displayMax] = highIndex;						//hold temp new index in highest index (bottom row of display)

        int index_decremeter = highIndex - 1;						// reference to index just before currently selected

        for (int i = variableIndicies; i > 0; i--) 					// step backward through all indicies to be filled and set index values to display
        {

            if (index_decremeter < 0)
            {

                index_decremeter = arrayMax;
            }
            indexArray[i] = index_decremeter;
            index_decremeter--;
        }
        /*repaint items display with new set of names*/

        sketch->itemSelection(staticRefreshIndex, ITEM_SELECT_NUMBER_SHOWN, currentListPosition, source, indexArray);
    }
    else     // no shift of displayed names required, simply redraw same items with different highlight
    {
        sketch->itemSelection(staticRefreshIndex, ITEM_SELECT_NUMBER_SHOWN, currentListPosition, source);
    }

}
//GENERIC UP DOWN BUTTONS USED IN MANY STATES! SWITCH(currentMenu) DETERMINES ACTION OF ANY GIVEN MENU
void InputProcessor_v3::checkUpDownButtons(Point& p)
{
    /*up or down button selection - if within horizontal boundaries of up/down buttons*/
    if ((p.x > (SCROLL_UP_LEFT)) && (p.x < SCROLL_UP_LEFT + SCROLL_BUTTON_WIDTH))
    {
        if ((p.y > SCROLL_UP_HEIGHT_OFFSET) && (p.y < (SCROLL_UP_HEIGHT_OFFSET + SCROLL_BUTTON_HEIGHT)))        //UP button pressed
        {
            disableInput(); // dont let input interfere with calculations

            sketch->scrollButtons(1);
            delay(250);
            sketch->scrollButtons(0);

            switch (currentMenu)
            {

            case ASSIGN_PROBE:
            {
                stepUpItemSelectionList(*items);
                break;

            }

            case ASSIGN_EMPLOYEE:
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING:
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING:
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_INVALID_DATA_WARNING:
            {
                stepUpItemSelectionList(*employees);
                break;
            }
            }


            enableInput(); // let user make another selection

        }
        //DOWN BUTTON PRESSED
        if ((p.y > SCROLL_DN_HEIGHT_OFFSET) && (p.y < (SCROLL_DN_HEIGHT_OFFSET + SCROLL_BUTTON_HEIGHT)))
        {
            disableInput(); // dont let input interfere with calculations

            sketch->scrollButtons(2);
            delay(250);
            sketch->scrollButtons(0);

            switch (currentMenu)
            {

            case ASSIGN_PROBE:
            {
                stepDownItemSelectionList(*items);
                break;

            }

            case ASSIGN_EMPLOYEE:
            {
                stepDownItemSelectionList(*employees);
                break;
            }
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_AMBIENT_WARNING:
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_TT_WARNING:
            case CONFIRM_EMPLOYEE_ACKNOWLEDGE_INVALID_DATA_WARNING:
            {
                stepDownItemSelectionList(*employees);
                break;
            }
            }
            enableInput(); // let user make another selection
        }
    }
}

/*returns boolean true-confirm or false-cancel as well as adjusting reference var
@param trigger:  reference var should be set to 0 before calling this function, and will be changed to 1
of button press falls within boundaries.
ex:     int trigger = 0;
bool result = checkOkCancelButtons(p, trigger);
if(trigger ==1){
//do stuff here with bool return val since button has been pressed
}*/
bool InputProcessor_v3::checkOkCancelButtons(Point& p, int& trigger)
{
    bool result = false;                                                                // default false return

    trigger = 0;                                                                        //default no button press passed to ref. var

    /*OK/CANCEL BUTTON  WIDTH CONSTRAINTS*/
    if ((p.x > (OK_BUTTON_LEFT)) && (p.x < OK_BUTTON_LEFT + OK_CXL_BUTTON_WIDTH))
    {
        /*OK BUTTON HEIGHT CONSTRAINTS*/
        if ((p.y > OK_BUTTON_HEIGHT_OFFSET) && (p.y < (OK_BUTTON_HEIGHT_OFFSET + OK_BUTTON_HEIGHT)))
        {
                                                                                        /*if we're clear to proceed, pass trigger value*/
            trigger = 1;
            return true;
        }
        /*CANCEL HEIGHT BUTTON CONSTRAINT*/
        else if ((p.y > CXL_BUTTON_HEIGHT_OFFSET) && (p.y < (CXL_BUTTON_HEIGHT_OFFSET + OK_BUTTON_HEIGHT)))
        {
            /*fire the trigger, return false for cancel*/
            trigger = 1;
            return false;
        }
    }

    return false;                                                                       // user press, if any, is outside button area
}


//ALLOW USER INPUT
void InputProcessor_v3::enableInput()
{
    enabled = true;
}

//DISALLOW USER INPUT
void InputProcessor_v3::disableInput()
{
    enabled = false;
}

//IS USER INPUT ENABLED?
bool InputProcessor_v3::inputIsEnabled()
{
    return enabled;
}

//SET CURRENT ENUM VALUE TO MENU (systemConstants.h)
void InputProcessor_v3::setCurrentMenu(int menu)
{
	currentMenu = menu;
}

//GET VALUE OF CURRENT MENU
int InputProcessor_v3::getCurrentMenu()
{
    	return currentMenu;
}

void InputProcessor_v3::setTransactionID(String id)
{
	transactionID = id;
}

String InputProcessor_v3::getTransactionId()
{
	return transactionID;
}

