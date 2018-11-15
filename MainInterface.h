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
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/mman.h>
#include <json/json.h>
#include "Utilities.h"
#include "structs.h"

namespace std
{




#define RX_DATA_SIZE 2000
#define TX_DATA_SIZE 1000



class MainInterface {
private:

	const char* rxFifoPath = "/home/pedalUiRx";
	const char* txFifoPath = "/home/pedalUiTx";
	int rxFifoFd;
	int txFifoFd;

	Json::Value comboUiDataJson;
	//******IPC stuff ******
	struct _ipcData {
		int change;
		string comboName;
		string currentStatus;
		bool usbPortOpen;
		bool hostGuiActive;
		int exit;
	};
	char fromMainIntName[50];
	char toMainIntName[50];
	int fromMainIntFD;
	int toMainIntFD;
	struct _ipcData *fromMainIntMemory;
	struct _ipcData *toMainIntMemory;
	int shmData[10];
	char commandString[100];
	int openFlxMainComm(void);
	int closeFlxMainComm(void);

public:
	MainInterface();
	~MainInterface();

	string sendUserRequestDataAndWaitForResponse(string userRequestData, int waitTime);
	int sendUserRequestData(string userRequestData);

	vector<string>listCombos();
	string getComboUiData(string comboName);
	int changeComboParamValue(ControlParameterPair controlParameterPair, int valueIndex);
	int saveCombo();

	string getFlxUtilityData();
	int changeFlxUtilityValue(string utilParamString, double utilValue);
	int changeFlxUtilityValue(string utilParamString, int utilValue);
	int changeFlxUtilityValue(string utilParamString, string utilParamOption);
	int saveFlxUtilityData();

	PedalStatus readFlxMain(void);

};
}
#endif /* MAININTERFACE_H_ */
