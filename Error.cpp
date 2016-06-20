
#include <TFTv2.h>

#include "Error.h"
#include "SystemConstants.h"
#include "Draw.h"

Error::Error(TFT scrnRef, String errorText, String errCode){

	String errCodeString = "Error Code: ";

	errCodeString += errCode; // append code number for user display

	/*create message object*/
	 msg = new Message(5, scrnRef);
	/*set text*/
	msg->setLine(errorText, 1);
	msg->setLine(errCodeString, 2);
	msg->setLine(ERR_1, 3);
	msg->setLine(ERR_2, 4);
	msg->setLine(ERR_3, 5);

	/*set output options*/
	msg->setColor(RED);
	msg->setTxtSize(2);

	msg->showMessage();

}

void Error::doFatalStall(){
	while(1);
}