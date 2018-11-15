/*
 * Effect.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "Effect.h"

namespace std
{



#define dbg 0
Effect::Effect(Json::Value effectJsonData)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::Effect: " << endl;
#endif

	this->name = effectJsonData["name"].asString();
	this->abbr = effectJsonData["abbr"].asString();
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}
	this->parameters.clear();


	for(auto & paramJson : effectJsonData["params"])
	{
		Parameter control = Parameter(paramJson);
		this->parameters.push_back(control);
	}

#if(dbg >= 2)

	cout << "paramCount: " << this->parameters.size();
#endif
#if(dbg >= 1)
	cout << "********** EXITING Effect::Effect: "  << endl;
#endif
}

Effect::~Effect() {
	// TODO Auto-generated destructor stub
}

#define dbg 0
void Effect::addParameter(Json::Value parameterDataJson)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::addParameter: " << endl;
#endif
#if(dbg >= 2)
	cout << "parameter name: " << parameterDataJson["name"].asString() << endl;
#endif

	this->parameters.push_back(Parameter(parameterDataJson));
#if(dbg >= 1)
	cout << "********** EXITING Effect::addParameter: "  << endl;
#endif
}

string Effect::getAbbr()
{
	return this->abbr;
}

string Effect::getName()
{
	return this->name;
}

vector<string> Effect::getParamSoftKeyAbbrs()
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::getParamSoftKeyElements: " << startIndex << "," << this->parameters.size() << endl;
#endif
	vector<string> params;

	for(auto & parameter : this->parameters)
	{
		string param = parameter.getAbbr();
		params.push_back(param);
	}
#if(dbg >= 1)
	cout << "********** EXITING Effect::getParamSoftKeyElements: " << this->parameters.size() << endl;
#endif

	return params;
}

#define dbg 0
Parameter Effect::getParameter(unsigned int paramIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::setParameter: " << index << endl;
#endif
	Parameter param;

	if(paramIndex < this->parameters.size())
	{
		param = this->parameters[paramIndex];
	}
	else
	{
		param.setName("");
		param.setAbbr("");
	}
#if(dbg >= 1)
	cout << "********** EXITING Effect::setParameter: "  << endl;
#endif
	return param;
}

#define dbg 0
ControlParameterPair Effect::getControlParameterPair(unsigned int paramIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::getControlParameterPair: " << paramIndex << endl;
#endif
	ControlParameterPair controlParamPair;

	if(paramIndex < this->parameters.size())
	{
		controlParamPair = this->parameters[paramIndex].getControlParameterPair();
	}
	else
	{
		controlParamPair.parentControl = "";
		controlParamPair.parameter = "";
	}
#if(dbg >= 1)
	cout << "********** EXITING Effect::getControlParameterPair: " << endl;
#endif
#if(dbg >= 2)
	cout << controlParamPair.parentControl << ":" << controlParamPair.parameter << endl;
#endif

	return controlParamPair;
}


void Effect::updateParameter(unsigned int index, int direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::updateParameter: " << index << endl;
#endif

	this->parameters[index].updateValueIndex(direction);

#if(dbg >= 1)
	cout << "********** EXITING Effect::updateParameter: "  << endl;
#endif
}

#define dbg 1
int Effect::getParamCount()
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::getParamCount: " << endl;
#endif

#if(dbg >= 1)
	cout << "********** EXITING Effect::getParamCount: " << this->parameters.size()  << endl;
#endif
	return this->parameters.size();
}
}
