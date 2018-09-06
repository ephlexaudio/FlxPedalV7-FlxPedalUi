/*
 * MainInterface.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "MainInterface.h"

using namespace std;

int rxFifoFd;
int txFifoFd;


const char* rxFifoPath = "/home/pedalUiRx";
const char* txFifoPath = "/home/pedalUiTx";
#define RX_DATA_SIZE 1000 // OfxUi will be recieving larger ui data in JSON format
#define TX_DATA_SIZE 100 // largest data to send out will be combo list

#define dbg 2
MainInterface::MainInterface() {
	// TODO Auto-generated constructor stub
	rxFifoFd = -1;
	txFifoFd = -1;
	errno = 0;
	if(mkfifo(rxFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		cout << "mkfifo rxFifoPath errno: " << errno << endl;
	}
	errno = 0;
	if(mkfifo(txFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		cout << "mkfifo txFifoPath errno: " << errno << endl;
	}

	int rxFifoFdErr,txFifoFdErr = 0;
	for(int i = 0; (rxFifoFd == -1 || txFifoFd == -1) && i < 20; i++)
	{
		errno = 0;
		rxFifoFd = open(rxFifoPath, O_RDWR | O_NONBLOCK);
		rxFifoFdErr = errno;
		usleep(10000);
		errno = 0;
		txFifoFd = open(txFifoPath, O_RDWR | O_NONBLOCK);
		txFifoFdErr = errno;
		usleep(10000);
#if(dbg >= 2)
		cout << "waiting for FIFOs" << endl;
		cout << "OFX_PEDAL_UI_RPI: " << strerror(rxFifoFdErr) << ":" << strerror(txFifoFdErr) << endl;
		cout << "FIFO FDs: " << rxFifoFd << ":" << txFifoFd << endl;
#endif
	}
#if(dbg >= 2)
	cout << "FIFOs opened." << endl;
#endif
}

MainInterface::~MainInterface() {
	// TODO Auto-generated destructor stub

	int ret;

	if(txFifoFd >= 0)
	{
		 cout << "closing and removing /home/pedalUiTx." << endl;
		close(txFifoFd);
		system("rm /home/pedalUiTx");
	}
	else
	{
		 cout << "/home/pedalUiTx not found." << endl;
	}

	if(rxFifoFd >= 0)
	{
		 cout << "closing and removing /home/pedalUiRx." << endl;
		close(rxFifoFd);
		system("rm /home/pedalUiRx");
	}
	else
	{
		 cout << "/home/pedalUiRx not found." << endl;
	}




}

bool MainInterface::isDataReady()
{
	bool status = false;



	return status;
}

#define dbg 0
string MainInterface::getTestComboUiData()
{
	string uiDataString = "";

	Json::Value uiDataJson;
	uiDataJson = Json::Value();
	char tempString[100];
	// create JSON structure first
	uiDataJson["title"] = "combo";
	for(int effectIndex = 0; effectIndex < 2; effectIndex++)
	{
		snprintf(tempString,15,"effect%d", effectIndex);
		uiDataJson["effects"][effectIndex]["name"] = string(tempString);
		snprintf(tempString,15,"fx%d", effectIndex);
		uiDataJson["effects"][effectIndex]["abbr"] = string(tempString);

		for(int parameterIndex = 0; parameterIndex < 6; parameterIndex++)
		{
			snprintf(tempString,15,"parameter%d", parameterIndex);
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["name"] = string(tempString);
			snprintf(tempString,15,"prm%d", parameterIndex);
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["abbr"] = string(tempString);
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["value"] = 10;
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["type"] = 0;
		}
	}

	// create String from JSON structure

	snprintf(tempString, 100, "{\"title\": \"%s\", \"effects\":[",  uiDataJson["title"].asString().c_str());
	uiDataString += string(tempString);
	for(int effectIndex = 0; effectIndex < 2; effectIndex++)
	{
		snprintf(tempString,100,"{\"name\":\"%s\",\"abbr\":\"%s\",\"params\":[",
		uiDataJson["effects"][effectIndex]["name"].asString().c_str(),
		uiDataJson["effects"][effectIndex]["abbr"].asString().c_str());
		uiDataString += string(tempString);
		for(int parameterIndex = 0; parameterIndex < 6; parameterIndex++)
		{
			snprintf(tempString,100,"{\"name\":\"%s\",\"abbr\":\"%s\",\"value\":%d,\"type\":%d}",
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["name"].asString().c_str(),
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["abbr"].asString().c_str(),
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["value"].asInt(),
			uiDataJson["effects"][effectIndex]["params"][parameterIndex]["type"].asInt());
			uiDataString += string(tempString);
			if(parameterIndex < 5) uiDataString += string(",");
		}
		uiDataString += "]}";
		if(effectIndex < 1) uiDataString += string(",");

	}
	uiDataString += "]}";
#if(dbg >= 2)
	cout << uiDataJson.toStyledString() << endl;

	cout << uiDataString << endl;
#endif
	return uiDataString;
}


#define dbg 1
string MainInterface::sendUserRequestDataAndWaitForResponse(string userRequestData, int waitTime)
{
	int lengthWritten = 0;
	string responseData;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::sendUserRequestDataAndWaitForResponse: " << userRequestData << endl;
#endif
	char userRequestDataCharArray[100];
	strncpy(userRequestDataCharArray, userRequestData.c_str(),100);
#if(dbg >= 2)
	for(int i = 0; i < 100 && userRequestDataCharArray[i] != 0; i++)
	{
		cout << userRequestDataCharArray[i] << "(" << (int)userRequestDataCharArray[i] << "), ";
	}
	cout << endl;
#endif

	clearBuffer(this->rxData,RX_DATA_SIZE);

	lengthWritten = write(txFifoFd, userRequestDataCharArray,100);

	if(lengthWritten > 0)
	{
		usleep(waitTime*1000);
		while((this->rxDataSize = read(rxFifoFd,this->rxData,RX_DATA_SIZE)) > 1)
		{
			responseData += string(this->rxData);
		}
	}

	clearBuffer(this->txData,TX_DATA_SIZE);

	clearBuffer(this->rxData,RX_DATA_SIZE);

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::sendUserRequestDataAndWaitForResponse: " << responseData << endl;
#endif
	return responseData;
}

#define dbg 0
int MainInterface::sendUserRequestData(string userRequestData)
{
	int lengthWritten = 0;
	int status = -1;
	string responseData;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::sendUserRequestData: " << userRequestData << endl;
#endif
	char userRequestDataCharArray[100];
	strncpy(userRequestDataCharArray, userRequestData.c_str(),100);
#if(dbg >= 2)
	for(int i = 0; i < 100 && userRequestDataCharArray[i] != 0; i++)
	{
		cout << userRequestDataCharArray[i] << "(" << (int)userRequestDataCharArray[i] << "), ";
	}
	cout << endl;
#endif

	if((lengthWritten = write(txFifoFd, userRequestDataCharArray,100)) > 0)
	{
		status = 0;
	}


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::sendUserRequestData: " << status << endl;
#endif
	return status;
}

#define dbg 0
string MainInterface::getUserRequestResponse()
{
	string responseData;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getUserRequestResponse: " << endl;
#endif

	while((this->rxDataSize = read(rxFifoFd,this->rxData,RX_DATA_SIZE)) > 1)
	{
		responseData += string(this->rxData);
	}
	clearBuffer(this->rxData,RX_DATA_SIZE);

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getUserRequestResponse: " << responseData << endl;
#endif
	return responseData;

}


#define dbg 2
vector<string> MainInterface::listCombos()
{
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::listCombos: " << endl;
#endif

	string dataFromOfxMain;
	vector<string> comboArray;

#if(dbg >= 1)
	cout << " PEDAL UI REQUESTING COMBO LIST" << endl;
#endif
	dataFromOfxMain.clear();
	dataFromOfxMain = this->sendUserRequestDataAndWaitForResponse("listCombos", 500);
#if(dbg >= 1)
	cout << " PEDAL UI  COMBO LIST REQUESTED" << endl;
#endif
	char *token;
	char dataFromOfxMainCharArray[RX_DATA_SIZE];
	strncpy(dataFromOfxMainCharArray, dataFromOfxMain.c_str(),dataFromOfxMain.size());
	dataFromOfxMain.clear();
	token = strtok(dataFromOfxMainCharArray,",");
	while(token != NULL)
	{
		comboArray.push_back(string(token));
		token = strtok(NULL,",");
	}

#if(dbg >= 2)
	cout << "combo list: " << endl;
	for(int i = 0; i < comboArray.size(); i++)
	{
		cout << comboArray[i] << endl;
	}

#endif


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::listCombos: " << comboArray.size() << endl;
#endif

	return comboArray;
}

#define dbg 0
string MainInterface::getComboUiData(string comboName)
{
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getComboUiData: " << comboName << endl;
#endif
	string comboString;
	if(comboName.empty() == false)
	{
		string getComboCommandString = "getCombo:" + comboName;
		comboString = this->sendUserRequestDataAndWaitForResponse(getComboCommandString, 1000);
	}
	else
	{
#if(dbg >= 2)
	cout << "combo name is empty." <<  endl;
#endif
	}

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getComboUiData: " << comboString <<  endl;
#endif
	return comboString;
}

#define dbg 0
int MainInterface::changeComboParamValue(int paramIndex, int valueIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeValue: " << endl;
#endif
	int status = 0;
	char changeValueCommand[20];
	snprintf(changeValueCommand,20,"changeValue:%d=%d", paramIndex, valueIndex);
	string changeValueCommandString = string(changeValueCommand);
	this->sendUserRequestData(changeValueCommandString);

#if(dbg >= 2)
	cout << "changeValueCommandString: " << changeValueCommandString <<  endl;
#endif


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeValue: " <<  endl;
#endif
	return status;
}

#define dbg 0
int MainInterface::saveCombo()
{
	int status = 0;

#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::saveCombo: " << endl;
#endif

	string saveComboCommandString = "saveCombo";
#if(dbg >= 2)
	cout << "saveComboCommandString: " << saveComboCommandString <<  endl;
#endif
	this->sendUserRequestDataAndWaitForResponse(saveComboCommandString,500);

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::saveCombo: " <<  endl;
#endif


	return status;
}

#define dbg 0
string MainInterface::getFlxUtilityData()
{
	string utilDataString;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getFlxUtilityData: " << endl;
#endif

	string getUtilitiesCommandString = "getUtilities";
	utilDataString = this->sendUserRequestDataAndWaitForResponse(getUtilitiesCommandString, 500);


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getFlxUtilityData: " <<  endl;
#endif
#if(dbg >= 2)
	cout << "********** utilDataString: " << utilDataString <<  endl;
#endif
	return utilDataString;
}


#define dbg 0
int MainInterface::changeFlxUtilityValue(string utilParamString, float utilValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeFlxUtilityValue: ";
	cout << utilParamString << "," << utilValue << endl;
#endif

	char changeValueCommand[70];
	snprintf(changeValueCommand,70,"changeUtilValue:%s=%f", utilParamString.c_str(), utilValue);
	string changeValueCommandString = string(changeValueCommand);
	this->sendUserRequestData(changeValueCommandString);


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeFlxUtilityValue: " << changeValueCommandString << endl;
#endif
	return status;
}

#define dbg 2
int MainInterface::saveFlxUtilityData()
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::saveFlxUtilityData: " << endl;
#endif

	this->sendUserRequestData("saveUtilities");

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::saveFlxUtilityData: " << status << endl;
#endif
	return status;
}

#define dbg 0
PedalStatus MainInterface::getCurrentStatus(string activeInterface)
{
	int status = 0;

	PedalStatus _pedalStatus;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getCurrentStatus: " << endl;
#endif

	char getCurrentStatusCommand[50];
	string getCurrentStatusResponse;

	string getCurrentStatusResponseSubstring[3];
	int stringStartIndexes[3];
	int colonIndex;
	int endIndex;

	snprintf(getCurrentStatusCommand,50,"getCurrentStatus:%s", activeInterface.c_str());
	string getCurrentStatusCommandString = string(getCurrentStatusCommand);
#if(dbg >= 2)
	cout << "saveComboCommandString: " << getCurrentStatusCommandString <<  endl;
#endif

	getCurrentStatusResponse = this->sendUserRequestDataAndWaitForResponse(getCurrentStatusCommandString, 500);


	stringStartIndexes[0] = getCurrentStatusResponse.find('|',0);
	getCurrentStatusResponseSubstring[0] = getCurrentStatusResponse.substr(0,stringStartIndexes[0]);
	getCurrentStatusResponseSubstring[0] = getCurrentStatusResponseSubstring[0].substr(getCurrentStatusResponseSubstring[0].find(":")+1);
	_pedalStatus.globalComboIndex = atoi(getCurrentStatusResponseSubstring[0].c_str());

	stringStartIndexes[1] = getCurrentStatusResponse.find('|',stringStartIndexes[0]+1);
	getCurrentStatusResponseSubstring[1] = getCurrentStatusResponse.substr(stringStartIndexes[0]+1, stringStartIndexes[1]-stringStartIndexes[0]-1);
	getCurrentStatusResponseSubstring[1] = getCurrentStatusResponseSubstring[1].substr(getCurrentStatusResponseSubstring[1].find(":")+1);
	strncpy(_pedalStatus.currentStatus,getCurrentStatusResponseSubstring[1].c_str(),20);

	getCurrentStatusResponseSubstring[2] = getCurrentStatusResponse.substr(stringStartIndexes[1]+1);
	getCurrentStatusResponseSubstring[2] = getCurrentStatusResponseSubstring[2].substr(getCurrentStatusResponseSubstring[2].find(":")+1);
	_pedalStatus.hostGuiActive = atoi(getCurrentStatusResponseSubstring[2].c_str());

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getCurrentStatus: " << _pedalStatus.globalComboIndex;
	cout << " ***** " << _pedalStatus.currentStatus << " ***** " << _pedalStatus.hostGuiActive <<endl;
#endif


	return _pedalStatus;
}

#define dbg 0
PedalStatus MainInterface::parseCurrentStatusString(string statusString)
{
	int status = 0;

	PedalStatus _pedalStatus;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::parseCurrentStatusString: " << endl;
#endif

	string getCurrentStatusResponseSubstring[3];
	int stringStartIndexes[3];
	int colonIndex;
	int endIndex;

#if(dbg >= 2)
	cout << "saveComboCommandString: " << getCurrentStatusCommandString <<  endl;
#endif

	stringStartIndexes[0] = statusString.find('|',0);
	getCurrentStatusResponseSubstring[0] = statusString.substr(0,stringStartIndexes[0]);
	getCurrentStatusResponseSubstring[0] = getCurrentStatusResponseSubstring[0].substr(getCurrentStatusResponseSubstring[0].find(":")+1);
	_pedalStatus.globalComboIndex = atoi(getCurrentStatusResponseSubstring[0].c_str());

	stringStartIndexes[1] = statusString.find('|',stringStartIndexes[0]+1);
	getCurrentStatusResponseSubstring[1] = statusString.substr(stringStartIndexes[0]+1, stringStartIndexes[1]-stringStartIndexes[0]-1);
	getCurrentStatusResponseSubstring[1] = getCurrentStatusResponseSubstring[1].substr(getCurrentStatusResponseSubstring[1].find(":")+1);
	strncpy(_pedalStatus.currentStatus,getCurrentStatusResponseSubstring[1].c_str(),20);

	getCurrentStatusResponseSubstring[2] = statusString.substr(stringStartIndexes[1]+1);
	getCurrentStatusResponseSubstring[2] = getCurrentStatusResponseSubstring[2].substr(getCurrentStatusResponseSubstring[2].find(":")+1);
	_pedalStatus.hostGuiActive = atoi(getCurrentStatusResponseSubstring[2].c_str());

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::parseCurrentStatusString: " << _pedalStatus.globalComboIndex;
	cout << " ***** " << _pedalStatus.currentStatus << " ***** " << _pedalStatus.hostGuiActive <<endl;
#endif


	return _pedalStatus;
}

#define dbg 0
PedalStatus MainInterface::getCurrentStatus()
{
	int status = 0;

	PedalStatus _pedalStatus;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getCurrentStatus: " << endl;
#endif

	char getCurrentStatusCommand[50];
	string getCurrentStatusResponse;

	string getCurrentStatusResponseSubstring[3];
	int stringStartIndexes[3];
	int colonIndex;
	int endIndex;

	snprintf(getCurrentStatusCommand,50,"getCurrentStatus");
	string getCurrentStatusCommandString = string(getCurrentStatusCommand);
#if(dbg >= 2)
	cout << "saveComboCommandString: " << getCurrentStatusCommandString <<  endl;
#endif

	getCurrentStatusResponse = this->sendUserRequestDataAndWaitForResponse(getCurrentStatusCommandString, 500);


	stringStartIndexes[0] = getCurrentStatusResponse.find('|',0);
	getCurrentStatusResponseSubstring[0] = getCurrentStatusResponse.substr(0,stringStartIndexes[0]);
	getCurrentStatusResponseSubstring[0] = getCurrentStatusResponseSubstring[0].substr(getCurrentStatusResponseSubstring[0].find(":")+1);
	_pedalStatus.globalComboIndex = atoi(getCurrentStatusResponseSubstring[0].c_str());

	stringStartIndexes[1] = getCurrentStatusResponse.find('|',stringStartIndexes[0]+1);
	getCurrentStatusResponseSubstring[1] = getCurrentStatusResponse.substr(stringStartIndexes[0]+1, stringStartIndexes[1]-stringStartIndexes[0]-1);
	getCurrentStatusResponseSubstring[1] = getCurrentStatusResponseSubstring[1].substr(getCurrentStatusResponseSubstring[1].find(":")+1);
	strncpy(_pedalStatus.currentStatus,getCurrentStatusResponseSubstring[1].c_str(),20);

	getCurrentStatusResponseSubstring[2] = getCurrentStatusResponse.substr(stringStartIndexes[1]+1);
	getCurrentStatusResponseSubstring[2] = getCurrentStatusResponseSubstring[2].substr(getCurrentStatusResponseSubstring[2].find(":")+1);
	_pedalStatus.hostGuiActive = atoi(getCurrentStatusResponseSubstring[2].c_str());

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getCurrentStatus: " << _pedalStatus.globalComboIndex;
	cout << " ***** " << _pedalStatus.currentStatus << " ***** " << _pedalStatus.hostGuiActive <<endl;
#endif


	return _pedalStatus;
}
