

#ifndef MESSAGE_H

#define MESSAGE_H

#include "Draw.h"
#include "GraphicConstants.h"

#include <TFTv2.h>
//#include <String.h>

class Message {

	private: 

		int numLines;
		
		int txtColor;
	
		int txtSize;
	
		String	*msgPtr;

		TFT screen;
	


	public: 

		String *getMsg();
		
		Message(int numLines, TFT scrnRef);

		Message(String msg, TFT scrnRef); // quick one line message construct
	
		void setLine(String text, int lineNum);

		String getLine(int lineNum);
	
		void showMessage();
	
		void setColor(int color);

		void setTxtSize(int size);

		void setNumLines(int );

		void blankBox();

		void showLine(int line);

		~Message(){delete [] msgPtr;}
};

#endif

/*create object, set number of lines of text.
populate string array line by line with strings of text. 
showMessage(): 1. draws message box. 2: steps through array, gets each line, converts to c_str, gets horiz and vert alignment, draws text*/
