/*
 * UserInterface.h
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <signal.h>
#include <sstream>

#include <string>
#include <cstring>
#include <iostream>

#include "Combo.h"

#include "ComputeModulePinOuts2.h"
#include "structs.h"
#include "GPIOClass.h"
#include "Utilities.h"
#include "SPI.h"
#include "config.h"

#define LCD_LINE1_ADDR 0x00
#define LCD_LINE2_ADDR 0x40
#define LCD_LINE3_ADDR 0x14
#define LCD_LINE4_ADDR 0x54
#define SET_DDRAM_ADDRESS 0x80

namespace std
{







class UserInterface2 {
private:
	GPIOClass initializePin(GPIOClass pin, int pinNumber, string direction);
	int rotEncReadCount;
	char menuLcdTempString[21];
	void printPinData(GPIOClass pin);
	string softkeyAlignedString(vector<string> softkeys);
	string softkeyAlignedString(string key1, string key2, string key3, string key4);
	string centeredString(string text, int totalStringLength);
	GPIOClass rotEncOut[2];
	GPIOClass rotEncRst;
	GPIOClass footswitchIn[2];
	GPIOClass footswitchOut[2];
	GPIOClass menuButtons[7];
	GPIOClass powerButton;
	GPIOClass inputCoupling[3];
	SPI spi;

public:
	UserInterface2();
	~UserInterface2();


	int readButtons();
	bool isPowerButtonPushed();
	void resetEncoder();
	int readEncoder();

	void writeLcdLine(int lineNumber, string lineString);

	void writeLcdHeader(int menuLevel, string title, string subtitle);
	void writeLcdFxParameter(Parameter parameter);
	void writeLcdUtilParameter(UtilParam parameter);
	void writeLcd(string line1, string line2, string line3, string line4);
	void writeLcdWithSoftKeys(string line1, string line2, string line3, vector<string> softKeyAbbrs);
	void writeLcdCentered(string line1, string line2, string line3, string line4);
	void writeSoftKeys(int startIndex, vector<string> softKeyAbbrs);

};
}
#endif /* USERINTERFACE_H_ */
