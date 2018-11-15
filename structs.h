/*
 * structs.h
 *
 *  Created on: Jul 14, 2017
 *      Author: buildrooteclipse
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

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

namespace std
{




struct ControlParameterPair
{
	string parentControl;
	string parameter;
};
struct SoftKeyElement{
	string name;
	string abbr;
	int index;
};

struct PedalStatus{
	string comboName;
	string currentStatus;
	bool usbPortOpen;
	bool hostGuiActive;
};

struct DblStr {
	double value;
	string option;
};


struct UtilDoubleValue {
	double value;
	double minimum;
	double maximum;
};

struct UtilIntValue {
	int value;
	int minimum;
	int maximum;
};

struct UtilOption {
	string option;
	vector<string> validOptions;
};



struct UtilParam{
	string name;
	string abbr;
	int utilParamIndex;
	int paramType; //0=Double, 1=String Option
	UtilOption	option;
	UtilIntValue intValue;
	UtilDoubleValue doubleValue;
};

struct UtilType{
	string name;
	string abbr;
	vector<UtilParam> utilParams;
};
}
#endif /* STRUCTS_H_ */
