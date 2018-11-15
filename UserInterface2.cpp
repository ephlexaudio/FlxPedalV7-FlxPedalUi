/*
 * UserInterface.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */



#include "UserInterface2.h"
#include "LookUpTable.h"
//#include "config.h"


namespace std
{



UserInterface2::UserInterface2() {
	// TODO Auto-generated constructor stub


	this->menuButtons[0] = this->initializePin(this->menuButtons[0],BUTTON0,"in");
	this->menuButtons[1] = this->initializePin(this->menuButtons[1],BUTTON1,"in");
	this->menuButtons[2] = this->initializePin(this->menuButtons[2],BUTTON2,"in");
	this->menuButtons[3] = this->initializePin(this->menuButtons[3],BUTTON3,"in");
	this->menuButtons[4] = this->initializePin(this->menuButtons[4],BUTTON4,"in");
	this->menuButtons[5] = this->initializePin(this->menuButtons[5],BUTTON5,"in");
	this->menuButtons[6] = this->initializePin(this->menuButtons[6],BUTTON6,"in");
	this->rotEncOut[0] = this->initializePin(this->rotEncOut[0],ROTENC_OUTA,"in");
	this->rotEncOut[1] = this->initializePin(this->rotEncOut[1],ROTENC_OUTB,"in");
	this->rotEncRst = this->initializePin(this->rotEncRst,RST_ROTENC_FFS,"out");
	this->rotEncReadCount = 0;
	this->powerButton = this->initializePin(this->powerButton,POWER_BUTTON,"in");
	this->inputCoupling[0] = this->initializePin(this->inputCoupling[0],IN2_AC_COUPLED,"out");
	this->inputCoupling[1] = this->initializePin(this->inputCoupling[1],IN2_DC_COUPLED,"out");
	this->inputCoupling[2] = this->initializePin(this->inputCoupling[2],IN2_GNDED,"out");

}

UserInterface2::~UserInterface2()
{
	// TODO Auto-generated destructor stub
}


#define dbg 0
GPIOClass UserInterface2::initializePin(GPIOClass pin, int pinNumber, string direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::initializePin: " << pinNumber << ": " << direction << endl;
#endif
	int status = 0;

	pin = GPIOClass(pinNumber);

	char dirCharArray[4];
	strncpy(dirCharArray, direction.c_str(),4);
	if(pin.export_gpio() == -1) status = -1;
	if(pin.setdir_gpio(dirCharArray) == -1) status = -1;

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::initializePin: " << status << endl;
#endif
	if(status == -1) pin = 0;
	return pin;
}

void UserInterface2::printPinData(GPIOClass pin)
{
#if(dbg >= 2)
	cout << "Pin number: " << pin.get_gpionum() << endl;
#endif
}



#define dbg 0
int UserInterface2::readButtons()
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::readButtons: " << endl;
#endif
	int buttonPushed = -1;

	for(int buttonIndex = 0; buttonIndex < 7; buttonIndex++)
	{
		int buttonValue;
		if(this->menuButtons[buttonIndex].getval_gpio(buttonValue) >= 0)
		{
			if(buttonValue == 1)
			{
				buttonPushed =  6 - buttonIndex; // buttons are backwards
			}
		}
		else
		{
#if(dbg >= 2)
			cout << "error reading button " << buttonIndex << endl;
#endif
		}
	}



#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::readButtons: " << buttonPushed[0] << ":" << buttonPushed[1] << endl;
#endif

	return buttonPushed;
}

#define dbg 0
bool UserInterface2::isPowerButtonPushed()
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::isPowerButtonPushed: "  << endl;
#endif
	bool status = false;
	int buttonValue = 0;

	this->powerButton.getval_gpio(buttonValue);
	if(buttonValue == 0) status = true;
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::isPowerButtonPushed: " << endl;
#endif

	return status;
}

void UserInterface2::resetEncoder()
{
	this->rotEncRst.setval_gpio(0);
	this->rotEncRst.setval_gpio(1);
}

#define dbg 0
int UserInterface2::readEncoder()
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::readEncoder: " << endl;
#endif


	int result = 0;
	int rotEncA = 0;
	int rotEncB = 0;
	this->rotEncOut[0].getval_gpio(rotEncA);
	this->rotEncOut[1].getval_gpio(rotEncB);
	result = -1*rotEncA + rotEncB;
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::readEncoder: " << rotEncA << ":" << rotEncB << "->" << result << endl;
#endif

	return result;
}

#define dbg 0
void UserInterface2::writeLcdLine(int lineNumber, string lineString)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdLine: " << endl;
#endif

	try
	{
		char sendCharArray[22];
		char lineCharArray[20];

		clearBuffer(sendCharArray,22);
		clearBuffer(lineCharArray,20);

		strncpy(lineCharArray,lineString.c_str(),20);
		sendCharArray[0] = (char)lineNumber;
		strncat(sendCharArray, lineCharArray, 21);
		int remainingZeroed = 22 - strlen(sendCharArray);
		for(int i = 0; i < remainingZeroed; i++)
		{
			strcat(sendCharArray," ");
		}
		sendCharArray[21] = 0;
		this->spi.sendData(sendCharArray,22);
		usleep(LCD_LINE_WRITE_SPACING);

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::writeLcdLine:" << e.what() << endl;
	}

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdLine: " << strlen(sendCharArray) << endl;
#endif
#if(dbg >= 2)
	cout << "********** " << sendCharArray << endl;
#endif
}

void UserInterface2::writeLcd(string line1, string line2, string line3, string line4)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcd: "  << endl;
#endif
	this->writeLcdLine(1,line1);
	this->writeLcdLine(2,line2);
	this->writeLcdLine(3,line3);
	this->writeLcdLine(4,line4);
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcd: " << status << endl;
#endif
}


