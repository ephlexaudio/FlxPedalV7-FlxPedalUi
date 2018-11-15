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
#include "Utilities.h"
#include "Effect.h"

namespace std
{




class Combo {
private:
	string name;
	vector<Effect> effects;
public:
	Combo();
	~Combo();


	int getCombo(string comboUiData);
	string getName();
	vector<string> getEffectSoftKeyAbbrs();
	vector<Effect> getEffects();
	Effect getEffect(int effectIndex);
	void updateParameter(int effectIndex, int parameterIndex, int direction);
};
}
#endif /* COMBO_H_ */
