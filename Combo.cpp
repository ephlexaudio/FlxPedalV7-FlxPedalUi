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

#define dbg 0
int Combo::getCombo(string comboUiData)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getCombo: " << endl;
#endif
#if(dbg >= 3)
	cout << "********** : " << comboUiData << endl;
#endif

	int status = 0;
	int effectCount;

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

			effectCount = this->comboDataJson["effects"].size();

	#if(dbg >= 3)
		cout << this->comboDataJson.toStyledString() <<  endl;
	#endif

			for(auto & effectJson : this->comboDataJson["effects"])
			{
				Effect effect = Effect(effectJson);
				this->effects.push_back(effect);
			}
		}
		catch(std::exception &e)
		{
			cout << "error in Combo::getCombo: " << e.what() << endl;
			status = -1;
		}
	}
#if(dbg >= 2)
	cout << "effectCount: " << effectCount << endl;
#endif
#if(dbg >= 1)
	cout << "********** EXITING Combo::getCombo: " << this->name << "\t\t status:" << status <<  endl;
#endif


	return status;
}


string Combo::getName()
{
	return this->name;
}


vector<string> Combo::getEffectSoftKeyAbbrs()
{
	vector<string> softkeys;

	for(auto & effect : this->effects)
	{
		softkeys.push_back(effect.getAbbr());
	}

	return softkeys;
}

vector<Effect> Combo::getEffects()
{
	return this->effects;
}

#define dbg 0
Effect Combo::getEffect(int effectIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING Combo::getEffect: " << endl;
#endif

#if(dbg >= 1)
	cout << "********** EXITING Combo::getEffect: " << effectIndex <<  endl;
#endif

	return this->effects[effectIndex];
}


void Combo::updateParameter(int effectIndex, int parameterIndex, int direction)
{
	try
	{
		this->effects[effectIndex].updateParameter(parameterIndex, direction);
	}
	catch(std::exception &e)
	{
		cout << "Combo::updateParameter error: " << e.what() << endl;
	}
}
