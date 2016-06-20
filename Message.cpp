#include <TFTv2.h>
//#include <string>
#include "Message.h"
#include "GraphicConstants.h"
#include "Draw.h"

/*CONSTRUCT*/
Message::Message(int lines, TFT scrnRef) {

	/*create string array to hold strings for display text*/
	numLines = lines;
	msgPtr = new String[numLines];
	screen = scrnRef;

	/*set default options so user not required to before displaying message*/
	Message::setColor(BLACK);
	Message::setTxtSize(2);

}
Message::Message(String msg, TFT scrnRef) {

	Message::setColor(BLACK);
	Message::setTxtSize(2);
	numLines = 1;
	msgPtr = new String[numLines];

	setLine(msg, 1);

	screen = scrnRef;

	showMessage();
}




void Message::setLine(String text, int line) {
	int offSet = line - 1; // adjust for human 'line' numbers in array sequence
	msgPtr[offSet] = text;

}

String Message::getLine(int line) {
	return *(msgPtr + line); // get string
}

void Message::setNumLines(int n) {
	numLines = n;
}

void Message::setColor(int color) {

	txtColor = color;
}

void Message::setTxtSize(int size) {
	txtSize = size;
}

void Message::showMessage() {

	//background for text
	screen.fillRectangle(0, 0, TXT_BOX_WIDTH, TXT_BOX_HEIGHT, GRAY1);

	// step through string array and write each to screen

	for (int i = 0; i < numLines; i++) {

		//get address of string
		String ptr = getLine(i);


		int lineLength = ptr.length(); // length of string

			//get vertical alignment
		int v_align = Draw::getTextVerticalAlignment(i + 1, numLines, TXT_BOX_HEIGHT, SMALL_PAD);

		// get horizontal alignment
		int h_align = Draw::getTextCenterAlignment(lineLength, 2);

		screen.drawString(ptr.c_str(), h_align, v_align, txtSize, txtColor);

	}

}

void Message::blankBox() {

	screen.fillRectangle(0, 0, TXT_BOX_WIDTH, TXT_BOX_HEIGHT, GRAY1);
}

void Message::showLine(int line) {

	String ptr = getLine(line - 1); // offset line number for array index

	int lineLength = ptr.length(); // length of string

	//get vertical alignment
	int v_align = Draw::getTextVerticalAlignment(line, numLines, TXT_BOX_HEIGHT, SMALL_PAD);

	// get horizontal alignment
	int h_align = Draw::getTextCenterAlignment(lineLength, 2);

	screen.drawString(ptr.c_str(), h_align, v_align, txtSize, txtColor);
}

