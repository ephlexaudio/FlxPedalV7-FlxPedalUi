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
		//******* Noise gate**************
		tempType.name = "noiseGate";
		tempType.abbr = "gate";
		tempParam.name = "openThres";
		tempParam.abbr = "oTrs";
		tempParam.utilParamIndex = 0;
		tempParam.paramType = 0;
		tempParam.doubleValue = this->setParamValueDoubleStructLimits(0.005, 2.00);
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "closeThres";
		tempParam.abbr = "cTrs";
		tempParam.utilParamIndex = 1;
		tempParam.paramType = 0;
		tempParam.doubleValue = this->setParamValueDoubleStructLimits(0.001, 0.1);
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "gain";
		tempParam.abbr = "gain";
		tempParam.utilParamIndex = 2;
		tempParam.paramType = 0;
		tempParam.doubleValue = this->setParamValueDoubleStructLimits(0.0001, 0.05);
		tempType.utilParams.push_back(tempParam);
		this->utilTypes.push_back(tempType);
		//********** Envelope generator trigger ******
		tempType.utilParams.clear();
		tempType.name = "trigger";
		tempType.abbr = "trgr";
		tempParam.name = "highThres";
		tempParam.abbr = "hTrs";
		tempParam.utilParamIndex = 3;
		tempParam.paramType = 0;
		tempParam.doubleValue = this->setParamValueDoubleStructLimits(0.01, 1.00);
		tempType.utilParams.push_back(tempParam);
		tempParam.name = "lowThres";
		tempParam.abbr = "lTrs";
		tempParam.utilParamIndex = 4;
		tempParam.paramType = 0;
		tempType.utilParams.push_back(tempParam);
		tempParam.doubleValue = this->setParamValueDoubleStructLimits(0.01, 1.00);
		this->utilTypes.push_back(tempType);
		//*********** Host PC (for USB configuration) ***********
		tempType.utilParams.clear();
		tempType.name = "hostPc";
		tempType.abbr = "host";
		tempParam.name = "os";
		tempParam.abbr = "os";
		tempParam.utilParamIndex = 5;
		tempParam.paramType = 1;
		tempParam.option = this->setParamValueOptionStructOptions(vector<string>{"Win","Mac","Lin"});
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


//********************** PRIVATE **************************************************

#define dbg 0
bool FlxUtility::validateBoolDataUpdate(string value, bool defaultValue)
{
	bool boolValue = defaultValue;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::validateBoolDataUpdate: " << value << endl;
#endif

	try
	{
		// change any upper case to lower case
		for(auto & c : value)
		{
			if(c<97) c = c + 32;
		}

		if(value.compare("true") == 0) boolValue = true;
		else if(value.compare("false") == 0) boolValue = false;
		else boolValue = true;

	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getValidatedUpdateBoolData: " << e.what() << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::validateBoolDataUpdate: " << value << endl;
#endif

	return boolValue;
}


void FlxUtility::validateIntDataUpdate(string value, UtilIntValue &valueStruct)
{

	try
	{
		int tempValue = strtol(value.c_str(),NULL,10);
		if(valueStruct.minimum <= tempValue && tempValue <= valueStruct.maximum)  valueStruct.value = tempValue;
	}

	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getValidatedUpdateUIntData: " << e.what() << endl;
	}

}


void FlxUtility::validateDoubleDataUpdate(string value, UtilDoubleValue &valueStruct)
{

	try
	{
		double tempValue = strtod(value.c_str(),NULL);
		if(valueStruct.minimum <= tempValue && tempValue <= valueStruct.maximum)  valueStruct.value = tempValue;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getValidatedUpdateDoubleData: " << e.what() << endl;
	}

}

void FlxUtility::validateOptionDataUpdate(string choice, UtilOption &optionStruct)
{

	try
	{
		for(auto & validOption : optionStruct.validOptions)
		{
			if(choice.compare(validOption) == 0)
			{
				optionStruct.option = choice;
			}
		}
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getValidatedUpdateDoubleData: " << e.what() << endl;
	}
}



//********************** PUBLIC **************************************************





