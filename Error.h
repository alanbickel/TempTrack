
#ifndef ERROR_H

#define ERROR_H

#include "Message.h"
//#include <String.h>

class Error{

	private: 
	
		Message *msg;

	public:
		
		Error(TFT scrnRef, String errorText, String errCode);

		void doFatalStall();
};

#endif
/*same behavior as message object, then appends, three lines of text.  1st is "error code:" followed by the error code.  Second is "see 'iT3 error codes' ", 3rd is "for details"*/
