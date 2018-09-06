/*
 * Combo.h
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef COMBO_H_
#define COMBO_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <json/json.h>
#include "structs.h"

#include "Effect.h"

using namespace std;


class Combo {
private:
	Json::Reader jsonReader;
	Json::Value comboDataJson;
	string name;
	vector<Effect> effects;
public:
	Combo();
	virtual ~Combo();


	int getCombo(string comboUiData);
	string getName();
	void addEffect(Json::Value effectDataJson);
	vector<Effect> getEffectSoftKeyElements();
	vector<Effect> getEffects();
	Effect getEffect(int effectIndex);
	void updateParameter(int effectIndex, int parameterIndex, int direction);
};

#endif /* COMBO_H_ */
