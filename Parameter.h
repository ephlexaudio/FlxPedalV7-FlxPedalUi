/*
 * Parameter.h
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

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

#include <json/json.h>
#include "structs.h"

namespace std
{




class Parameter {
private:
	string name;
	string abbr;
	int paramType;
	int paramIndex;
	string parentControl;
	int parentControlIndex;
	int valueIndex;
public:
	Parameter();
	Parameter(string name, string abbr, int valueIndex, int type,int parentControlIndex);
	Parameter(Json::Value paramDataJson);
	~Parameter();

	void setName(string name);
	void setAbbr(string abbr);
	string getName();
	string getAbbr();
	ControlParameterPair getControlParameterPair();
	int getValueType();
	int getValueIndex();
	void updateValueIndex(int direction);

};
}
#endif /* PARAMETER_H_ */