// ************ Get lists or sizes *****************
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
vector<string> FlxUtility::getFlxUtilityParamAbbrList(int paramTypeIndex)
{
	vector<string> paramList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamAbbrList: " << endl;
#endif
	try
	{
		for(auto & utilParameter : this->utilTypes[paramTypeIndex].utilParams)
		{
			paramList.push_back(utilParameter.abbr);
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
	for(auto & param : paramList)
	{
		cout << param << ",";
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

vector<string> FlxUtility::getFlxUtilityParamOptionAbbrList(int typeIndex, int paramIndex)
{
	vector<string> optionAbbrList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamOptionAbbrList: " << typeIndex << "," << paramIndex << endl;
#endif
	try
	{
			optionAbbrList = this->utilTypes[typeIndex].utilParams[paramIndex].option.validOptions;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamOptionAbbrList error: " << e.what() << endl;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamOptionAbbrList: " << paramList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(auto & param : paramList)
	{
		cout << param << ",";
	}
	cout  << endl;
#endif

	return optionAbbrList;
}

int FlxUtility::getFlxUtilityParamOptionVectorSize(int typeIndex, int paramIndex)
{
	int size = 0;
	try
	{
		size = this->utilTypes[typeIndex].utilParams[paramIndex].option.validOptions.size();
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamOptionVectorSize error: " << e.what() << endl;
	}
	return size;

}


// ************ Get value,struct,etc ***************
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
		this->utilTypes[0].utilParams[0].doubleValue.value = this->utilParams["noiseGate"]["openThres"].asDouble();
		this->utilTypes[0].utilParams[1].doubleValue.value = this->utilParams["noiseGate"]["closeThres"].asDouble();
		this->utilTypes[0].utilParams[2].doubleValue.value = this->utilParams["noiseGate"]["gain"].asDouble();
		this->utilTypes[1].utilParams[0].doubleValue.value = this->utilParams["trigger"]["highThres"].asDouble();
		this->utilTypes[1].utilParams[1].doubleValue.value = this->utilParams["trigger"]["lowThres"].asDouble();
		this->utilTypes[2].utilParams[0].option.option = this->utilParams["hostPc"]["os"].asString();
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
vector<string> FlxUtility::getFlxUtilityTypeAbbrList()
{
	vector<string> paramTypeList;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityTypeAbbrList: " << endl;
#endif
	try
	{
		for(auto & utilType : this->utilTypes)
		{
			paramTypeList.push_back(utilType.abbr);
		}

	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityTypeAbbrList error: " << e.what() << endl;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityTypeAbbrList: " << paramTypeList.size() << endl;
#endif
#if(dbg >= 2)
	cout << "********** ";
	for(auto & paramType : paramTypeList)
	{
		cout << paramType << ",";
	}
	cout  << endl;
#endif
	return paramTypeList;
}

UtilParam FlxUtility::getFlxUtilityUtilParameter(int utilTypeIndex, int utilParamIndex)
{

		return this->utilTypes[utilTypeIndex].utilParams[utilParamIndex];
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


DblStr FlxUtility::getFlxUtilityParamValue(int typeIndex, int paramIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::getFlxUtilityParamValueDouble: " << typeIndex << "," << paramIndex << endl;
#endif
	DblStr value;
	try
	{
		value.value = 0.0;
		value.option.clear();

		if(this->utilTypes[typeIndex].utilParams[paramIndex].paramType == 0)
			value.value = this->utilTypes[typeIndex].utilParams[paramIndex].doubleValue.value;
		else if(this->utilTypes[typeIndex].utilParams[paramIndex].paramType == 1)
			value.option = this->utilTypes[typeIndex].utilParams[paramIndex].option.option;
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::getFlxUtilityParamValueDouble error: " << e.what() << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::getFlxUtilityParamValueDouble: " << paramValue << endl;
#endif

	return value;
}

int FlxUtility::getFlxUtilityParamType(int typeIndex, int paramIndex)
{
	return this->utilTypes[typeIndex].utilParams[paramIndex].paramType;
}


// ************ Set value,struct,etc **************

#define dbg 0
UtilDoubleValue FlxUtility::setParamValueDoubleStructLimits(double minimum, double maximum)
{
	UtilDoubleValue valueStruct;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::setParamValueDoubleStruct: " << endl;
#endif

	valueStruct.minimum = minimum;
	valueStruct.maximum = maximum;


	#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::setParamValueDoubleStruct: " << endl;
#endif

	return valueStruct;
}

#define dbg 0
UtilOption FlxUtility::setParamValueOptionStructOptions(vector<string> validOptions)
{
	UtilOption valueStruct;
#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::setParamValueOptionStruct: " << endl;
#endif

	valueStruct.validOptions = validOptions;

	#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::setParamValueOptionStruct: " << endl;
#endif

	return valueStruct;
}


// ************ Changes to utility *******************

#define dbg 2
int FlxUtility::changeFlxUtilityValue(int utilTypeIndex, int utilParamIndex, int genericIndex)
{
	int status = 0;

#if(dbg >= 1)
	cout << "********** ENTERING FlxUtility::changeFlxUtilityValue: ";
	cout << utilTypeIndex << ", " << utilParamIndex << ", " << genericIndex << endl;
#endif
	try
	{
		if(this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].paramType == 0)
		{
			int rotaryEnc = genericIndex;
			//****************** Check if change request is valid (in range, etc) ********************
			double utilValue = this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value;
			if(utilTypeIndex == 0) // noise gate
			{
				if((utilParamIndex == 0 && rotaryEnc < 0) || (utilParamIndex == 1 && rotaryEnc > 0))
				// utilParamIndex: 0=closeThres, 1=openThres
				{
					//************** Prevent noiseGate.closeThres from getting too close to noiseGate.openThres **************
					if((this->utilTypes[0].utilParams[1].doubleValue.value + 0.02) >= this->utilTypes[0].utilParams[0].doubleValue.value)
					{
	#if(dbg >= 2)
						cout << "noiseGate.closeThres too close to noiseGate.openThres" << this->utilTypes[0].utilParams[0].doubleValue.value;
						cout << ":" << this->utilTypes[0].utilParams[1].doubleValue.value << endl;
	#endif
						status = -1;
					}
				}
				else if(utilParamIndex == 2)
				{
					//************** Keep closed gate gain within range   **************
					UtilDoubleValue gain = this->utilTypes[0].utilParams[2].doubleValue;
					if((gain.value < gain.minimum  && rotaryEnc < 0 )||(rotaryEnc > 0 && gain.value > gain.maximum))
					{
	#if(dbg >= 2)
						cout << "closed gate gain out of valid range" << endl;
	#endif
						status = -1;
					}
				}
			}
			else if(utilTypeIndex == 1)
			{
				//************** Prevent trigger.lowThres from getting too close to trigger.highThres **************
				if((utilParamIndex == 0 && rotaryEnc < 0) || (utilParamIndex == 1 && rotaryEnc > 0))
				{
					if(this->utilTypes[1].utilParams[1].doubleValue.value  + 0.05 >= this->utilTypes[1].utilParams[0].doubleValue.value)
					{

	#if(dbg >= 2)
						cout << "trigger.lowThres too close to trigger.highThres" << this->utilTypes[1].utilParams[0].doubleValue.value;
						cout << ":" << this->utilTypes[1].utilParams[1].doubleValue.value  << endl;
	#endif
						status = -1;
					}
				}
			}

			UtilDoubleValue utilParam = this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue;
			if(utilParam.minimum > utilParam.value)
				{
				this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value = 1.1* utilParam.minimum;
				}
			else if(utilParam.maximum < utilParam.value)
			{
				this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value = 0.9* utilParam.maximum;
			}
			cout << utilParam.minimum << "<" << utilParam.value << "<" << utilParam.maximum << endl;
			//**************************** Make changes ***********************************
			if(status == 0)
			{
				if(1.0 < utilValue)
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.1*rotaryEnc;
				else if(0.1 < utilValue && utilValue <= 1.0)
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.01*rotaryEnc;
				else if(0.01 < utilValue && utilValue <= 0.1)
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.001*rotaryEnc;
				else if(0.001 < utilValue && utilValue <= 0.01)
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.0001*rotaryEnc;
				else if(0.0001 < utilValue && utilValue <= 0.001)
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.00001*rotaryEnc;
				else if(utilValue <= 0.0001 && rotaryEnc > 0)  // only allow increasing values at this level.
					this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].doubleValue.value += 0.00001*rotaryEnc;
			}
		}
		else if(this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].paramType == 1)
		{
			int utilParamOptionIndex = genericIndex;
			string utilOption;

			switch(utilParamOptionIndex)
			{
				case 1:
					utilOption = "Win";
					break;
				case 2:
					utilOption = "Mac";
					break;
				case 3:
					utilOption = "Lin";
					break;
				default:;
			}
			this->utilTypes[utilTypeIndex].utilParams[utilParamIndex].option.option = utilOption;
		}
	}
	catch(std::exception &e)
	{
		cout << "FlxUtility::changeFlxUtilityValue error: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	cout << "********** EXITING FlxUtility::changeFlxUtilityValue: ";
#endif

	return status;
}
