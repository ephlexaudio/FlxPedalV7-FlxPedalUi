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


#include <string>
#include <cstring>
#include <iostream>
#include "config.h"

#include "ComputeModulePinOuts2.h"
#include "structs.h"
#include "GPIOClass.h"
#include "Utilities.h"
#include "SPI.h"

#define LCD_LINE1_ADDR 0x00
#define LCD_LINE2_ADDR 0x40
#define LCD_LINE3_ADDR 0x14
#define LCD_LINE4_ADDR 0x54
#define SET_DDRAM_ADDRESS 0x80

using namespace std;



class UserInterface2 {
private:
	GPIOClass initializePin(GPIOClass pin, int pinNumber, string direction);
	int rotEncReadCount;
	//int initializePin(GPIOClass *pin, int pinNumber, string direction);
	void printPinData(GPIOClass pin);


public:
	UserInterface2();
	~UserInterface2();

	GPIOClass menuButtons[7];
	GPIOClass rotEncOut[2];
	GPIOClass rotEncRst;
	GPIOClass footswitchIn[2];
	GPIOClass footswitchOut[2];
	GPIOClass powerButton;
	//GPIOClass powerOffSignal;
	GPIOClass inputCoupling[3];
	//GPIOClass spiTest[3];
	SPI spi;

	int readButtons();
	bool isPowerButtonPushed();
	void resetEncoder();
	int readEncoder();

	void writeLcdLine(int lineNumber, string lineString);
	void writeLcdLine(int lineNumber, char *lineString);
	void powerOff();
	void spiBitBangTest();
	//void initializeLcd(void);
};

#endif /* USERINTERFACE_H_ */
