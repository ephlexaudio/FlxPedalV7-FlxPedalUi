/*
 * MainInterface.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include "MainInterface.h"
namespace std
{





#define dbg 0
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

	int rxFifoFdErr = 0;  // used if debug level is set to 2
	int txFifoFdErr = 0;

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
	strcpy(this->fromMainIntName,"/ipc_toPedalUiInt");
	strcpy(this->toMainIntName,"/ipc_fromPedalUiInt");

	this->fromMainIntFD = -1;
	this->toMainIntFD = -1;
	this->fromMainIntMemory = NULL;
	this->toMainIntMemory = NULL;

	this->openFlxMainComm();
	this->closeFlxMainComm();




#if(dbg >= 2)
	cout << "FIFOs opened." << endl;
#endif
}

MainInterface::~MainInterface() {
	// TODO Auto-generated destructor stub

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




#define dbg 0
string MainInterface::sendUserRequestDataAndWaitForResponse(string userRequestData, int waitTime)
{
	int lengthWritten = 0;
	int status = 0;
	string responseString;
	responseString.resize(1000);
	char responseData[2000];
	clearBuffer(responseData,2000);
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::sendUserRequestDataAndWaitForResponse: " << userRequestData << endl;
#endif
	try
	{
		char rxData[RX_DATA_SIZE];
		char txData[TX_DATA_SIZE];
		int rxDataSize = 0;

		clearBuffer(rxData,RX_DATA_SIZE);
		clearBuffer(txData,TX_DATA_SIZE);
		strncpy(txData,userRequestData.c_str(),TX_DATA_SIZE);
		lengthWritten = write(txFifoFd, txData,TX_DATA_SIZE);

		if(lengthWritten > 0)
		{
			usleep(waitTime*1000);
			while((rxDataSize = read(rxFifoFd,rxData,RX_DATA_SIZE)) > 1)
			{
				strcat(responseData,rxData);
			}
			clearBuffer(rxData,RX_DATA_SIZE);
		}
		responseString = string(responseData);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::sendUserRequestDataAndWaitForResponse: " << e.what() << endl;
		clearBuffer(responseData,2000);
		status = -1;
	}

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::sendUserRequestDataAndWaitForResponse: "  << "\t\tstatus: " << status << endl;
#endif
#if(dbg >= 2)
	cout << "responseString: " << responseString << endl;
#endif

	return responseString;
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
	try
	{

#if(dbg >= 2)
		for(auto & requestChar : userRequestData)
		{
			cout << requestChar << "(" << (int)requestChar << "), ";
		}
		cout << endl;

#endif

		if((lengthWritten = write(txFifoFd, userRequestData.c_str(),userRequestData.size())) > 0)
		{
			status = 0;
		}
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::sendUserRequestData: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::sendUserRequestData: " << status << endl;
#endif
	return status;
}



#define dbg 0
vector<string> MainInterface::listCombos()
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::listCombos: " << endl;
#endif
	string dataFromFlxMain;
	vector<string> comboArray;

	try
	{
#if(dbg >= 1)
		cout << " PEDAL UI REQUESTING COMBO LIST" << endl;
#endif
		dataFromFlxMain.clear();
		comboArray.clear();
		dataFromFlxMain = this->sendUserRequestDataAndWaitForResponse("listCombos", 1000);
#if(dbg >= 1)
		cout << " PEDAL UI  COMBO LIST REQUESTED" << endl;
#endif
		char *token;
		char dataFromFlxMainCharArray[RX_DATA_SIZE];
		clearBuffer(dataFromFlxMainCharArray,RX_DATA_SIZE);
		strncpy(dataFromFlxMainCharArray, dataFromFlxMain.c_str(),dataFromFlxMain.size());
		dataFromFlxMain.clear();
		token = strtok(dataFromFlxMainCharArray,",");
		while(token != NULL)
		{
			comboArray.push_back(string(token));
			token = strtok(NULL,",");
		}

#if(dbg >= 2)
		cout << "combo list: " << endl;
		for(auto & combo : comboArray)
		{
			cout << combo << endl;
		}

#endif

	}
	catch(exception &e)
	{
		cout << "error in MainInterface::listCombos: " << e.what() << endl;
		comboArray.clear();
		status = -1;
	}



#if(dbg >= 1)
	cout << "********** EXITING MainInterface::listCombos: " << comboArray.size() << "\t\tstatus: " << status << endl;
#endif

	return comboArray;
}

#define dbg 0
string MainInterface::getComboUiData(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getComboUiData: " << comboName << endl;
#endif
	string comboString;
	comboString.resize(1000,'\0');
	try
	{
		if(comboName.empty() == false)
		{
			string getComboCommandString = "getCombo:" + comboName;
			comboString = this->sendUserRequestDataAndWaitForResponse(getComboCommandString, 500);
		}
		else
		{
#if(dbg >= 2)
			cout << "combo name is empty." <<  endl;
#endif
		}
	}
	catch(exception &e)
	{
		status = -1;
		cout << "error in MainInterface::getComboUiData: " << e.what() << endl;
		comboString.clear();
	}

#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getComboUiData: " << "\t\t status: " << status <<  endl;
#endif
#if(dbg >= 2)
	cout << "comboString: " << comboString <<   endl;
#endif
	return comboString;
}

#define dbg 0
int MainInterface::changeComboParamValue(ControlParameterPair controlParameterPair, int valueIndex)
{
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeComboParamValue: " << endl;
#endif
	int status = 0;
	try
	{
		char changeValueCommand[100];
		clearBuffer(changeValueCommand,100);
		snprintf(changeValueCommand,100,"changeValue:%s:%s=%d", controlParameterPair.parentControl.c_str(),
				 controlParameterPair.parameter.c_str(), valueIndex);
		string changeValueCommandString = string(changeValueCommand);
		this->sendUserRequestData(changeValueCommandString);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::changeComboParamValue: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 2)
	cout << "changeValueCommandString: " << changeValueCommandString <<  endl;
#endif


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeComboParamValue: " << status << endl;
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
	try
	{
		string saveComboCommandString = "saveCombo";
#if(dbg >= 2)
		cout << "saveComboCommandString: " << saveComboCommandString <<  endl;
#endif
		this->sendUserRequestDataAndWaitForResponse(saveComboCommandString,500);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::saveCombo: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::saveCombo: " << status <<  endl;
#endif


	return status;
}

#define dbg 2
string MainInterface::getFlxUtilityData()
{
	string utilDataString;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::getFlxUtilityData: " << endl;
#endif
	try
	{
		string getUtilitiesCommandString = "getUtilities";
		utilDataString = this->sendUserRequestDataAndWaitForResponse(getUtilitiesCommandString, 1000);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::getFlxUtilityData: " << e.what() << endl;
		utilDataString.clear();
	}



#if(dbg >= 1)
	cout << "********** EXITING MainInterface::getFlxUtilityData: " <<  endl;
#endif
#if(dbg >= 2)
	cout << "********** utilDataString: " << utilDataString <<  endl;
#endif
	return utilDataString;
}


#define dbg 0
int MainInterface::changeFlxUtilityValue(string utilParamString, double utilValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeFlxUtilityValue: ";
	cout << utilParamString << "," << utilValue << endl;
#endif
	string changeValueCommandString;
	ostringstream strs;
	string doubleStr;
	try
	{
		strs << utilValue;
		doubleStr = strs.str();
		changeValueCommandString = "changeUtilValue:" + utilParamString + "=" + doubleStr;
		this->sendUserRequestData(changeValueCommandString);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::changeFlxUtilityValue: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeFlxUtilityValue: " << changeValueCommandString << "\t\tstatus: " << status << endl;
#endif
	return status;
}

#define dbg 0
int MainInterface::changeFlxUtilityValue(string utilParamString, int utilValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeFlxUtilityValue: ";
	cout << utilParamString << "," << utilValue << endl;
#endif
	string changeValueCommandString;
	ostringstream strs;
	string intStr;
	try
	{
		strs << utilValue;
		intStr = strs.str();

		changeValueCommandString = "changeUtilValue:" + utilParamString + "=" + intStr;
		this->sendUserRequestData(changeValueCommandString);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::changeFlxUtilityValue: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeFlxUtilityValue: " << changeValueCommandString << "\t\tstatus: " << status << endl;
#endif
	return status;
}

int MainInterface::changeFlxUtilityValue(string utilParamString, string utilParamOption)
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::changeFlxUtilityValue: ";
	cout << utilParamString << "," << utilParamOption << endl;
#endif
	string changeValueCommandString;
	try
	{
		changeValueCommandString = "changeUtilValue:" + utilParamString + "=" + utilParamOption;

		this->sendUserRequestData(changeValueCommandString);
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::changeFlxUtilityValue: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	cout << "********** EXITING MainInterface::changeFlxUtilityValue: " << changeValueCommandString << "\t\tstatus: " << status << endl;
#endif
	return status;
}


#define dbg 0
int MainInterface::saveFlxUtilityData()
{
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING MainInterface::saveFlxUtilityData: " << endl;
#endif
	try
	{
		this->sendUserRequestData("saveUtilities");
	}
	catch(exception &e)
	{
		cout << "error in MainInterface::saveFlxUtilityData: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	cout << "********** EXITING MainInterface::saveFlxUtilityData: " << status << endl;
#endif
	return status;
}

int MainInterface::openFlxMainComm(void)
{
	int status = 0;

	int toFlxMainFD = shm_open (toMainIntName, O_RDWR, S_IRUSR | S_IWUSR);
	if(toFlxMainFD == -1)
	{
		printf("toMainIntFD open failed.\n");
	}

	else
	{
		errno = 0;
		toMainIntMemory = (struct _ipcData *)mmap (0, 100, PROT_WRITE,
	                        MAP_SHARED, toFlxMainFD, 0);
		if(toMainIntMemory == MAP_FAILED)
		{
			printf("toMainIntMemory mmap failed.\n");
			printf("errno %d:%s\n", errno, strerror(errno));
		}
	}


	int fromFlxMainFD = shm_open (fromMainIntName, O_RDWR, S_IRUSR | S_IWUSR);
	if(fromFlxMainFD == -1)
	{
		printf("fromMainIntFD open failed.\n");
	}
	else
	{
		errno = 0;
		fromMainIntMemory = (struct _ipcData *)mmap (0, 100, PROT_READ | PROT_WRITE,
	                        MAP_SHARED, fromFlxMainFD, 0);
		if(fromMainIntMemory == MAP_FAILED)
		{
			printf("fromMainIntMemory mmap failed.\n");
			printf("errno %d:%s\n", errno, strerror(errno));
		}

	}

	return status;
}

int MainInterface::closeFlxMainComm(void)
{
	int status = 0;

    close (toMainIntFD);
	if(toMainIntFD == -1)
	{
		printf("toMainIntFD close failed.\n");
	}

    close (fromMainIntFD);
	if(fromMainIntFD == -1)
	{
		printf("fromMainIntFD close failed.\n");
	}

	return status;
}


PedalStatus MainInterface::readFlxMain(void)
{
	PedalStatus pedalStatus;

	if(fromMainIntMemory->change > 0)
	{
		pedalStatus.comboName = fromMainIntMemory->comboName;
		pedalStatus.currentStatus = fromMainIntMemory->currentStatus;
		pedalStatus.usbPortOpen = fromMainIntMemory->usbPortOpen;
		pedalStatus.hostGuiActive = fromMainIntMemory->hostGuiActive;
		fromMainIntMemory->change = 0;
	}
	return pedalStatus;
}
}
