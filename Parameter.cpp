/*
 * Parameter.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "Parameter.h"

namespace std
{



Parameter::Parameter(){
	this->name = "";
	this->abbr = "";
	this->valueIndex = 0;
	this->paramType = 0;
	this->paramIndex = 0;
	this->parentControlIndex = 0;
}

Parameter::Parameter(string name, string abbr, int valueIndex, int type, int parentControlIndex) {
	// TODO Auto-generated constructor stub
	this->name = name;
	this->abbr = abbr;
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}
	this->valueIndex = valueIndex;
	this->paramType = type;
	this->paramIndex = 0;
	this->parentControlIndex = parentControlIndex;
}

#define dbg 0
Parameter::Parameter(Json::Value paramDataJson)
{
#if(dbg >= 1)
	cout << "********** ENTERING Parameter::Parameter: " << endl;
#endif
	this->name = paramDataJson["name"].asString();
	this->abbr = paramDataJson["abbr"].asString();
	while(this->abbr.size()<4)  // make abbr 4 characters to make all abbrs align for LCD softkeys.
	{
		this->abbr += " ";
	}
	this->parentControl = paramDataJson["parentControl"].asString();
	this->valueIndex = paramDataJson["value"].asInt();
	this->paramType = paramDataJson["paramType"].asInt();
	this->parentControlIndex = paramDataJson["ctrlIndx"].asInt();
	this->paramIndex = paramDataJson["index"].asInt();
#if(dbg >= 1)
	cout << "********** EXITING Parameter::Parameter: "  << endl;
#endif
}

Parameter::~Parameter() {
	// TODO Auto-generated destructor stub
}

void Parameter::setName(string name)
{
	this->name = name;
}

void Parameter::setAbbr(string abbr)
{
	this->abbr = abbr;
}

int Parameter::getValueType()
{
	return this->paramType;
}
string Parameter::getName()
{
	return this->name;
}

string Parameter::getAbbr()
{
	return this->abbr;
}


ControlParameterPair Parameter::getControlParameterPair()
{

	ControlParameterPair  controlParamPair;

	controlParamPair.parentControl = this->parentControl;
	controlParamPair.parameter = this->name;

	return controlParamPair;
}

int Parameter::getValueIndex()
{
	return this->valueIndex;
}


#define dbg 0
void Parameter::updateValueIndex(int direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING Parameter::updateValueIndex: " << direction << endl;
#endif

	int testValue = this->valueIndex + direction;
	if(0 <= testValue && testValue < 100)
	{
		this->valueIndex += direction;
	}


#if(dbg >= 1)
	cout << "********** EXITING Parameter::updateValueIndex: " << this->valueIndex << endl;
#endif

}
}
