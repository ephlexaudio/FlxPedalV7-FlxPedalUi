/*
 * Parameter.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "Parameter.h"

Parameter::Parameter(){

}

Parameter::Parameter(string name, string abbr, int valueIndex, int type) {
	// TODO Auto-generated constructor stub
	this->name = name;
	this->abbr = abbr;
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}
	this->valueIndex = valueIndex;
	this->paramType = type;
}

Parameter::Parameter(Json::Value paramDataJson)
{
	this->name = paramDataJson["name"].asString();
	this->abbr = paramDataJson["abbr"].asString();
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}

	this->valueIndex = paramDataJson["value"].asInt();
	this->paramType = paramDataJson["paramType"].asInt();
	this->paramIndex = paramDataJson["index"].asInt();
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

int Parameter::getValueIndex()
{
	return this->valueIndex;
}

int Parameter::getParamIndex()
{
	return this->paramIndex;
}

void Parameter::setValueIndex(int value)
{
	this->valueIndex = value;
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

