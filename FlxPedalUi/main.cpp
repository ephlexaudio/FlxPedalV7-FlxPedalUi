/*
 * main.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: buildrooteclipse
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <signal.h>
#include "UserInterface2.h"
#include "MainInterface.h"
#include "Combo.h"
#include "Utilities.h"
#include "FlxUtility.h"
#include "LookUpTable.h"//LookUpTable.h"

#if(dbg >= 1)
#endif

using namespace std;

static void signal_handler(int sig)
{
	//cout << "signal received: " << sig <<", OfxPedalUiRpi exiting ..." << endl;
	signal(sig, SIG_DFL);
	kill(getpid(),sig);
}

GPIOClass usbDetect = GPIOClass(USB_CONNECTED);
//GPIOClass enableUsbIO = GPIOClass(OFX_RDY0);
bool isUsbConnected();
UserInterface2 ui;



#define dbg 0
int main(int argc, char *argv[])
{
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGPIPE, signal_handler);
	signal(SIGALRM, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGSTKFLT, signal_handler);
	signal(SIGSTOP, signal_handler);
	signal(SIGXCPU, signal_handler);
	signal(SIGXFSZ, signal_handler);
	signal(SIGVTALRM, signal_handler);
	signal(SIGPROF, signal_handler);

	bool disableUsb = false;

	int status = 0;
	int menuLevel = 0;
	bool exit = false;
	bool uiChange = true;
	int rotEncOutput = 0;
	int rotEncValue = 0;
	int rotEncReadCount = 0;
	int poweroffButtonPushedCount = 0;
	char menuLcdTempString[21];

	/*int effectIndex = 0;
	int paramIndex = 0;*/
	int menuLevelIndex[2];  // replacing effectIndex and paramIndex, so can be used with utility.
	menuLevelIndex[0] = 0;
	menuLevelIndex[1] = 0;

	int softKeyGroupIndex = 0;
	bool hostUiActive = false;
	int comboIndex = 0;
	bool comboMenu = false;
	bool utilMenu = false;
	int buttonPushed = -1;
	string dataFromOfxMain;
	vector<string> comboNames;
	vector<string> utilTypeNames;
	vector<string> utilParamNames;
	vector<Parameter> softKeyParams;
	int paramValueType;
	int paramValueIndex;
	bool combosAvailable = true;
	bool userRequestSentFlag = false;
	int osSelected = 0;
	//string osSelectedName = "test";
	usbDetect.export_gpio();
	usbDetect.setdir_gpio("in");
	char verString[20] = "";
	char gadget[10] = "";

	if(argc > 1)
	{
		cout << "input arguments:" << endl;
		for(int i = 0; i < argc; i++)
		{
			cout << i << ":" << argv[i] << endl;
		}

		if(strncmp(argv[1],"-u",2) == 0)
		{
			disableUsb = true;
			cout << "********USB disabled***********" << endl;
		}
	}

	/*FILE *flxMainProc;
	//if(ofxParamJsonData["usbEnable"].asBool())
	{
		errno = 0;
		flxMainProc = popen("/home/FlxMain2 -d &", "w");
		cout << "launching FlxMain: " << strerror(errno) << endl;
	}*/

	// initialize
	MainInterface mainInt;
	Combo combo;
	FlxUtility flxUtil;
	//ui.initializeLcd();
	char version[6] = "";
	FILE *versionFP = popen("cat /home/version","r");
	fgets(version, 6, versionFP);
	pclose(versionFP);
	for(int i = 0; i < 5; i++)
	{
		if(version[i] < 46 || 57 < version[i])
		{
			version[i] = 0;
		}
	}

	snprintf(verString,20,"      V %s   ", version);
					     //1234567890123456789
	ui.writeLcdLine(1,"                ");
	ui.writeLcdLine(2,"   Ephlex Audio ");
	ui.writeLcdLine(3,"      FLX-1     ");
	ui.writeLcdLine(4,verString);
	//sleep(3);
	PedalStatus pedalStatus;
	//hostUiActive = true; // this will send execution into the  "USB disconnected,
						// get updated combo information" code to initialize the pedal.


	/**************************************************************************
	 * Get combo list and send first name of first combo to FlxMain process
	 **************************************************************************/

	comboNames = mainInt.listCombos();
	if(comboNames.size() != 0)
	{
		combosAvailable = true;
		combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
	}
	else
	{					 //1234567890123456789
		combosAvailable = false;
		ui.writeLcdLine(1,"No combos        ");
		ui.writeLcdLine(2,"available        ");
		ui.writeLcdLine(3,"                 ");
		ui.writeLcdLine(4,"                 ");
	}
	uiChange = true;


	/****************************************************************************
	 * 		Get utility parameter list from FlxMain
	 ****************************************************************************/

	flxUtil.getFlxUtilityParams(mainInt.getFlxUtilityData());

	//********************************** loop *********************************

	while(exit == false)
	{
		if(ui.isPowerButtonPushed() == true)
		{
			if(poweroffButtonPushedCount == 0)
			{
				ui.writeLcdLine(1,"Keep power button");
				ui.writeLcdLine(2,"pressed down to");
				ui.writeLcdLine(3," power off");
				ui.writeLcdLine(4,"              ");
			}

			poweroffButtonPushedCount++;
#if(dbg >= 2)
			cout << "power button is pushed:" << poweroffButtonPushedCount << endl;
#endif
			usleep(200000);

			if(poweroffButtonPushedCount > 5)
			{
				mainInt.sendUserRequestData("powerOff");
				//system("poweroff"); // not needed if OfxMain is sending poweroff CLI command
				exit = true;
			}
			/*else
			{
				if(poweroffButtonPushedCount == 1)
				{
					ui.writeLcdLine(1,"              ");
					ui.writeLcdLine(2,"POWERING OFF  ");
					ui.writeLcdLine(4,"              ");
				}
				snprintf(menuLcdTempString, 20,"IN %d", 5-poweroffButtonPushedCount);
				ui.writeLcdLine(3,menuLcdTempString);
			}*/
		}
		else
		{
			if(poweroffButtonPushedCount > 0) // power button accidentally pushed, refresh LCD
			{
				uiChange = true;
				poweroffButtonPushedCount = 0;
			}

			if(gadget[0] != 'g')
			{

				FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
				while((fgets(gadget,5,gadgetDetect)) != NULL)
				{
					//cout << gadget << endl;
				}
				pclose(gadgetDetect);
			}


			if(isUsbConnected() == true  && (disableUsb == false || gadget[0] != 'g'))
			{					 //1234567890123456789
				//char *gadgetFound = NULL;


				if(gadget[0] != 'g')
				{

					FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
					while((fgets(gadget,5,gadgetDetect)) != NULL)
					{
						//cout << gadget << endl;
					}
					pclose(gadgetDetect);


					while(osSelected == 0 && isUsbConnected() == true)
					{

						ui.writeLcdLine(1,"Host PC Detected:");
						ui.writeLcdLine(2," Select OS:     ");
						ui.writeLcdLine(3,"                ");
						ui.writeLcdLine(4," Win  Mac  Lin  ");

						while(buttonPushed == -1 && isUsbConnected() == true)  // keeps LCD from flickering
						{
							buttonPushed = ui.readButtons();
						}
						if(buttonPushed >= 0)
						{
							usleep(500); // debounce
							//if(ui.readButtons() == buttonPushed)
							buttonPushed = ui.readButtons();
							{
								if(buttonPushed > 3) buttonPushed -= 2;
								else buttonPushed--;

								switch(buttonPushed)
								{
								case 0:		// Windows selected
									osSelected = 1;
									//osSelectedName = "Windows";
									system("/root/AcmRndisUsbInit");
									break;
								case 1:		// Mac OS X selected
									osSelected = 2;
									//osSelectedName = "Mac OS X";
									system("/root/AcmEcmUsbInit");
									break;
								case 2:		// Linux selected
									osSelected = 3;
									//osSelectedName = "Linux";
									system("/root/AcmEcmUsbInit");
									break;
								default:;
								}
							}
						}
					}
				}


				if(disableUsb == false)
				{
					ui.writeLcdLine(1,"Connected to :");
					ui.writeLcdLine(2,"Host          ");

					/*************** POLL for updating activity or SFTP access **************
					FILE* updateFd = popen("cat /updateStatus", "r");
					char updateStatus[10] = "";
					while(fgets(updateStatus,10,updateFd) != NULL );
					pclose(updateFd);
					if(strncmp(updateStatus,"ready",5) == 0)
					{
						ui.writeLcdLine(3,"Ready for Update");
					}
					else if(strncmp(updateStatus,"updating",8) == 0)
					{
						ui.writeLcdLine(3,"Getting Update");
					}
					else if(strncmp(updateStatus,"done",4) == 0)
					{
						sleep(4);
						ui.writeLcdLine(3,"Update Done ");
						sleep(3);
					}
					else if(strncmp(updateStatus,"sftpAccess",10) == 0)
					{
						ui.writeLcdLine(3,"SFTP Access Granted");
					}
					/*********************************************************************/
					menuLevel = 0;
					//if(hostUiActive == false)
					{
						mainInt.getCurrentStatus("host");
						hostUiActive = true;
					}

				}
			}
			else //if((osSelected > 0 && isUsbConnected() == true)  || isUsbConnected() == false)
			{

				if(hostUiActive == true)  // USB disconnected, get updated combo information
				{
					mainInt.getCurrentStatus("pedal");
					comboNames = mainInt.listCombos();
					if(comboNames.size() != 0)
					{
						combosAvailable = true;
						combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
						uiChange = true;
					}
					else
					{					 //1234567890123456789
						combosAvailable = false;
						ui.writeLcdLine(1,"No combos        ");
						ui.writeLcdLine(2,"available        ");
						ui.writeLcdLine(3,"                 ");
						ui.writeLcdLine(4,"                 ");

					}
					hostUiActive = false;
				}
				if(combosAvailable == true)
				{


					if(rotEncReadCount < 10)  // this if/else code gives the encoder the ability to increment by speed
					{
						ui.resetEncoder();
						usleep(5000);

						int value = ui.readEncoder();
						rotEncOutput += value;
						rotEncReadCount++;

					}
					else
					{
		#if(dbg >= 1)
						cout << "rotEncOutput: " << rotEncOutput << endl;
		#endif
						rotEncReadCount = 0;
						// read buttons and process accordingly

						buttonPushed = ui.readButtons();
						if(buttonPushed >= 0)
						{
							usleep(1000); // debounce
							buttonPushed = ui.readButtons();
							//while(ui.readButtons() >= 0);
							{
								if(buttonPushed == 0) // LCD left
								{
	#if(dbg >= 2)
									cout << "LCD left pushed" << endl;
	#endif
									switch(menuLevel)
									{
									case 0:
										// send "get combo" command to OfxMain via FIFO
										if(comboIndex > 0)
										{
											comboIndex--;
										}
										combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
										break;
									case 1:

										if(comboMenu == true)
										{
											menuLevelIndex[0]/*effectIndex*/ = 0;
										}
										else if(utilMenu == true)
										{
											menuLevelIndex[0]/*effectIndex*/ = 0;
											mainInt.saveFlxUtilityData();
										}
										menuLevel--;
										comboMenu = false;  // set both to false before going to menu level 0.
										utilMenu = false;
										break;
									case 2:
										if(softKeyGroupIndex > 0)  softKeyGroupIndex--;
										else
										{

											softKeyGroupIndex = 0;
											if(comboMenu == true)
											{
												menuLevelIndex[1]/*paramIndex*/ = 0;
											}
											else if(utilMenu == true)
											{
												menuLevelIndex[1]/*paramIndex*/ = 0;
											}
											menuLevel--;
										}
										break;
									case 3:
										if(softKeyGroupIndex > 0)
										{
											softKeyGroupIndex--;
										}
										else
										{
											menuLevel--;
										}

										break;
									default:;
									}
								}
								else if(buttonPushed == 6) // LCD right
								{
	#if(dbg >= 2)
									cout << "LCD right pushed" << endl;
	#endif
									switch(menuLevel)
									{
									case 0:
										// send "get combo" command to OfxMain via FIFO
										if(comboIndex < comboNames.size()-1)
										{
											comboIndex++;
										}
										combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
										break;
									case 1:
										break;
									case 2:
										if(comboMenu == true)
										{
											// only increment if there are more params to the right
											if(4*(softKeyGroupIndex+1) < combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParamCount())
											{
												softKeyGroupIndex++;
											}
										}
										else if(utilMenu == true)
										{

										}
										break;
									case 3:
										if(comboMenu == true)
										{
											// only increment if there are more params to the right
											if(4*(softKeyGroupIndex+1) < combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParamCount())
											{
												softKeyGroupIndex++;
											}
										}
										else if(utilMenu == true)
										{

										}
										break;
									default:;
									}
								}
								else if(buttonPushed == 3) // Rotary encoder switch
								{
									switch(menuLevel)
									{
									case 0:
										break;
									case 1:
										if(comboMenu == true)
										{
											menuLevelIndex[0]/*effectIndex*/ = 0;
										}
										else if(utilMenu == true)
										{
											menuLevelIndex[0]/*effectIndex*/ = 0;
											mainInt.saveFlxUtilityData();
										}
										menuLevel--;
										comboMenu = false;
										utilMenu = false;
										break;
									case 2:
										softKeyGroupIndex = 0;
										if(comboMenu == true)
										{
											menuLevelIndex[1]/*paramIndex*/ = 0;
										}
										else if(utilMenu == true)
										{
											menuLevelIndex[1]/*paramIndex*/ = 0;
										}
										menuLevel--;
										break;
									case 3:
										menuLevel--;
										break;
									default:;
									}
								}
								else // softkey
								{
									if(buttonPushed > 3) buttonPushed -= 2;
									else buttonPushed--;

									switch(menuLevel)
									{
									case 0:
										comboMenu = false;
										utilMenu = false;
										if(buttonPushed == 0)
										{
											// send "save combo" command to OfxMain via FIFO
											mainInt.saveCombo();
										}
										else if(buttonPushed == 1)
										{
											menuLevel = 1;
											comboMenu = true;
										}
										else if(buttonPushed == 2)
										{
											menuLevel = 1;
											utilMenu = true;
										}


										break;
									case 1:		// effects in softkeys
										if(comboMenu == true)
										{
											if(buttonPushed < combo.getEffects().size())
											{
												menuLevelIndex[0]/*effectIndex*/ = buttonPushed;
												menuLevel = 2;
											}
										}
										else if(utilMenu == true)
										{
											menuLevelIndex[0]/*effectIndex*/ = buttonPushed;
											menuLevel = 2;
										}

										break;
									case 2:		// parameters in softkeys
										if(comboMenu == true)
										{
											if((4*softKeyGroupIndex+buttonPushed) < combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParamCount())
											{
												menuLevelIndex[1]/*paramIndex*/ = 4*softKeyGroupIndex+buttonPushed;
												menuLevel = 3;
											}
										}
										else if(utilMenu == true)
										{
											menuLevelIndex[1]/*paramIndex*/ = buttonPushed;
											menuLevel = 3;
										}
										break;
									case 3:		// parameters in softkeys
										if(comboMenu == true)
										{
											if((4*softKeyGroupIndex+buttonPushed) < combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParamCount())
											{
												menuLevelIndex[1]/*paramIndex*/ = 4*softKeyGroupIndex+buttonPushed;
												menuLevel = 3;
											}
										}
										break;
									default:;
									}
								}
								uiChange = true;
								while(buttonPushed == ui.readButtons()); // wait for button to be released
							}
						}

						// update LCD
						if(uiChange)
						{
							switch(menuLevel)
							{
							case 0:
								snprintf(menuLcdTempString,20,"%s",combo.getName().c_str());
								ui.writeLcdLine(1,menuLcdTempString);
								snprintf(menuLcdTempString,20," Save Prms Util");
								ui.writeLcdLine(4,menuLcdTempString);
								break;
							case 1:
								if(comboMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s",combo.getName().c_str());
									ui.writeLcdLine(1,menuLcdTempString);
									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									for(int effectIndex = 0; effectIndex < combo.getEffects().size(); effectIndex++)
									{
										strncat(menuLcdTempString, combo.getEffect(effectIndex).getAbbr().c_str(), 4);
										strcat(menuLcdTempString, " ");
									}
									ui.writeLcdLine(4,menuLcdTempString);
								}
								else if(utilMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s","util");
									ui.writeLcdLine(1,menuLcdTempString);
									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									for(int utilTypeIndex = 0; utilTypeIndex < flxUtil.getFlxUtilityTypeVectorSize(); utilTypeIndex++)
									{
										strncat(menuLcdTempString, flxUtil.getFlxUtilityTypeAbbr(utilTypeIndex).c_str(), 4);
										strcat(menuLcdTempString, " ");
									}
									ui.writeLcdLine(4,menuLcdTempString);
								}
								break;
							case 2:
								if(comboMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s->%s",combo.getName().c_str(),
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getName().c_str());
									ui.writeLcdLine(1,menuLcdTempString);

									/*clearBuffer(menuLcdTempString,21);
									paramValueType = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueType();
									paramValueIndex = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueIndex();
									snprintf(menuLcdTempString,20,"%s: %s %s      ",
											combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getName().c_str(),
											lutArray[paramValueType][paramValueIndex].c_str(),lutArray[paramValueType][100].c_str());
									ui.writeLcdLine(2,menuLcdTempString);*/

									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									softKeyParams = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParams(4*softKeyGroupIndex);
									for(int paramIndex = 0; paramIndex < softKeyParams.size(); paramIndex++)
									{
										strncat(menuLcdTempString, softKeyParams[paramIndex].getAbbr().c_str(), 4);
										strcat(menuLcdTempString, " ");
									}

									ui.writeLcdLine(4,menuLcdTempString);
								}
								else if(utilMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s->%s","util",flxUtil.getFlxUtilityTypeAbbr(menuLevelIndex[0]).c_str());
									ui.writeLcdLine(1,menuLcdTempString);


									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									utilParamNames = flxUtil.getFlxUtilityParamAbbrList(menuLevelIndex[0]);
									for(int utilParamIndex = 0; utilParamIndex < utilParamNames.size(); utilParamIndex++)
									{
										cout << "getFlxUtilityParamAbbr: " << utilParamNames[utilParamIndex].c_str() << endl;
										strncat(menuLcdTempString, utilParamNames[utilParamIndex].c_str(), 4);
										strcat(menuLcdTempString, " ");
									}
									ui.writeLcdLine(4,menuLcdTempString);
								}
								break;
							case 3:
								if(comboMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s->%s",combo.getName().c_str(),
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getName().c_str());
									ui.writeLcdLine(1,menuLcdTempString);



									paramValueType = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueType();
									paramValueIndex = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueIndex();
									snprintf(menuLcdTempString,20,"%s: %s %s      ",
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getName().c_str(),
											lutArray[paramValueType][paramValueIndex].c_str(), lutArray[paramValueType][100].c_str());
									ui.writeLcdLine(2,menuLcdTempString);
									// send "value change" command to OfxMain via FIFO
									mainInt.changeComboParamValue(combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getParamIndex(),
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueIndex());




									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									softKeyParams = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParams(4*softKeyGroupIndex);
									for(int paramIndex = 0; paramIndex < softKeyParams.size(); paramIndex++)
									{
										strncat(menuLcdTempString, softKeyParams[paramIndex].getAbbr().c_str(), 4);
										strcat(menuLcdTempString, " ");
									}

									ui.writeLcdLine(4,menuLcdTempString);

								}

								else if(utilMenu == true)
								{
									snprintf(menuLcdTempString,20,"%s->%s","util",
											flxUtil.getFlxUtilityTypeAbbr(menuLevelIndex[0]).c_str());
									ui.writeLcdLine(1,menuLcdTempString);
									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									snprintf(menuLcdTempString,20,"%s %f       ",
											flxUtil.getFlxUtilityParamName(menuLevelIndex[0], menuLevelIndex[1]).c_str(),
											flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1])
											);
									ui.writeLcdLine(2,menuLcdTempString);
								}
								break;
							default:;
							}
							uiChange = false;
						}
						else if(rotEncOutput != 0) // read encoder
						{
							if(menuLevel == 3)
							{
								if(comboMenu == true)
								{
									combo.updateParameter(menuLevelIndex[0]/*effectIndex*/,menuLevelIndex[1]/*paramIndex*/,rotEncOutput);

									paramValueType = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueType();
									paramValueIndex = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueIndex();
									snprintf(menuLcdTempString,20,"%s: %s %s      ",
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getName().c_str(),
											lutArray[paramValueType][paramValueIndex].c_str(), lutArray[paramValueType][100].c_str());
									ui.writeLcdLine(2,menuLcdTempString);
									// send "value change" command to OfxMain via FIFO
									mainInt.changeComboParamValue(combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getParamIndex(),
											combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParam(menuLevelIndex[1]/*paramIndex*/).getValueIndex());

									clearBuffer(menuLcdTempString,21);
									strcpy(menuLcdTempString, " ");
									softKeyParams = combo.getEffect(menuLevelIndex[0]/*effectIndex*/).getParams(4*softKeyGroupIndex);
									for(int paramIndex = 0; paramIndex < softKeyParams.size(); paramIndex++)
									{
										strncat(menuLcdTempString, softKeyParams[paramIndex].getAbbr().c_str(), 4);
										strcat(menuLcdTempString, " ");
									}

									ui.writeLcdLine(4,menuLcdTempString);



								}



								else if(utilMenu == true)
								{
									string typeName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]);
									string paramName = flxUtil.getFlxUtilityParamName(menuLevelIndex[0], menuLevelIndex[1]);
									flxUtil.changeFlxUtilityValue(menuLevelIndex[0], menuLevelIndex[1],rotEncOutput);
									snprintf(menuLcdTempString,20,"%s %f       ", paramName.c_str(),
											flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]));
									ui.writeLcdLine(2,menuLcdTempString);
									// send "util value change" command to OfxMain via FIFO
									string typeParamName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]) + "_"
											+ flxUtil.getFlxUtilityParamName(menuLevelIndex[0],menuLevelIndex[1]);
									mainInt.changeFlxUtilityValue(typeParamName,
											flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]));
								}
							}
						}
						rotEncOutput = 0;
					}
				}
				else
				{

				}
			}
		}
	}
	char emptyLine[20];
	 	 	 	 	 //1234567890123456789
	/*ui.writeLcdLine(1,emptyLine);
	ui.writeLcdLine(2,"POWERING OFF  ");
	ui.writeLcdLine(3,emptyLine);
	ui.writeLcdLine(4,emptyLine);*/
	//enableUsbIO.setval_gpio(0);


	return status;
}

#define dbg 0
bool isUsbConnected()
{

	bool status = false;
	int usbLine = 0;
#if(dbg >= 1)
	cout << "********** ENTERING FlxPedalUi:main:isUsbConnected: " <<  endl;
#endif

	usbDetect.getval_gpio(usbLine);
	if(usbLine == 1) status = true;
	//if(ui.readButtons() == 5) status = true;

#if(dbg >= 1)
	cout << "********** EXITING FlxPedalUi:main::isUsbConnected: " << status << endl;
#endif

	return status;
}

