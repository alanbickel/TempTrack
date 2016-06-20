#ifndef DRAW_H
#define DRAW_H

#include <TFTv2.h>
#include "DataWrapper.h";
#include "GraphicConstants.h"
#include "SystemConstants.h"

class Draw
{
private:
    TFT *screen;

public:
    Draw(TFT &scrn);
    void mainMenu(int selected);
    void employeeSelectMenu(DataWrapper &source);
	void employeeAcknowledgeWarning(DataWrapper &source, String warningType);
    void itemTrackingMenu(DataWrapper &source);
    void probeButtons(int selected);
    void itemSelection(int selectedIndex, int numShown, int currentListPosition, DataWrapper list, int *displayArray = NULL);
    void employeeSelection(int selectedIndex, int numShown, int currentListPosition, DataWrapper list, int *displayArray = NULL);
    void calibrationAdjustScreen();
	void employeeAcknowledgeScreen();

    void calibrationScreen(int selectedIndex, float temp);
    void ambientProbeButton(bool selected);
    void tempDisplayBox(double temperature);
    void temperatureString(double temperature);


    void selectBackButtons(int selected);
    void scrollButtons(int selected, bool isCalibrationMenuCalling = false);
    void bigDoneButton(bool selected);

    static int getTextVerticalAlignment(int lineNumber, int totalLines, int boxHeight, int padding);
    static int getTextCenterAlignment(int txt_len, int txt_size);
    static int getTextLeftQuadrantHorizontalAlignment(int txt_len, int txt_size );
};

#endif
