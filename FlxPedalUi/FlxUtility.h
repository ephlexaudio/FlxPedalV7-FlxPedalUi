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

using namespace std;

#define UTIL_PARAM_COUNT 5

const float utilAmp[100] = {0.01,0.02,0.04,0.06,0.08,0.1,0.12,0.14,0.16,0.18,0.2,0.22,0.24,0.26,0.28,0.3,0.32,0.34,0.36,0.38,0.4,0.42,0.44,0.46,0.48,0.5,0.52,0.54,0.56,0.58,0.6,0.62,0.64,0.66,0.68,0.7,0.72,0.74,0.76,0.78,0.8,0.82,0.84,0.86,0.88,0.9,0.92,0.94,0.96,0.98,1.0,1.11,1.22,1.33,1.44,1.55,1.66,1.77,1.88,1.99,2.1,2.21,2.32,2.43,2.54,2.65,2.76,2.87,2.98,3.09,3.2,3.417,3.644,3.871,4.098,4.325,4.552,4.779,5.006,5.233,5.46,5.687,5.914,6.141,6.368,6.595,6.822,7.049,7.276,7.503,7.73,7.957,8.184,8.411,8.638,8.865,9.092,9.319,9.546,9.773};

struct UtilParam{
	string name;
	string abbr;
	int utilParamIndex;
	float value;
};

struct UtilType{
	string name;
	string abbr;
	//UtilParam utilParams[UTIL_PARAM_COUNT];
	vector<UtilParam> utilParams;
};

/*typedef map<string, UtilParam> ParamMap;
typedef map<string, ParamMap> TypeMap;*/

class FlxUtility {
private:
	string utilParamsString;
	Json::Reader utilParamsReader;

	Json::Value utilParams;

	//std::map<string,std::map<string,UtilParam>> utilTypeMap;

	//UtilParamType utilParamTypes[2];
	vector<UtilType> utilTypes;

	//int getParamIndex(string paramType, string paramName);
public:
	FlxUtility();
	virtual ~FlxUtility();

	int getFlxUtilityParams(string paramString);
	int changeFlxUtilityValue(int utilTypeIndex, int utilParamIndex, int rotaryEnc);
	vector<string> getFlxUtilityTypeNameList();
	vector<string> getFlxUtilityTypeAbbrList();
	int getFlxUtilityTypeVectorSize();
	vector<string> getFlxUtilityParamNameList(int typeIndex);
	vector<string> getFlxUtilityParamAbbrList(int typeIndex);
	int getFlxUtilityParamVectorSize(int typeIndex);
	string getFlxUtilityTypeName(int typeIndex);
	string getFlxUtilityTypeAbbr(int typeIndex);
	string getFlxUtilityParamName(int typeIndex, int paramIndex);
	string getFlxUtilityParamAbbr(int typeIndex, int paramIndex);
	float getFlxUtilityParamValue(int typeIndex, int paramIndex);
	int saveFlxUtilityParams();
};

#endif /* FLXUTILITY_H_ */
