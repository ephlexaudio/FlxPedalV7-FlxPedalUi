/*
 * UserInterface.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "config.h"

#include "UserInterface2.h"

#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::: " << endl;
#endif
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::: " << endl;
#endif

using namespace std;

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

	/*this->spiTest[0] = this->initializePin(this->spiTest[0],10, "out");
	this->spiTest[1] = this->initializePin(this->spiTest[1],11, "out");*/
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
	//if(pin->setval_gpio(pinNumber) == -1) status = -1;

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::initializePin: " << status << endl;
#endif
	if(status == -1) pin = NULL;
	return pin;
}

/*#define dbg 0
int UserInterface::initializePin(GPIOClass *pin, int pinNumber, string direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface::initializePin: " << pinNumber << ": " << direction << endl;
#endif
	int status = 0;

	//pin = new GPIOClass(pinNumber);

	char dirCharArray[4];
	strncpy(dirCharArray, direction.c_str(),4);
	if(pin->export_gpio() == -1) status = -1;
	if(pin->setdir_gpio(dirCharArray) == -1) status = -1;
	//if(pin->setval_gpio(pinNumber) == -1) status = -1;

#if(dbg >= 1)
	cout << "********** EXITING UserInterface::initializePin: " << status << endl;
#endif

	return status;
}*/


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
	int buttonPushedCount = 0;

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
	cout << "********** EXITING UserInterface2::readButtons: " << endl;
#endif

	return buttonPushed;
}


bool UserInterface2::isPowerButtonPushed()
{
	bool status = false;
	int buttonValue = 0;

	this->powerButton.getval_gpio(buttonValue);
	if(buttonValue == 0) status = true;

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
	//cout << rotEncA << ":" << rotEncB << endl;
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

	char sendCharArray[22];
	char lineCharArray[20];

	clearBuffer(sendCharArray,22);
	clearBuffer(lineCharArray,20);

	strncpy(lineCharArray,lineString.c_str(),20);
	sendCharArray[0] = (char)lineNumber;
	strncat(sendCharArray, lineCharArray, 21);
	for(int i = strlen(sendCharArray); i < 21; i++)
	{
		strcat(sendCharArray," ");
	}
	//strncat(sendCharArray,0,22);
	this->spi.sendData(sendCharArray,22);
	usleep(LCD_LINE_WRITE_SPACING);

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdLine: " << strlen(sendCharArray) << endl;
#endif
#if(dbg >= 2)
	cout << "********** " << sendCharArray << endl;
#endif
}


#define dbg 0
void UserInterface2::writeLcdLine(int lineNumber, char *lineString)
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::writeLcdLine: " << endl;
#endif

	char sendCharArray[50];

	clearBuffer(sendCharArray,50);

	sendCharArray[0] = (char)lineNumber;
	strncat(sendCharArray, lineString, 20);
	for(int i = strlen(sendCharArray); i < 21; i++)
	{
		strcat(sendCharArray," ");
	}
	this->spi.sendData(sendCharArray,22);
	usleep(LCD_LINE_WRITE_SPACING);

#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::writeLcdLine: " << strlen(sendCharArray) << endl;
#endif
#if(dbg >= 2)
	cout << "********** " << sendCharArray << endl;
#endif
}



#define dbg 0
void UserInterface2::powerOff()
{
#if(dbg >= 1)
	cout << "********** ENTERING UserInterface2::powerOff: " << endl;
#endif

	system("poweroff");
#if(dbg >= 1)
	cout << "********** EXITING UserInterface2::powerOff: " << endl;
#endif

}


