/*
 * FlxUtility.h
 *
 *  Created on: Dec 6, 2017
 *      Author: buildrooteclipse
 */

#ifndef FLXUTILITY_H_
#define FLXUTILITY_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <math.h>
#include <map>
#include <linux/types.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <json/json.h>
#include <algorithm>
#include "structs.h"
using namespace std;


class FlxUtility {
private:
	Json::Reader utilParamsReader;

	Json::Value utilParams;
	vector<UtilType> utilTypes;
	bool validateBoolDataUpdate(string value, bool defaultValue);
	void validateIntDataUpdate(string value, UtilIntValue &valueStruct);
	void validateDoubleDataUpdate(string value, UtilDoubleValue &valueStruct);
	void validateOptionDataUpdate(string value, UtilOption &optionStruct);

public:
	FlxUtility();
	~FlxUtility();

	// ************ Get lists or sizes *****************
	vector<string> getFlxUtilityTypeAbbrList();
	int getFlxUtilityTypeVectorSize();
	vector<string> getFlxUtilityParamAbbrList(int typeIndex);
	int getFlxUtilityParamVectorSize(int typeIndex);
	int getFlxUtilityParamOptionVectorSize(int typeIndex, int paramIndex);
	vector<string>getFlxUtilityParamOptionAbbrList(int typeIndex, int paramIndex);

	// ************ Get value,struct,etc ***************
	int getFlxUtilityParams(string paramString);
	UtilParam getFlxUtilityUtilParameter(int utilTypeIndex, int utilParamIndex);
	string getFlxUtilityTypeName(int typeIndex);
	string getFlxUtilityParamName(int typeIndex, int paramIndex);
	DblStr getFlxUtilityParamValue(int typeIndex, int paramIndex);
	int getFlxUtilityParamType(int typeIndex, int paramIndex);

	// ************ Set value,struct,etc **************
	UtilDoubleValue setParamValueDoubleStructLimits(double minimum, double maximum);
	UtilOption setParamValueOptionStructOptions(vector<string> validOptions);

	int changeFlxUtilityValue(int utilTypeIndex, int utilParamIndex, int genericIndex);

};

#endif /* FLXUTILITY_H_ */