void UserInterface2::writeLcdWithSoftKeys(string line1, string line2, string line3, vector<string> softKeyAbbrs)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdWithSoftKeys: "  << endl;
#endif
	this->writeLcdLine(1,line1);
	this->writeLcdLine(2,line2);
	this->writeLcdLine(3,line3);
	this->writeLcdLine(4,softkeyAlignedString(softKeyAbbrs));
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdWithSoftKeys: "  << endl;
#endif
}


void UserInterface2::writeLcdCentered(string line1, string line2, string line3, string line4)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdCentered: "  << endl;
#endif
	this->writeLcdLine(1,this->centeredString(line1,20));
	this->writeLcdLine(2,this->centeredString(line2,20));
	this->writeLcdLine(3,this->centeredString(line3,20));
	this->writeLcdLine(4,this->centeredString(line4,20));
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdCentered: "  << endl;
#endif
}



void UserInterface2::writeLcdHeader(int menuLevel, string title, string subtitle)
{
	string menuLcdString;
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdHeader: "  << endl;
#endif

	try
	{
		switch(menuLevel)
		{
			case 0:
				this->writeLcdLine(1,title);
				break;
			case 1:
				this->writeLcdLine(1,title);
				break;
			case 2:
				this->writeLcdLine(1,title + "->" + subtitle);
				break;
			default:;
		}

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::writeLcdHeader:" << e.what() << endl;
	}

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdHeader: " << endl;
#endif

}

#define dbg 1
void UserInterface2::writeLcdFxParameter(Parameter parameter)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdFxParameter: "  << endl;
#endif
	try
	{
		string parameterLine;
		int paramValueType = parameter.getValueType();
		int paramValueIndex = parameter.getValueIndex();
		parameterLine = parameter.getName() + ": " + lutArray[paramValueType][paramValueIndex] + lutArray[paramValueType][100];
		parameterLine.resize(20);
		this->writeLcdLine(2,parameterLine);

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::writeLcdFxParameter:" << e.what() << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdFxParameter: " << endl;
#endif

}

#define dbg 2
void UserInterface2::writeLcdUtilParameter(UtilParam parameter)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdUtilParameter: "  << endl;
#endif
#if(dbg >= 2)
	cout << parameter.name << ":" << parameter.paramType << ":" << parameter.option.option << endl;
#endif
	try
	{
		string parameterLine;

		ostringstream strs;
		string doubleStr;

		if(parameter.paramType == 0)
		{
			strs << parameter.doubleValue.value;
			doubleStr = strs.str();
			parameterLine = parameter.name + ":" + doubleStr;
		}
		else if(parameter.paramType == 1)
			parameterLine = parameter.name + ":" + parameter.option.option;

		this->writeLcdLine(2,parameterLine);

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::writeLcdUtilParameter:" << e.what() << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdUtilParameter: " << endl;
#endif

}

#define dbg 0
void UserInterface2::writeSoftKeys(int startIndex, vector<string> softKeyAbbrs)
{

	vector<string> keys;
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeSoftKeys: "  << endl;
#endif
	try
	{
		while(softKeyAbbrs.size() < 4) softKeyAbbrs.push_back(string(""));

		for(int paramIndex = 0; (4*startIndex+paramIndex) < static_cast<int>(softKeyAbbrs.size()) && (paramIndex < 4); paramIndex++)
		{
			keys.push_back(softKeyAbbrs[4*startIndex + paramIndex]);
		}
		string tempKeyString = this->softkeyAlignedString(keys);
		this->writeLcdLine(4,tempKeyString);

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::writeSoftKeys:" << e.what() << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeSoftKeys: " << endl;
#endif

}


string UserInterface2::softkeyAlignedString(vector<string> softkeys)
{
	string softkeyString;
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::softkeyAlignedString: "  << endl;
#endif

	try
	{
		for(auto & key : softkeys)
		{
			cout << key << endl;
		}
		while(softkeys.size()<4) softkeys.push_back(string(""));

		softkeyString.clear();


		for(int i = 0; i < 4; i++)
		{
			softkeyString += " ";
			while(softkeys[i].size()<4) // make sure softkeys[i] is 4 chars long
			{
				softkeys[i] += " ";
			}
			softkeyString += softkeys[i];

		}

	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::softkeyAlignedString: " << e.what() << endl;
	}

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::softkeyAlignedString: " << endl;
#endif

	return softkeyString;
}

string UserInterface2::softkeyAlignedString(string key1, string key2, string key3, string key4)
{
	string softkeyString;
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::softkeyAlignedString: " << endl;
#endif

	try
	{
		softkeyString.clear();
		string softKeys[4] = {key1,  key2,  key3,  key4};

		for(int i = 0; i < 4; i++)
		{
			softkeyString += " ";
			while(softKeys[i].size()<4) // make sure softkeys[i] is 4 chars long
			{
				softKeys[i] += " ";
			}
			softkeyString += softKeys[i];
		}
	}
	catch(exception &e)
	{
		cout << "exception in UserInterface2::softkeyAlignedString:" << e.what() << endl;
	}

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::softkeyAlignedString: " << endl;
#endif

	return softkeyString;
}

string UserInterface2::centeredString(string text, int totalStringLength)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::centeredString: " << endl;
#endif
	string centeredText;
	string paddingString;
	int paddingSize = (totalStringLength - text.size())/2;

	for(int i = 0; i < paddingSize; i++)
	{
		paddingString += " ";
	}
	centeredText = paddingString + text + paddingString;
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::centeredString: " << endl;
#endif

	return centeredText;
}

}
