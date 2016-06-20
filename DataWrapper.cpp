#include "DataWrapper.h"
//#include <MemoryFree.h>

/*	---------------------------------------------------
CONSTRUCT
---------------------------------------------------
*/

DataWrapper::DataWrapper(String fileName){

	loaded = false; // set flag to determine if file loaded

	nameArray = NULL;
	numberArray = NULL;

	file = fileName; // set file name for later use

	numItems = getNumberItems(file); // get the number of items in this file

	setNameAndNumberArray(); // pull this file's info from SD card

	/*debug*/
	//serialTest();
}
DataWrapper::DataWrapper(SdFat &card){

    sdCard = &card;     // set pointer to initialized storage object

	loaded = false; // set flag to determine if file loaded
	nameArray = NULL;
	numberArray = NULL;
}



/*	---------------------------------------------------
SETTERS
---------------------------------------------------
*/
/*
@param file: file name to pull data from
~ this function pulls info from .csv and stores in array of strings.
uses ',' as delimiter, and '\n' for item seperation
*/
void DataWrapper::setNameAndNumberArray(){

	//SdFile myFile;

	int newLine = (int)'\n';	//  line terminator constant
	int delimiter = (int)',';	// delimiter constant
	int lineNum = 0;			// count the lines of the csv
	int delim_ID = 0;			// flag to tell if we've hit the delimiter

	/*hold each line value in temporary string	*/
	String name = "";
	String number = "";

	/*initialize field members to appropriate size arrays*/

	int size = getNumberItems();

	nameArray = new String[size];
	numberArray = new String[size];


    File dataFile = sdCard->open(file.c_str(), FILE_READ);
	if(dataFile){
		while (dataFile.available()){
			char inputChar = dataFile.read(); // Gets one byte from serial buffer

			if(inputChar == newLine){

				name.trim(); number.trim();	// clear out extraneous whitespace

				if((!name.equals("")) && (!number.equals(""))){  // if both strings hold value

					/*log name and number to appropriate array*/
					nameArray[lineNum] = name;
					numberArray[lineNum] = number;



					/*reset temporary arrays*/
					name = "";
					number = "";

					// valid data, we are successfully loaded file
					this->setLoadState(true);
				}
				lineNum++;			//increment line counter
				delim_ID = 0;		// reset delimiter flag
			} else {
				/*not newline*/
				if(inputChar == delimiter){  // if we're at the delimiter, set the flag
					delim_ID = 1;

				} else {

					if(delim_ID == 0 ){ // if we're before the comma, we're dealing with the name

						name.concat((String)inputChar);

					}
					else { // we're after the comma, so it is part of  number.

						number.concat((String)inputChar);

					}
				}
			}
		}
dataFile.close();
	}

}

void DataWrapper::setFileAssociation(String fileName){

	file = fileName; // set file name for later use

	numItems = getNumberItems(file); // get the number of items in this file

	setNameAndNumberArray(); // pull this file's info from SD card
}


/*	---------------------------------------------------
GETTERS
---------------------------------------------------
*/

//@param file: file name to open and count number of items.
int DataWrapper::getNumberItems(String file){

	SdFile myFile;

	const char *ptr = file.c_str();

	const int newLine = (int)'\n'; //  line terminator constant

	int lines = 0;

	File dataFile = sdCard->open(file.c_str(), FILE_READ);
	if(dataFile){
		while (dataFile.available()){

			char inputChar = dataFile.read(); // Gets one byte from serial buffer

			if (inputChar == newLine){ // if newline, increment counter
				lines++;
			}
		}
dataFile.close();
	}

	return lines;
}

//overloaded no arg call, returns field value
int DataWrapper::getNumberItems(){

	return numItems;
}

void DataWrapper::setLoadState(bool b){
	loaded = b;
}

bool DataWrapper::isLoaded(){
	return loaded;
}

String* DataWrapper::getNameArray(){
	return nameArray;
}
/*
void DataWrapper::serialTest(){

		Serial.print("file: ");
		Serial.println(file);
		Serial.print("number items:: ");
		Serial.println(numItems);
		Serial.print("array size: ");
		Serial.println(sizeof(nameArray[0]));
		Serial.print("freeMemory()=");
    Serial.println(freeMemory());

	for(int i = 0; i < numItems; i++){
		Serial.print("Name: ");
		Serial.println(nameArray[i]);
		Serial.print("Number: ");
		Serial.println(numberArray[i]);
	}

}
*/
