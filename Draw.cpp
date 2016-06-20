
#include "Draw.h"



/*CONSTRUCT*/
Draw::Draw(TFT &scrn)
{
    screen = &scrn;     // store reference to Global TFT object
}

/*MAIN MENU
@param selected: selected button, is redrawn with highlighted background color to show selection
*/
void Draw::mainMenu(int selected)
{

    int numOptions = 4,
        optNum = selected - 1, // get selected menu option
        highlight[4];				// hold reference to highlight selected menu option

    String text[] = {"Begin Tracking",  "Quick Temp Check",  "Item Progress",  "Calibration"};

    for(int i = 0; i < numOptions; i++)  // 'highlight' selected menu button by changing color on screen
    {

        if(optNum == i)
        {

            highlight[i] = GRAY2;

        }
        else
        {

            highlight[i] = GRAY1;
        }

    }
    //draw buttons
    for(int i =1; i <5; i++)
    {
        int upperButtonCoord = HEIGHT *(i-1) + (PADDING_SPACE *i); // define upper boundry of button
        //get vertical alignment
        int v_align = getTextVerticalAlignment(i, 4,TXT_BOX_HEIGHT, SMALL_PAD);

        // get horizontal alignment
        int h_align = getTextCenterAlignment(text[i-1].length(), 2);

        screen->fillRectangle(LEFT,upperButtonCoord,WIDTH,HEIGHT, highlight[i-1]); // draw  button


        //TextFunctions::writeCenteredText(screen, text[i-1], length[i-1], getTextVertAlign(i)-5, 2, BLUE); // draw text on button
        screen->drawString(text[i-1].c_str(), h_align , upperButtonCoord+18 , 2, BLACK);
    }


}

/*select employee name to associate with cooling item*/
void Draw::employeeSelectMenu(DataWrapper &source)
{

    String prompt = "Select Employee:";

    int h_align = Draw::getTextCenterAlignment(prompt.length(), 2);
    int v_align = Draw::getTextVerticalAlignment(1,2,TXT_BOX_HEIGHT, SMALL_PAD);
    screen->drawString(prompt.c_str(),h_align ,v_align , 2, WHITE);

    /*display list of names*/
    itemSelection(0, ITEM_SELECT_NUMBER_SHOWN, 1, source);
    scrollButtons(0);
    selectBackButtons(0);

}

void Draw::employeeAcknowledgeWarning(DataWrapper &source, String warningType) {
	String prompt = "Confirm ";
	String prompt2 = warningType;
	prompt2 += " Warning";

	int h_align = Draw::getTextCenterAlignment(prompt.length(), 2);
	int h_align_2 = Draw::getTextCenterAlignment(prompt2.length(), 2);
	int v_align = Draw::getTextVerticalAlignment(1, 2, TXT_BOX_HEIGHT, SMALL_PAD);
	int v_align_2 = v_align + 18;
	screen->drawString(prompt.c_str(), h_align, v_align, 2, WHITE);
	screen->drawString(prompt2.c_str(), h_align_2, v_align_2, 2, WHITE);

	/*display list of names*/
	itemSelection(0, ITEM_SELECT_NUMBER_SHOWN, 1, source);
	scrollButtons(0);
	bigDoneButton(false);
}


/*Begin Tracking Item Menu
wrapper function for displaying probe selection/ item assignment.
*/
void Draw::itemTrackingMenu(DataWrapper &source)
{


    /*directions for user*/
    String probePrompt = "Select Probe:";
    int h_align = Draw::getTextCenterAlignment(probePrompt.length(), 2);
    int v_align = Draw::getTextVerticalAlignment(1,2,TXT_BOX_HEIGHT, SMALL_PAD);
    screen->drawString(probePrompt.c_str(), h_align , v_align , 2, WHITE);

    probeButtons(0); // draw probe buttons

    /*draw item selection list*/
    itemSelection(0, ITEM_SELECT_NUMBER_SHOWN, 1, source);
    scrollButtons(0);
    selectBackButtons(0);

}

void Draw::probeButtons(int selected)
{

    int optNum = selected - 1,		// get selected probe button (if any)
        highlight[5],				// hold reference to highlight selected probe button
        numOptions = 5;				// number of buttons

    for(int i = 0; i < numOptions; i++)  // 'highlight' selected menu button by changing color on screen
    {
        if(optNum == i)
        {
            highlight[i] = GRAY2;
        }
        else
        {
            highlight[i] = GRAY1;
        }
    }
    /*draw menu buttons*/
    for(int i = 1; i < 6; i++)
    {

        int loop_pos = i-1,

            button_left = (SMALL_PAD * i) + (PROBE_BUTTON_WIDTH * loop_pos);

        screen->fillRectangle(	button_left,
                                PROBE_BUTTON_HEIGHT_OFFSET,
                                PROBE_BUTTON_WIDTH,
                                PROBE_BUTTON_HEIGHT,
                                highlight[loop_pos]);

        int button_horizontal = button_left + (PROBE_BUTTON_WIDTH/3);

        screen->drawNumber(i,button_horizontal, PROBE_BUTTON_HEIGHT_OFFSET+(PROBE_BUTTON_HEIGHT/3), 2,BLACK);
    }
}

