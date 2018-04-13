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

using namespace std;

class Parameter {
private:
	string name;
	string abbr;
	int valueIndex;
	int paramType;
	int paramIndex;
public:
	Parameter();
	Parameter(string name, string abbr, int valueIndex, int type);
	Parameter(Json::Value paramDataJson);
	virtual ~Parameter();

	void setName(string name);
	void setAbbr(string abbr);
	string getName();
	string getAbbr();
	int getValueType();
	int getValueIndex();
	int getParamIndex();
	void setValueIndex(int value);
	void updateValueIndex(int direction);

};

#endif /* PARAMETER_H_ */
