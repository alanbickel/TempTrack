#ifndef DATAWRAPPER_H

#define DATAWRAPPER_H

#include <SdFat.h>


/*this class houses information about a specific file that is stored onto SD card.
pulls info from .csv files into memory to display to user.  Prep items, employee numbers, and corrective actions
are all mutable wth this class.*/
class DataWrapper{
private:

    SdFat *sdCard;      // pointer to storage volume
	int numItems;  // number of items in the file
	String file; // hold file name
	String *nameArray; // hold array of names from csv
	String *numberArray; // hold array of numbers from csv
	bool loaded;

public:
	DataWrapper(String fileName); // create file relationship
	DataWrapper(SdFat &card);

	void setFileAssociation(String fileName); // manual association of file to previously constructed DW object

	int getNumberItems(String fileName); // get number of items from given file on SD card
	int getNumberItems(); // return member field value

	void setNameAndNumberArray(); //create String array of item names

	/*return array pointers to accedd stored information*/
	String *getNameArray();
	String *getNumberArray();

	void setLoadState(bool b); // set whether or not arrays are properly loaded
	bool isLoaded();		// get load state

	/*debug*/
	void serialTest();
};


#endif
