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
#include "LookUpTable.h"

#if(dbg >= 1)
#endif

using namespace std;

static void signal_handler(int sig)
{
	signal(sig, SIG_DFL);
	kill(getpid(),sig);
}

GPIOClass usbDetect = GPIOClass(USB_CONNECTED);
bool isUsbConnected();
int openUsbPort();
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

	int menuLevelIndex[2];  // replacing effectIndex and paramIndex, so can be used with utility.
	menuLevelIndex[0] = 0;
	menuLevelIndex[1] = 0;

	int softKeyGroupIndex = 0;
	bool hostUiActive = false;
	int comboIndex = 0;
	int comboUtilSelect = 0;

	int buttonPushed = -1;
	string dataFromOfxMain;
	vector<string> comboNames;
	vector<string> utilTypeNames;
	vector<string> utilParamNames;
	vector<Parameter> softKeyParams;
	int paramValueType;
	int paramValueIndex;
	bool combosAvailable = true;
	bool noCombos = false;
	bool userRequestSentFlag = false;

	usbDetect.export_gpio();
	usbDetect.setdir_gpio("in");
	char verString[20] = "";
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


	// initialize
	MainInterface mainInt;
	Combo combo;
	FlxUtility flxUtil;
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
	cout << "version: " << verString << endl;
	ui.writeLcdLine(1,"                ");
	ui.writeLcdLine(2,"   Ephlex Audio ");
	ui.writeLcdLine(3,"      FLX-1     ");
	ui.writeLcdLine(4,verString);
	PedalStatus pedalStatus;


	/**************************************************************************
	 * Get combo list and send first name of first combo to FlxMain process
	 **************************************************************************/

	for(int i = 0; comboNames.empty() == true && i < 5; i++)
	{
		comboNames = mainInt.listCombos();
		usleep(100000);
	}

	if(comboNames.size() != 0)
	{
		combosAvailable = true;
		cout << "combosAvailable: " << combosAvailable << endl;
		combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
	}
	else
	{
		combosAvailable = false;
		cout << "combosAvailable: " << combosAvailable << endl;
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
				exit = true;
			}
		}
		else
		{
			if(poweroffButtonPushedCount > 0) // power button accidentally pushed, refresh LCD
			{
				uiChange = true;
				poweroffButtonPushedCount = 0;
			}



			{
				if(hostUiActive == true && isUsbConnected() == false)  // USB disconnected, get updated combo information
				{
					mainInt.getCurrentStatus("pedal");
					comboUtilSelect = 0;
					for(int i = 0; i < 5; i++)
					{
						comboNames = mainInt.listCombos();
						if(comboNames.size() != 0)
						{
							combosAvailable = true;
							combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
							uiChange = true;
							break;
						}
					}

					if(comboNames.size() == 0)
					{
						combosAvailable = false;
						ui.writeLcdLine(1,"No combos        ");
						ui.writeLcdLine(2,"available        ");
						ui.writeLcdLine(3,"                 ");
						ui.writeLcdLine(4,"                 ");

					}
					hostUiActive = false;
				}
				else if(hostUiActive == false && isUsbConnected() == true)  // USB connected
				{
					openUsbPort();
					mainInt.getCurrentStatus("host");
					hostUiActive = true;
					comboUtilSelect = 1;
					uiChange = true;
				}

				if(combosAvailable == true)
				{
					noCombos = false;
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

						/***********Split into two seperate menu branches: combo and utility *******/

						buttonPushed = ui.readButtons();
						if(buttonPushed >= 0)
						{
							usleep(1000); // debounce
							buttonPushed = ui.readButtons();
						}

							// read buttons and process accordingly
							if(comboUtilSelect == 0)
							{
								{// begin button block
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

												menuLevelIndex[0] = 0;
											menuLevel--;
											break;
										case 2:
											if(softKeyGroupIndex > 0)  softKeyGroupIndex--;
											else
											{
												softKeyGroupIndex = 0;
													menuLevelIndex[1] = 0;
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
										uiChange = true;
									}
									else if(buttonPushed == 6) // LCD right
									{
										try
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
													// only increment if there are more params to the right
													if(4*(softKeyGroupIndex+1) < combo.getEffect(menuLevelIndex[0]).getParamCount())
													{
														softKeyGroupIndex++;
													}
												break;
											case 3:
													// only increment if there are more params to the right
													if(4*(softKeyGroupIndex+1) < combo.getEffect(menuLevelIndex[0]).getParamCount())
													{
														softKeyGroupIndex++;
													}
												break;
											default:;
											}
											uiChange = true;

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD Right error: " << e.what() << endl;
										}





									}
									else if(buttonPushed == 3) // Rotary encoder switch
									{
										switch(menuLevel)
										{
										case 0:
											break;
										case 1:
												menuLevelIndex[0] = 0;
											menuLevel--;
											break;
										case 2:
											softKeyGroupIndex = 0;
												menuLevelIndex[1] = 0;
											menuLevel--;
											break;
										case 3:
											menuLevel--;
											break;
										default:;
										}
										uiChange = true;
									}
									else if (buttonPushed >= 0)
									{
										try
										{
											int softkey = buttonPushed;
											if(softkey > 3) softkey -= 2;
											else softkey--;
											cout << "FlxPedalUi: softkey1: " << softkey << endl;
											switch(menuLevel)
											{
											case 0:
												if(softkey == 0)
												{
													// send "save combo" command to OfxMain via FIFO
													mainInt.saveCombo();
												}
												else if(softkey == 1)
												{
													menuLevel = 1;

												}
												else if(softkey == 2)
												{
													menuLevel = 1;
													comboUtilSelect = 1;
												}

												break;
											case 1:		// effects in softkeys
													if(softkey < combo.getEffects().size())
													{
														menuLevelIndex[0] = softkey;
														if(combo.getEffect(menuLevelIndex[0]).getParamCount() > 0)
														{
															menuLevel = 2;
														}

													}
												break;
											case 2:		// parameters in softkeys
													if((4*softKeyGroupIndex+softkey) < combo.getEffect(menuLevelIndex[0]).getParamCount())
													{
														menuLevelIndex[1] = 4*softKeyGroupIndex+softkey;
													}

												break;
											default:;
											}
											uiChange = true;

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, softkey error: " << e.what() << endl;
										}

									}

								}// end button block

									while(buttonPushed == ui.readButtons() && buttonPushed >= 0); // wait for button to be released



								// update LCD
								if(uiChange  && comboUtilSelect == 0)
								{

									cout << "FlxPedal: menuLevel1: " << menuLevel << endl;


									switch(menuLevel)
									{
									case 0:
										try
										{
											if(isUsbConnected() == false  || disableUsb == true)
											{
												snprintf(menuLcdTempString,20,"%s",combo.getName().c_str());
												ui.writeLcdLine(1,menuLcdTempString);
												snprintf(menuLcdTempString,20," Save Efx  Util");
												ui.writeLcdLine(4,menuLcdTempString);
											}
											else
											{
												ui.writeLcdLine(1,"Connected to :");
												ui.writeLcdLine(2,"Host          ");
												snprintf(menuLcdTempString,20,"           Util");
												ui.writeLcdLine(4,menuLcdTempString);

											}

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 0: " << e.what() << endl;
										}




										break;
									case 1:
										try
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
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 1: " << e.what() << endl;
										}
										break;
									case 2:
										try
										{
											snprintf(menuLcdTempString,20,"%s->%s",combo.getName().c_str(),
													combo.getEffect(menuLevelIndex[0]).getName().c_str());
											ui.writeLcdLine(1,menuLcdTempString);

											paramValueType = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueType();
											paramValueIndex = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueIndex();
											snprintf(menuLcdTempString,20,"%s: %s %s      ",
													combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getName().c_str(),
													lutArray[paramValueType][paramValueIndex].c_str(), lutArray[paramValueType][100].c_str());
											ui.writeLcdLine(2,menuLcdTempString);
											// send "value change" command to OfxMain via FIFO

											clearBuffer(menuLcdTempString,21);
											strcpy(menuLcdTempString, " ");
											softKeyParams = combo.getEffect(menuLevelIndex[0]).getParams(4*softKeyGroupIndex);
											for(int paramIndex = 0; paramIndex < softKeyParams.size(); paramIndex++)
											{
												strncat(menuLcdTempString, softKeyParams[paramIndex].getAbbr().c_str(), 4);
												strcat(menuLcdTempString, " ");
											}

											ui.writeLcdLine(4,menuLcdTempString);
										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 2: " << e.what() << endl;
										}

										break;



									default:;
									}
									rotEncOutput = 0;
									uiChange  = false;

								}
								else if(rotEncOutput != 0 && comboUtilSelect == 0) // read encoder
								{
									if(menuLevel == 2)
									{
										try
										{
											combo.updateParameter(menuLevelIndex[0],menuLevelIndex[1],rotEncOutput);

											paramValueType = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueType();
											paramValueIndex = combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueIndex();
											snprintf(menuLcdTempString,20,"%s: %s %s      ",
													combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getName().c_str(),
													lutArray[paramValueType][paramValueIndex].c_str(), lutArray[paramValueType][100].c_str());
											ui.writeLcdLine(2,menuLcdTempString);
											// send "value change" command to OfxMain via FIFO
											mainInt.changeComboParamValue(combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getParamIndex(),
													combo.getEffect(menuLevelIndex[0]).getParam(menuLevelIndex[1]).getValueIndex());

											clearBuffer(menuLcdTempString,21);
											strcpy(menuLcdTempString, " ");
											softKeyParams = combo.getEffect(menuLevelIndex[0]).getParams(4*softKeyGroupIndex);
											for(int paramIndex = 0; paramIndex < softKeyParams.size(); paramIndex++)
											{
												strncat(menuLcdTempString, softKeyParams[paramIndex].getAbbr().c_str(), 4);
												strcat(menuLcdTempString, " ");
											}
											ui.writeLcdLine(4,menuLcdTempString);

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 3 rotary: " << e.what() << endl;
										}
									}
									rotEncOutput = 0;
									uiChange  = false;
								}


							}
							else if(comboUtilSelect == 1)
							{
								{
									if(buttonPushed == 0) // LCD left
									{
		#if(dbg >= 2)
										cout << "LCD left pushed" << endl;
		#endif
										switch(menuLevel)
										{
										case 0:
											break;
										case 1:

												menuLevelIndex[0] = 0;
												mainInt.saveFlxUtilityData();
												if(isUsbConnected() == false || disableUsb == true)
												{
													comboUtilSelect = 0;
												}

											menuLevel--;
											break;
										case 2:
											if(softKeyGroupIndex > 0)  softKeyGroupIndex--;
											else
											{

												softKeyGroupIndex = 0;
													menuLevelIndex[1] = 0;

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
										uiChange = true;
									}
									else if(buttonPushed == 6) // LCD right
									{
		#if(dbg >= 2)
										cout << "LCD right pushed" << endl;
		#endif
										uiChange = true;
									}
									else if(buttonPushed == 3) // Rotary encoder switch
									{
										switch(menuLevel)
										{
										case 0:
											break;
										case 1:
												menuLevelIndex[0] = 0;
												mainInt.saveFlxUtilityData();
												if(isUsbConnected() == false || disableUsb == true)
												{
													comboUtilSelect = 0;
												}

											menuLevel--;
											break;
										case 2:
											softKeyGroupIndex = 0;
												menuLevelIndex[1] = 0;
											menuLevel--;
											break;
										case 3:
											menuLevel--;
											break;
										default:;
										}
										uiChange = true;
									}
									else if (buttonPushed >= 0)
									{
										try
										{
											int softkey = buttonPushed;
											if(softkey > 3) softkey -= 2;
											else softkey--;

											switch(menuLevel)
											{
											case 0:
												if(softkey == 0)
												{
												}
												else if(softkey == 1)
												{
													// should be no access to combo menus if host is connected
													if(hostUiActive == false)
													{
														menuLevel = 1;
														comboUtilSelect = 0;
													}

												}
												else if(softkey == 2)
												{
													menuLevel = 1;
												}


												break;
											case 1:		// utilities in softkeys

												if(softkey < flxUtil.getFlxUtilityTypeVectorSize())
												{
													menuLevelIndex[0] = softkey;
													menuLevel = 2;
												}

												break;
											case 2:		// utility parameters in softkeys
												if(4*softKeyGroupIndex+softkey < flxUtil.getFlxUtilityParamVectorSize(menuLevelIndex[0]))
												{
													menuLevelIndex[1] = softkey;
												}

												break;
											default:;
											}
											uiChange = true;

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=1, softkey error: " << e.what() << endl;

										}

									}
								}
								while(buttonPushed == ui.readButtons() && buttonPushed >= 0); // wait for button to be released

								// update LCD
								if(uiChange && comboUtilSelect == 1)
								{
									switch(menuLevel)
									{
									case 0:
										try
										{
											if(isUsbConnected() == false || disableUsb == true)
											{
												snprintf(menuLcdTempString,20,"%s",combo.getName().c_str());
												ui.writeLcdLine(1,menuLcdTempString);
												snprintf(menuLcdTempString,20," Save Efx  Util");
												ui.writeLcdLine(4,menuLcdTempString);
											}
											else
											{
												ui.writeLcdLine(1,"Connected to :");
												ui.writeLcdLine(2,"Host          ");
												snprintf(menuLcdTempString,20,"           Util");
												ui.writeLcdLine(4,menuLcdTempString);

											}

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 0: " << e.what() << endl;

										}


										break;
									case 1:
										try
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
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 1: " << e.what() << endl;

										}
										break;
									case 2:
										try
										{
											snprintf(menuLcdTempString,20,"%s->%s","util",flxUtil.getFlxUtilityTypeAbbr(menuLevelIndex[0]).c_str());
											ui.writeLcdLine(1,menuLcdTempString);


											clearBuffer(menuLcdTempString,21);
											strcpy(menuLcdTempString, " ");

											snprintf(menuLcdTempString,20,"%s %f       ",
													flxUtil.getFlxUtilityParamName(menuLevelIndex[0], menuLevelIndex[1]).c_str(),
													flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1])
													);
											ui.writeLcdLine(2,menuLcdTempString);
											clearBuffer(menuLcdTempString,21);
											strcpy(menuLcdTempString, " ");
											utilParamNames = flxUtil.getFlxUtilityParamAbbrList(menuLevelIndex[0]);
											for(int utilParamIndex = 0; utilParamIndex < utilParamNames.size(); utilParamIndex++)
											{
												strncat(menuLcdTempString, utilParamNames[utilParamIndex].c_str(), 4);
												strcat(menuLcdTempString, " ");
											}
											ui.writeLcdLine(4,menuLcdTempString);

										}
										catch(std::exception &e)
										{
											cout << "comboUtilSelect=0, LCD menuLevel 2: " << e.what() << endl;

										}
										break;
									default:;
									}
									rotEncOutput = 0;
									uiChange  = false;
								}
								else if(rotEncOutput != 0 && comboUtilSelect == 1) // read encoder
								{

										if(menuLevel == 2)
										{
											try
											{
												flxUtil.changeFlxUtilityValue(menuLevelIndex[0], menuLevelIndex[1],rotEncOutput);

												string typeName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]);
												string paramName = flxUtil.getFlxUtilityParamName(menuLevelIndex[0], menuLevelIndex[1]);
												snprintf(menuLcdTempString,20,"%s %f       ", paramName.c_str(),
															flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]));
												ui.writeLcdLine(2,menuLcdTempString);
													// send "util value change" command to OfxMain via FIFO
												string typeParamName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]) + "_"
															+ flxUtil.getFlxUtilityParamName(menuLevelIndex[0],menuLevelIndex[1]);
												mainInt.changeFlxUtilityValue(typeParamName,
															flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]));
												clearBuffer(menuLcdTempString,21);
												strcpy(menuLcdTempString, " ");
												for(int utilParamIndex = 0; utilParamIndex < utilParamNames.size(); utilParamIndex++)
												{
													strncat(menuLcdTempString, utilParamNames[utilParamIndex].c_str(), 4);
													strcat(menuLcdTempString, " ");
												}
												ui.writeLcdLine(4,menuLcdTempString);
											}
											catch(std::exception &e)
											{
												cout << "comboUtilSelect=0, LCD menuLevel 3 rotary: " << e.what() << endl;

											}
										}
										uiChange  = false;

										rotEncOutput = 0;

								}

							}


					}
				}
				else if(noCombos == false)
				{
					ui.writeLcdLine(1,"Connected to :");
					ui.writeLcdLine(2,"Host          ");
					noCombos = true;
				}
			}// blank
		}// keep power on
	}
	char emptyLine[20];

	return status;
}


int openUsbPort()
{
	char gadget[10] = "";
	int osSelected = 0;
	int buttonPushed = -1;

	if(gadget[0] != 'g')
	{

		FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
		while((fgets(gadget,5,gadgetDetect)) != NULL);
		pclose(gadgetDetect);
	}

	if(gadget[0] != 'g')
	{

		FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
		while((fgets(gadget,5,gadgetDetect)) != NULL);
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
				buttonPushed = ui.readButtons();
				int osKeys = buttonPushed;
				{
					if(osKeys > 3) osKeys -= 2;
					else osKeys--;

					switch(osKeys)
					{
					case 0:		// Windows selected
						osSelected = 1;
						system("/root/AcmRndisUsbInit");
						break;
					case 1:		// Mac OS X selected
						osSelected = 2;
						system("/root/AcmEcmUsbInit");
						break;
					case 2:		// Linux selected
						osSelected = 3;
						system("/root/AcmEcmUsbInit");
						break;
					default:;
					}
				}
			}
		}
	}

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

#if(dbg >= 1)
	cout << "********** EXITING FlxPedalUi:main::isUsbConnected: " << status << endl;
#endif

	return status;
}
