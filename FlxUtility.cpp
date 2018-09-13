/*
 * FlxUtility.cpp
 *
 *  Created on: Dec 6, 2017
 *      Author: buildrooteclipse
 */

#include "FlxUtility.h"

FlxUtility::FlxUtility() {
	// TODO Auto-generated constructor stub
	UtilType tempType;
	UtilParam tempParam;
  // Create utility menu
	try
	{
		tempType.name = "noiseGate";
		tempType.abbr = "gate";
		tempParam.name = "openThres";
		tempParam.abbr = "oTrs";
		tempParam.utilParamIndex = 0;
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "closeThres";
		tempParam.abbr = "cTrs";
		tempParam.utilParamIndex = 1;
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "gain";
		tempParam.abbr = "gain";
		tempParam.utilParamIndex = 2;
		tempType.utilParams.push_back(tempParam);
		this->utilTypes.push_back(tempType);

		tempType.utilParams.clear();
		tempType.name = "trigger";
		tempType.abbr = "trgr";
		tempParam.name = "highThres";
		tempParam.abbr = "hTrs";
		tempParam.utilParamIndex = 3;
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "lowThres";
		tempParam.abbr = "lTrs";
		tempParam.utilParamIndex = 4;
		tempType.utilParams.push_back(tempParam);
		this->utilTypes.push_back(tempType);

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::FlxUtility error: " << e.what() << endl;
	}

}

FlxUtility::~FlxUtility() {
	// TODO Auto-generated destructor stub
}

#define dbg 0
int FlxUtility::getFlxUtilityParams(string paramString)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParams: " << paramString << endl;
#endif

	try
	{
		this->utilParamsReader.parse(paramString,this->utilParams);
		this->utilTypes[0].utilParams[0].value = this->utilParams["noiseGate"]["openThres"].asFloat();
		this->utilTypes[0].utilParams[1].value = this->utilParams["noiseGate"]["closeThres"].asFloat();
		this->utilTypes[0].utilParams[2].value = this->utilParams["noiseGate"]["gain"].asFloat();
		this->utilTypes[1].utilParams[0].value = this->utilParams["trigger"]["highThres"].asFloat();
		this->utilTypes[1].utilParams[1].value = this->utilParams["trigger"]["lowThres"].asFloat();
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParams error: " << e.what() << endl;
	}




#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParams: " << endl;
#endif

	return status;
}

#define dbg 0
int FlxUtility::changeFlxUtilityValue(int utilTypeIndex, int utilParamIndex, int rotaryEnc)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::changeFlxUtilityValue: ";
	cout << utilTypeIndex << ", " << utilParamIndex << ", " << rotaryEnc << endl;
#endif
	try
	{
		float utilValue = this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value;

		if(1.0 < fabsf(utilValue))
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.1*rotaryEnc;

		}
		else if(0.1 < fabsf(utilValue) && fabsf(utilValue) <= 1.0)
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.01*rotaryEnc;

		}
		else if(0.01 < fabsf(utilValue) && fabsf(utilValue) <= 0.1)
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.001*rotaryEnc;

		}
		else if(0.001 < fabsf(utilValue) && fabsf(utilValue) <= 0.01)
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.0001*rotaryEnc;
		}
		else if(0.0001 < fabsf(utilValue) && fabsf(utilValue) <= 0.001)
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.00001*rotaryEnc;
		}
		else if(fabsf(utilValue) <= 0.0001)
		{
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value += 0.000001*rotaryEnc;
		}
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::changeFlxUtilityValue error: " << e.what() << endl;
	}



#if(dbg >= 2)
	cout << "utilValue: " << utilValue << endl;
	cout << "fabsf(utilValue): " << fabsf(utilValue) << endl;
#endif


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::changeFlxUtilityValue: ";
	cout << this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].value << endl;
#endif

	return status;
}

#define dbg 0
vector<string> FlxUtility::getFlxUtilityTypeNameList()
{
	vector<string> paramTypeList;

#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeNameList: " << endl;
#endif
	try
	{
		for(int i = 0; i < this->utilTypes.size(); i++)
		{
			paramTypeList.push_back(this->utilTypes[i].name);
		}

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeNameList error: " << e.what() << endl;
	}

#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeNameList: " << paramTypeList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(int i = 0; i < paramTypeList.size(); i++)
	{
		cout << paramTypeList[i] << ",";
	}
	cout  << endl;
#endif

	return paramTypeList;
}