/*	@param selectedIndex :	array index to start at, if not using reference array
@param numShown:		number if items in list to display
@param listPosition:	highlight 'nth' line to show current selection
@param list:			Data Wrapper object to pull item names form
@param displayArray:	optional array of index vlues to display. used to update screen after'up' or 'down' button pressed
*/
void Draw::itemSelection(int selectedIndex, int numShown, int listPosition, DataWrapper list, int *displayArray)
{


    int size = list.getNumberItems();


    String *names = list.getNameArray();

    /* set each button background to highlight current selection*/
    int *highlight = new int[numShown];

    for(int i = 0; i < numShown; i++ )
    {

        if((listPosition-1) == i)
        {
            highlight[i] = GRAY2;
        }
        else
        {
            highlight[i] = GRAY1;
        }
    }


    for(int i = 0; i < numShown; i++)
    {

        int buttonOffset = ITEM_SELECT_BUTTON_OFFSET + (ITEM_SELECT_BUTTON_HEIGHT*(i+1)); 		// button vertical alignment
        int textOffset = buttonOffset + (ITEM_SELECT_BUTTON_HEIGHT/2); 							// center text vertically over button

        screen->fillRectangle(LEFT,(buttonOffset +(i*2)), ITEM_SELECT_BUTTON_WIDTH,				// draw button
                              ITEM_SELECT_BUTTON_HEIGHT, highlight[i] );

        // if no integer array passed, we're not updating from button press, so initial display
        if(!displayArray)
        {

            /* start at current position within array*/
            int displayLength = names[selectedIndex].length();										// get length of current string

            int h_align = Draw::getTextCenterAlignment(displayLength, 2);							// get horizontal alingment for string display

            screen->drawString(names[selectedIndex].c_str(), h_align , textOffset , 2, BLACK);

            if(selectedIndex == (size-1)) 															// if incrementing will overstep array bounds,
            {
                //wrap around to zero
                selectedIndex  = 0;
            }
            else
            {
                selectedIndex  ++;
            }
            // in array passed, so we're updating from up or down button press
        }
        else
        {
            int displayLength = names[displayArray[i]].length();										// get length of current string

            int h_align = Draw::getTextCenterAlignment(displayLength, 2);							// get horizontal alingment for string display

            screen->drawString(names[displayArray[i]].c_str(), h_align , textOffset , 2, BLACK);
        }
    }



    delete [] highlight; // free memory from highlight array
}

void Draw::employeeSelection(int selectedIndex, int numShown, int listPosition, DataWrapper list, int *displayArray)
{

    int size = list.getNumberItems();
    String *names = list.getNameArray();

    /* set each button background to highlight current selection*/
    int *highlight = new int[numShown];

    for(int i = 0; i < numShown; i++ )
    {

        if((listPosition-1) == i)
        {
            highlight[i] = GRAY2;
        }
        else
        {
            highlight[i] = GRAY1;
        }
    }
    for(int i = 0; i < numShown; i++)
    {

        int buttonOffset = EMPLOYEE_LIST_OFFSET + (EMPLOYEE_BUTTON_HEIGHT*(i+1)); 		// button vertical alignment
        int textOffset = buttonOffset + (EMPLOYEE_BUTTON_HEIGHT/2); 							// center text vertically over button

        screen->fillRectangle(LEFT,(buttonOffset +(i*2)), EMPLOYEE_BUTTON_WIDTH,				// draw button
                              EMPLOYEE_BUTTON_HEIGHT, highlight[i] );

        // if no integer array passed, we're not updating from button press, so initial display
        if(!displayArray)
        {

            /* start at current position within array*/
            int displayLength = names[selectedIndex].length();										// get length of current string

            int h_align = Draw::getTextCenterAlignment(displayLength, 2);							// get horizontal alingment for string display

            screen->drawString(names[selectedIndex].c_str(), h_align , textOffset , 2, BLACK);

            if(selectedIndex == (size-1)) 															// if incrementing will overstep array bounds,
            {
                //wrap around to zero
                selectedIndex  = 0;
            }
            else
            {
                selectedIndex  ++;
            }
            // in array passed, so we're updating from up or down button press
        }
        else
        {
            int displayLength = names[displayArray[i]].length();										// get length of current string

            int h_align = Draw::getTextCenterAlignment(displayLength, 2);							// get horizontal alingment for string display

            screen->drawString(names[displayArray[i]].c_str(), h_align , textOffset , 2, BLACK);
        }
    }


    delete [] highlight; // free memory from highlight array
}

