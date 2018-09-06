/*
 * MainInterface.h
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef MAININTERFACE_H_
#define MAININTERFACE_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <json/json.h>
#include "Utilities.h"
#include "structs.h"

using namespace std;

#define RX_DATA_SIZE 1000
#define TX_DATA_SIZE 100

class MainInterface {
private:
	char rxData[RX_DATA_SIZE];
	int rxDataSize = 0;
	char txData[TX_DATA_SIZE];
	int txDataSize = 0;
	Json::Value comboUiDataJson;

public:
	MainInterface();
	virtual ~MainInterface();

	bool isDataReady();
	string getTestComboUiData();
	string sendUserRequestDataAndWaitForResponse(string userRequestData, int waitTime);
	int sendUserRequestData(string userRequestData);
	string getUserRequestResponse();

	vector<string>listCombos();
	string getComboUiData(string comboName);
	int changeComboParamValue(int paramIndex, int valueIndex);
	int saveCombo();

	string getFlxUtilityData();
	int changeFlxUtilityValue(string utilParamString, float utilValue);
	int saveFlxUtilityData();

	PedalStatus getCurrentStatus(string activeInterface);
	PedalStatus getCurrentStatus();
	PedalStatus parseCurrentStatusString(string statusString);
};

#endif /* MAININTERFACE_H_ */
