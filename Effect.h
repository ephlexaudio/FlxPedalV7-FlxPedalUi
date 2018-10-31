/*
 * Effect.h
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef EFFECT_H_
#define EFFECT_H_

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

#include "Parameter.h"

using namespace std;

class Effect {
private:
	string name;
	string abbr;
	Json::Reader jsonReader;
	Json::Value effectDataJson;
	vector<Parameter> parameters;
public:
	Effect(Json::Value effectJsonData);
	~Effect();

	void addParameter(Json::Value parameterDataJson);
	string getAbbr();
	string getName();
	vector<string> getParamSoftKeyAbbrs();
	Parameter getParameter(int paramIndex);
	ControlParameterPair getControlParameterPair(int paramIndex);
	void updateParameter(int index, int direction);
	int getParamCount();


};

#endif /* EFFECT_H_ */