void Draw::temperatureString(double temperature)
{
    String txtTemp = "Current Temp: ";
    String dTemp(temperature);
    txtTemp+= dTemp;
    int horizAlign = Draw::getTextCenterAlignment(txtTemp.length(), 6);

    int vertAlign =  (TEMP_BOX_HEIGHT/2) + TEMP_BOX_VERT_ALIGN;

    //black out screen behind where temp is displayed, effectively 'overwrites' old temperature
    Tft.fillRectangle(0, TEMP_BOX_VERT_ALIGN, TEMP_BOX_WIDTH ,TEMP_BOX_HEIGHT, BLACK);

    Tft.drawString(txtTemp.c_str(), 0, vertAlign, 2 , GREEN );
}

void Draw::employeeAcknowledgeScreen() {

}


void Draw::calibrationScreen(int highlight, float temp)
{
    bool ambientHighlight;

    if(highlight = AMBIENT_PROBE_READING)
    {
        ambientHighlight = true;
    }
    else
    {
        ambientHighlight = false;
    }

    /*set text display off whichever probe has been selected*/

    String probePrompt = "Select Probe:";

    int prompt_horiz_align = Draw::getTextCenterAlignment(probePrompt.length(), 2);
    int prompt_vert_align = Draw::getTextVerticalAlignment(1,2,TXT_BOX_HEIGHT, SMALL_PAD);

    Tft.drawString(probePrompt.c_str(), prompt_horiz_align , prompt_vert_align , 2, WHITE); // directions for user
    probeButtons(highlight);            // draw probe buttons

    temperatureString(temp);        // draw temperature on screen

    scrollButtons(0, true);     // true flag displays scroll buttons as '+' and '-' instead of text
    bigDoneButton(false);
    ambientProbeButton(ambientHighlight);



}

void Draw::bigDoneButton(bool selected){
    int highlight;
    if(selected){
        highlight = GRAY2;
    } else {
        highlight = GRAY1;
    }
    char text[] = "Done";

    int txt_horiz_align = (CENTER_SCREEN/2) - 18;
    int txt_vert_align = DONE_BUTTON_HEIGHT_OFFSET + (DONE_BUTTON_HEIGHT/2.5);

    screen->fillRectangle(DONE_BUTTON_LEFT, DONE_BUTTON_HEIGHT_OFFSET, DONE_BUTTON_WIDTH, DONE_BUTTON_HEIGHT, highlight);
    screen->drawString(text,txt_horiz_align,txt_vert_align,2,BLACK);


}
/*
draw 'select' and 'back' buttons.
@param: selectedButton: int value of which(if any) button has been selected
*/
void Draw::selectBackButtons(int selectedButton)
{

    int selection[2];
    switch(selectedButton) 			//determine which button to highlight based on user input
    {

    case 0:
    {
        selection[0] = GRAY1;
        selection[1] = GRAY1;
        break;
    }
    case 1:
    {
        selection[0] = GRAY2;
        selection[1] = GRAY1;
        break;
    }
    case 2:
    {
        selection[0] = GRAY1;
        selection[1] = GRAY2;
        break;
    }
    }

    char confirmText[] = "Select";
    char cancelText[] = "Back";

    int ok_horiz_align = (CENTER_SCREEN/2) - 18;						// center text in middle of ech button
    int cxl_horiz_align = (CENTER_SCREEN/2) - 12;

    int ok_txt_height_align = OK_BUTTON_HEIGHT_OFFSET + (OK_BUTTON_HEIGHT/4);				// get vertical position for text
    int cxl_txt_align = CXL_BUTTON_HEIGHT_OFFSET + (OK_BUTTON_HEIGHT/4);

    screen->fillRectangle(OK_BUTTON_LEFT, OK_BUTTON_HEIGHT_OFFSET, OK_CXL_BUTTON_WIDTH, OK_BUTTON_HEIGHT, selection[0]);
    screen->drawString(confirmText,ok_horiz_align,ok_txt_height_align,2,BLACK);
    screen->fillRectangle(CXL_BUTTON_LEFT, CXL_BUTTON_HEIGHT_OFFSET, OK_CXL_BUTTON_WIDTH, OK_BUTTON_HEIGHT, selection[1]);
    screen->drawString(cancelText,cxl_horiz_align,cxl_txt_align,2,BLACK);

}

