/*
 * Combo.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "Combo.h"

Combo::Combo() {
	// TODO Auto-generated constructor stub

}

Combo::~Combo() {
	// TODO Auto-generated destructor stub
}

#define dbg 1
int Combo::getCombo(string comboUiData)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getCombo: " << endl;
#endif
#if(dbg >= 2)
	cout << "********** : " << comboUiData << endl;
#endif

	int status = 0;

	if(this->effects.empty() == false)
	{
		this->effects.clear();
	}

	if(comboUiData.empty() == false)
	{

		try
		{
			this->jsonReader.parse(comboUiData, this->comboDataJson);
			this->name = this->comboDataJson["title"].asString();

			int effectCount = this->comboDataJson["effects"].size();
	#if(dbg >= 2)
		cout << this->comboDataJson.toStyledString() <<  endl;
	#endif

			for(int i = 0; i < effectCount; i++)
			{
				Effect effect = Effect(this->comboDataJson["effects"][i]);
				this->effects.push_back(effect);
			}
		}
		catch(std::exception &e)
		{
			cout << "error in Combo::getCombo: " << e.what() << endl;
			status = -1;
		}
	}

#if(dbg >= 1)
	cout << "********** EXITING Combo::getCombo: " << this->name <<  endl;
#endif


	return status;
}


string Combo::getName()
{
	return this->name;
}



vector<Effect> Combo::getEffectSoftKeyElements()
{
	return this->effects;
}

vector<Effect> Combo::getEffects()
{
	return this->effects;
}

Effect Combo::getEffect(int effectIndex)
{
	return this->effects[effectIndex];
}

/*void Combo::setEffect(Effect effect, int index)
{
	this->effects[index] = effect;
}*/

void Combo::updateParameter(int effectIndex, int parameterIndex, int direction)
{
	this->effects[effectIndex].updateParameter(parameterIndex, direction);
}

void Combo::addEffect(Json::Value effectDataJson)
{

	/*string name = effectDataJson["name"].asString();
	string abbr = effectDataJson["abbr"].asString();
	Json::Value paramDataArray = effectDataJson["params"];

	for(int i = 0; i < paramDataArray.size(); i++)
	{

	}
	this->effects.push_back()*/
}

/*string Combo::getComboName()
{
	return this->combo["title"].asString();
}


string Combo::getEffectName(int effectIndex)
{
	return this->combo["effects"][effectIndex]["name"].asString();
}


string Combo::getEffectAbbr(int effectIndex)
{
	return this->combo["effects"][effectIndex]["abbr"].asString();
}


string Combo::getParameterName(int effectIndex, int parameterIndex)
{
	return this->combo["effects"][effectIndex]["params"][parameterIndex]["name"].asString();
}


string Combo::getParameterAbbr(int effectIndex, int parameterIndex)
{
	return this->combo["effects"][effectIndex]["params"][parameterIndex]["abbr"].asString();
}

#define dbg 0
int Combo::getParameterValue(int effectIndex, int parameterIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getParameterValue: " << effectIndex << ":" << parameterIndex << endl;
#endif

	int parameterValueString = this->combo["effects"][effectIndex]["params"][parameterIndex]["value"].asInt();
#if(dbg >= 1)
	cout << "********** EXITING Combo::getParameterValue: " << parameterValueString << endl;
#endif

	return parameterValueString;
}

#define dbg 0
void Combo::updateParameterValue(int effectIndex, int parameterIndex, int direction)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::updateParameterValue: " << effectIndex << ":" << parameterIndex << endl;
#endif

	int parameterValue = this->combo["effects"][effectIndex]["params"][parameterIndex]["value"].asInt();

	if(direction == 1)  parameterValue++;
	else if(direction == 2)  parameterValue--;

	this->combo["effects"][effectIndex]["params"][parameterIndex]["value"] = parameterValue;

#if(dbg >= 1)
	cout << "********** EXITING Combo::updateParameterValue: " << direction << ":" << parameterValue << endl;
#endif
}

#define dbg 0
vector<SoftKeyElement> Combo::getEffectSoftKeyElements()
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getEffectSoftKeyElements: " << endl;
#endif
	vector<SoftKeyElement> effectElements;

	for(int effectIndex = 0; effectIndex < this->combo["effects"].size(); effectIndex++)
	{
		SoftKeyElement tempElement;
		tempElement.name = this->combo["effects"][effectIndex]["name"].asString();
		tempElement.abbr = this->combo["effects"][effectIndex]["abbr"].asString();
		tempElement.index = effectIndex;

		effectElements.push_back(tempElement);
	}
#if(dbg >= 1)
	cout << "********** EXITING Combo::getEffectSoftKeyElements: " << effectElements.size() << endl;
#endif

	return effectElements;
}

#define dbg 0
vector<SoftKeyElement> Combo::getParamSoftKeyElements(int effectIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getParamSoftKeyElements: " << endl;
#endif
	vector<SoftKeyElement> paramElements;

	for(int paramIndex = 0; paramIndex < this->combo["effects"][effectIndex]["params"].size(); paramIndex++)
	{
		SoftKeyElement tempElement;
		tempElement.name = this->combo["effects"][effectIndex]["params"][paramIndex]["name"].asString();
		tempElement.abbr = this->combo["effects"][effectIndex]["params"][paramIndex]["abbr"].asString();
		tempElement.index = paramIndex;

		paramElements.push_back(tempElement);
	}

#if(dbg >= 1)
	cout << "********** EXITING Combo::getParamSoftKeyElements: " << paramElements.size() << endl;
#endif

	return paramElements;
}*/
