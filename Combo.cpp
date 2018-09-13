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