void Draw::scrollButtons(int selectedButton, bool isCalibrationMenuCalling)
{

    int selection[2];

    switch(selectedButton) 			//determine which button to highlight based on user input
    {
    case 0:
    {
        selection[0] = GRAY1;
        selection[1] = GRAY1;
        break;
    }
    case 1:
    {
        selection[0] = GRAY2;
        selection[1] = GRAY1;
        break;
    }
    case 2:
    {
        selection[0] = GRAY1;
        selection[1] = GRAY2;
        break;
    }
    }
    String upTxt;
    String dnTxt;
    int up_txt_horiz_alignment;
    int dn_txt_horiz_alignment;
    if(!isCalibrationMenuCalling)      // if we need text displayed
    {
        upTxt = "Up";
        dnTxt = "Down";
        up_txt_horiz_alignment = SCROLL_UP_LEFT + (SCROLL_BUTTON_WIDTH/2) - 12;
        dn_txt_horiz_alignment = SCROLL_DN_LEFT + (SCROLL_BUTTON_WIDTH/2) - 18;

    }
    else                                // show '+' and '-' for calibration screen
    {
        upTxt = "+";
        dnTxt = "-";
         up_txt_horiz_alignment = SCROLL_UP_LEFT + (SCROLL_BUTTON_WIDTH/2) - 12;
         dn_txt_horiz_alignment = SCROLL_DN_LEFT + (SCROLL_BUTTON_WIDTH/2) - 12;

    }


    /*horizontal offset for text*/

    int up_text_vert_align = SCROLL_UP_HEIGHT_OFFSET + (SCROLL_BUTTON_HEIGHT/4);
    int dn_text_vert_align = SCROLL_DN_HEIGHT_OFFSET + (SCROLL_BUTTON_HEIGHT/4);

    screen->fillRectangle(SCROLL_UP_LEFT, SCROLL_UP_HEIGHT_OFFSET, SCROLL_BUTTON_WIDTH, SCROLL_BUTTON_HEIGHT, selection[0]);
    screen->drawString(upTxt.c_str(),up_txt_horiz_alignment,up_text_vert_align,2,BLACK);
    screen->fillRectangle(SCROLL_DN_LEFT, SCROLL_DN_HEIGHT_OFFSET, SCROLL_BUTTON_WIDTH, SCROLL_BUTTON_HEIGHT, selection[1]);
    screen->drawString(dnTxt.c_str(),dn_txt_horiz_alignment,dn_text_vert_align,2,BLACK);
}

void Draw::ambientProbeButton(bool selected)
{
    String ambient = "Cooler";
    int highlight;
    if(selected)
    {
        highlight = GRAY2;
    }
    else
    {
        highlight = GRAY1;
    }
    screen->fillRectangle(AMBIENT_BUTTON_LEFT, AMBIENT_BUTTON_HEIGHT_OFFSET, AMBIENT_BUTTON_WIDTH, PROBE_BUTTON_HEIGHT, highlight);

    int txtCenter = getTextCenterAlignment(ambient.length(), 2);
    int textVert = AMBIENT_BUTTON_HEIGHT_OFFSET +(PROBE_BUTTON_HEIGHT/3);

    screen->drawString(ambient.c_str(),txtCenter,textVert, 2 ,BLACK  );

}


/********************************************************
--------------STATIC TEXT ALIGNMENT FUNCTIONS------------
********************************************************/
/*
@param lineNumber:  current line of text
@param totalLines:  number of lines to display witin text area
@param boxHeight:   height of text area
@param padding :    buffer to space lines out

Return integer value of text offset from top of screen, based on number of total lines of text
within the given text area.  Provides evenly spaced text placement
*/
int Draw::getTextVerticalAlignment(int lineNumber, int totalLines, int boxHeight, int padding)
{

    int placement = lineNumber-1;

    int oneLineSpace = boxHeight / totalLines;

    int linePlacement = ((oneLineSpace * placement) + (padding * lineNumber));


    /*caveat for one line of text...*/
    if(totalLines ==1)
    {
        linePlacement = boxHeight/2; // auto center vertically on text box
    }
    return linePlacement;
}

/*
@param screen: TFT object to wtire to touchscreen
@param text : pointer to character array to display
@param txt_len:  length of text to display(do not count null terminator in char array)
@param vert_pos: vertical positioning of text
@param txt_size: size of text
@param color : text color

Draw text centered on screen
*/
int Draw::getTextCenterAlignment(int txt_len, int txt_size)
{

    int stringWidth = txt_len * CHARACTER_WIDTH * txt_size;
    int alignment = CENTER_SCREEN - (stringWidth/2);

    return alignment;
}

int Draw::getTextLeftQuadrantHorizontalAlignment(int txt_len, int txt_size )
{
    int stringWidth = txt_len * CHARACTER_WIDTH * txt_size;
    int alignment = (CENTER_SCREEN/2) - (stringWidth/2);

    return alignment;
}