#define dbg 0
vector<string> FlxUtility::getFlxUtilityTypeAbbrList()
{
	vector<string> paramTypeList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeNameList: " << endl;
#endif
	try
	{
		for(int i = 0; i < this->utilTypes.size(); i++)
		{
			paramTypeList.push_back(this->utilTypes[i].abbr);
		}

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeAbbrList error: " << e.what() << endl;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeNameList: " << paramTypeList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(int i = 0; i < paramTypeList.size(); i++)
	{
		cout << paramTypeList[i] << ",";
	}
	cout  << endl;
#endif
	return paramTypeList;
}

int FlxUtility::getFlxUtilityTypeVectorSize()
{
	int size = 0;
	try
	{
		size = this->utilTypes.size();
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeVectorSize error: " << e.what() << endl;
	}

	return size;
}



#define dbg 0
vector<string> FlxUtility::getFlxUtilityParamNameList(int paramTypeIndex)
{
	vector<string> paramList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeNameList: " << endl;
#endif
	try
	{
		for(int i = 0; i < this->utilTypes[paramTypeIndex].utilParams.size(); i++)
		{
			paramList.push_back(this->utilTypes[paramTypeIndex].utilParams[i].name);
		}

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamNameList error: " << e.what() << endl;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeNameList: " << paramList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(int i = 0; i < paramList.size(); i++)
	{
		cout << paramList[i] << ",";
	}
	cout  << endl;
#endif
	return paramList;
}

#define dbg 0
vector<string> FlxUtility::getFlxUtilityParamAbbrList(int paramTypeIndex)
{
	vector<string> paramList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamAbbrList: " << endl;
#endif
	try
	{
		for(int i = 0; i < this->utilTypes[paramTypeIndex].utilParams.size(); i++)
		{
			paramList.push_back(this->utilTypes[paramTypeIndex].utilParams[i].abbr);
		}

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamAbbrList error: " << e.what() << endl;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamAbbrList: " << paramList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(int i = 0; i < paramList.size(); i++)
	{
		cout << paramList[i] << ",";
	}
	cout  << endl;
#endif
	return paramList;
}

int FlxUtility::getFlxUtilityParamVectorSize(int typeIndex)
{
	int size = 0;
	try
	{
		size = this->utilTypes[typeIndex].utilParams.size();
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamVectorSize error: " << e.what() << endl;
	}
	return size;
}


#define dbg 0
string FlxUtility::getFlxUtilityTypeName(int typeIndex)
{
	string typeName;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeName: " << typeIndex << endl;
#endif
	try
	{
		typeName = this->utilTypes[typeIndex].name;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeName error: " << e.what() << endl;
	}



#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeName: " << typeName << endl;
#endif
	return typeName;
}

#define dbg 0
string FlxUtility::getFlxUtilityTypeAbbr(int typeIndex)
{
	string typeAbbr;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeAbbr: " << typeIndex << endl;
#endif
	try
	{
		typeAbbr = this->utilTypes[typeIndex].abbr;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeAbbr error: " << e.what() << endl;
	}



#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeAbbr: " << typeAbbr << endl;
#endif
	return typeAbbr;
}

#define dbg 0
string FlxUtility::getFlxUtilityParamName(int typeIndex, int paramIndex)
{
	string paramName;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamName: " << typeIndex << "," << paramIndex << endl;
#endif
	try
	{
		paramName = this->utilTypes[typeIndex].utilParams[paramIndex].name;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamName error: " << e.what() << endl;
	}



#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamName: " << paramName << endl;
#endif
	return paramName;
}

#define dbg 0
string FlxUtility::getFlxUtilityParamAbbr(int typeIndex, int paramIndex)
{
	string paramAbbr;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamAbbr: " << typeIndex << "," << paramIndex << endl;
#endif
	try
	{
		paramAbbr = this->utilTypes[typeIndex].utilParams[paramIndex].abbr;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamAbbr error: " << e.what() << endl;
	}



#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamAbbr: " << paramAbbr << endl;
#endif
	return paramAbbr;
}

#define dbg 0
float FlxUtility::getFlxUtilityParamValue(int typeIndex, int paramIndex)
{
	float paramValue;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamValue: " << typeIndex << "," << paramIndex << endl;
#endif
	try
	{
		paramValue = this->utilTypes[typeIndex].utilParams[paramIndex].value;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamValue error: " << e.what() << endl;
	}

	paramValue = this->utilTypes[typeIndex].utilParams[paramIndex].value;

#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamValue: " << paramValue << endl;
#endif
	return paramValue;
}

int FlxUtility::saveFlxUtilityParams()
{
	int status = 0;



	return status;
}
