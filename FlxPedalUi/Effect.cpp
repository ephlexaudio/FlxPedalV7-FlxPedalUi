/*
 * Effect.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "Effect.h"

#include "Parameter.h"

Effect::Effect(string name, string abbr) {
	// TODO Auto-generated constructor stub
	this->name = name;
	this->abbr = abbr;
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}
	this->parameters.clear();

}

Effect::Effect(Json::Value effectJsonData)
{
	this->name = effectJsonData["name"].asString();
	this->abbr = effectJsonData["abbr"].asString();
	while(this->abbr.size()<4)
	{
		this->abbr += " ";
	}
	this->parameters.clear();

	int paramCount = effectJsonData["params"].size();
	for(int i = 0; i < paramCount; i++)
	{
		Parameter param = Parameter(effectJsonData["params"][i]);
		this->parameters.push_back(param);
	}
}

Effect::~Effect() {
	// TODO Auto-generated destructor stub
}

void Effect::addParameter(Json::Value paramDataJson)
{
	/*string name = paramDataJson["name"].asString();
	string abbr = paramDataJson["abbr"].asString();
	int valueIndex = paramDataJson["value"].asInt();
	int type = paramDataJson["type"].asInt();*/

	this->parameters.push_back(Parameter(paramDataJson));
}

string Effect::getName()
{
	return this->name;
}

string Effect::getAbbr()
{
	return this->abbr;
}

vector<Parameter> Effect::getParamSoftKeyElements(int startIndex)
{
	vector<Parameter> params;
	/*int paramCount = this->parameters.size()-4*startIndex;

	if(paramCount > 4) paramCount = 4;
	else if(paramCount < 0) paramCount = this->parameters.size();*/

	if(this->parameters.size() > startIndex)
	{
		for(int i = startIndex;  (i < this->parameters.size()) && (i < startIndex+4); i++)
		{
			Parameter param = this->parameters[i];
			params.push_back(param);
		}
	}

	return params;
}

#define dbg 0
vector<Parameter> Effect::getParams(int startIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::getParams: " << startIndex << "," << this->parameters.size() << endl;
#endif


	vector<Parameter> params;
	/*int paramCount = this->parameters.size()-4*startIndex;

	if(paramCount > 4) paramCount = 4;
	else if(paramCount < 0) paramCount = this->parameters.size();*/

	if(this->parameters.size() > startIndex)
	{
		for(int i = startIndex;  (i < this->parameters.size()) && (i < startIndex+4); i++)
		{
			Parameter param = this->parameters[i];
			params.push_back(this->parameters[i]);
#if(dbg >= 2)
			cout << "getting parameter: " << i << endl;
			cout << param.getName() << endl;
#endif
		}
	}

#if(dbg >= 1)
	cout << "********** EXITING Effect::getParams: " << this->parameters.size() << endl;
#endif

	return params;
}

#define dbg 0
Parameter Effect::getParam(int paramIndex)
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


void Effect::updateParameter(int index, int direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING Effect::setParameter: " << index << endl;
#endif

	this->parameters[index].updateValueIndex(direction); //.setValueIndex(value);

#if(dbg >= 1)
	cout << "********** EXITING Effect::setParameter: "  << endl;
#endif
}

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
/*int Effect::getParameterValueIndex(int parameterIndex)
{

}*/

