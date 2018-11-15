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
#include "config.h"
#include "UserInterface2.h"
#include "MainInterface.h"
#include "Combo.h"
#include "Utilities.h"
#include "FlxUtility.h"



using namespace std;

static void signal_handler(int sig)
{
	cout << "signal received: " << sig <<", OfxPedal exiting ..." << endl;
	signal(sig, SIG_DFL);
	kill(getpid(),sig);
}




#define dbg 1
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

	Combo combo;
	FlxUtility flxUtil;
	MainInterface mainInt;
	int menuLevelIndex[4];  // replacing effectIndex and paramIndex, so can be used with utility.
	PedalStatus pedalStatus;
	UserInterface2 ui;

	int status = 0;
	int menuLevel = 0;
	/* MENU LEVELS:
	 * 0: Main menu
	 * 1: Combo menu (effect softkeys) or Utility menu (Utility type softkeys)
	 * 2: Combo menu (parameter softkeys) or Utility menu (parameter softkeys). Rotary encoder enabled.
	 * 3: Utility menu (parameter options softkeys).
	 */

	bool exit = false;
	bool uiChange = true;
	menuLevelIndex[0] = 0;
	menuLevelIndex[1] = 0;
	int buttonPushed = -1;
	bool mainUsbConnected = false;

	int rotEncOutput = 0;
	int rotEncReadCount = 0;
	int poweroffButtonPushedCount = 0;


	 int softKeyGroupIndex[2] = {0,0}; // 0= combo/utility parameter softkeys, 1=utility parameter option softkeys
	 int comboIndex = 0;
	int comboUtilSelect = 0;
	PedalStatus currentStatus;

	string dataFromOfxMain;
	string nonrequestDataFromOfxMain;
	vector<string> comboNames;
	vector<string> softKeyAbbrs;
	bool combosAvailable = true;

	//char verString[20] = "";
	if(argc > 1)
	{
		cout << "input arguments:" << endl;
		for(int i = 0; i < argc; i++)
		{
			cout << i << ":" << argv[i] << endl;
		}
	}


	// initialize
	char version[6] = "";
	FILE *versionFP = popen("cat /home/version","r");
	fgets(version, 6, versionFP);
	pclose(versionFP);

	for(auto & verChar : version)
	{
		if(verChar < 46 || 57 < verChar)
		{
			verChar = 0;
		}
	}

	ui.writeLcdCentered("  ","Ephlex Audio","FLX-1","V"+string(version));
	sleep(2);


	/****************************************************************************
	 * 		Get utility parameter list from FlxMain
	 ****************************************************************************/

	flxUtil.getFlxUtilityParams(mainInt.getFlxUtilityData());

	/**************************************************************************
	 * Get combo list and send first name of first combo to FlxMain process
	 **************************************************************************/

	for(int i = 0; comboNames.empty() == true ; i++)
	{
		comboNames = mainInt.listCombos();
		usleep(100000);
	}

	if(comboNames.size() != 0)
	{
		combosAvailable = true;
		cout << "combosAvailable: " << comboNames.size() << endl;
		combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
	}
	else
	{
		combosAvailable = false;
		ui.writeLcd("No combos","available",NULL,NULL);
	}
	uiChange = true;




	//********************************** loop *********************************

	while(exit == false)
	{
		pedalStatus = mainInt.readFlxMain();
		//*********************** Power off ***********************************
		if(ui.isPowerButtonPushed() == true)
		{
			if(poweroffButtonPushedCount == 0)
			{
				ui.writeLcd("Keep power button","pressed down to"," power off","");
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
		else //*********************** UI loop *****************************************
		{
			if(poweroffButtonPushedCount > 0) // power button accidentally pushed, refresh LCD
			{
				uiChange = true;
				poweroffButtonPushedCount = 0;
			}


			//********* Connection mode: pedal (no connection) or host (USB connection)*******************************
#if(DEBUG == 0)
			if(pedalStatus.usbPortOpen == true && mainUsbConnected == false)
			{
				comboUtilSelect = 1;
				mainUsbConnected = true;
				cout << "changing pedal UI to utility mode." << endl;
				uiChange = true;

			}
			else if(pedalStatus.usbPortOpen == false && mainUsbConnected == true)
			{
				comboUtilSelect = 0;
				mainUsbConnected = false;
				cout << "changing pedal UI to regular mode." << endl;
				uiChange = true;
			}
#endif

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
#if(dbg >= 2)
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
						if(buttonPushed >= 0)
						{// begin button block
							if(buttonPushed == 0) // LCD left
							{
#if(dbg >= 2)
								cout << "LCD left pushed: " <<  endl;
#endif
								switch(menuLevel)
								{
									case 0:
										// send "get combo" command to OfxMain via FIFO
										if(comboIndex > 0)
										{
											comboIndex--;
											uiChange = true;
										}
										combo.getCombo(mainInt.getComboUiData(comboNames[comboIndex]));
										break;
									case 1:
										menuLevelIndex[0] = 0;
										menuLevel--;
										uiChange = true;
										break;
									case 2:
										if(softKeyGroupIndex[0] > 0)
										{
											softKeyGroupIndex[0]--;
										}
										else
										{
											softKeyGroupIndex[0] = 0;
											menuLevelIndex[1] = 0;
											menuLevel--;
										}
										uiChange = true;
										break;
									default:;
								}

							}
							else if(buttonPushed == 6) // LCD right
							{
								try
								{
#if(dbg >= 2)
									cout << "LCD right pushed: " << comboUtilSelect << ":" << menuLevel << ":" << comboIndex <<  endl;
#endif
									switch(menuLevel)
									{
										case 0:
											if(comboIndex+1 < static_cast<int>(comboNames.size()))
											{
												// send "get combo" command to OfxMain via FIFO
												comboIndex++;
												string nextComboName = comboNames[comboIndex];
												string nextComboJsonString = mainInt.getComboUiData(nextComboName);
												combo.getCombo(nextComboJsonString);
												uiChange = true;
											}

											break;
										case 1:
											break;
										case 2:
											// only increment if there are more params to the right
											if(4*(softKeyGroupIndex[0]+1) < combo.getEffect(menuLevelIndex[0]).getParamCount())
											{
												softKeyGroupIndex[0]++;
											}
											uiChange = true;

											break;
										default:;
									}
								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=0, LCD Right error: " << e.what() << endl;
								}
							}
							else if(buttonPushed == 3 ) // Rotary encoder switch
							{
								switch(menuLevel)
								{
									case 0:
										break;
									case 1:
										menuLevelIndex[0] = 0;
										mainInt.saveFlxUtilityData();
										menuLevel--;
										uiChange = true;
										break;
									case 2:
										softKeyGroupIndex[0] = 0;
										menuLevelIndex[1] = 0;
										menuLevel--;
										uiChange = true;
										break;
									default:;
								}
							}
							else if (buttonPushed >= 0)  // soft keys
							{
								try
								{
									int softkey = 0;
									softkey = buttonPushed;
									// take rotary encoder out of softkeys
									if(softkey > 3) softkey -= 2;
									else softkey--;
									Effect effect = combo.getEffect(menuLevelIndex[0]);
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
												uiChange = true;
											}
											else if(softkey == 2)
											{
												menuLevel = 1;
												uiChange = true;
												comboUtilSelect = 1; // change LCD to display utilities
											}
											break;
										case 1:		// effects in softkeys

											if(softkey < static_cast<int>(combo.getEffects().size()))
											{

												menuLevelIndex[0] = softkey;
												if(effect.getParamCount() > 0)
												{
													menuLevel = 2;
													uiChange = true;
												}
											}
											break;
										case 2:		// parameters in softkeys
											if((4*softKeyGroupIndex[0]+softkey) < effect.getParamCount())
											{
												menuLevelIndex[1] = 4*softKeyGroupIndex[0]+softkey;
											}
											uiChange = true;
											break;
										default:;
									}
								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=0, softkey error: " << e.what() << endl;
								}

							}

						}// end button block

						while(buttonPushed == ui.readButtons() && buttonPushed >= 0); // wait for button to be released

						// update LCD
						if(comboUtilSelect == 0 && (uiChange || (menuLevel == 2 && rotEncOutput != 0)))
						{
							Effect effect = combo.getEffect(menuLevelIndex[0]);
							Parameter param = effect.getParameter(menuLevelIndex[1]);
							switch(menuLevel)
							{
								case 0:
									try
									{
										ui.writeLcdHeader(menuLevel,combo.getName(),"");
										ui.writeSoftKeys(0,vector<string>{"Save","Efx","Util"});
									}
									catch(std::exception &e)
									{
										cout << "comboUtilSelect=0, LCD menuLevel 0: " << e.what() << endl;
									}
									break;
								case 1:
									try
									{
										ui.writeLcdHeader(menuLevel,combo.getName(),"");
										ui.writeSoftKeys(0,combo.getEffectSoftKeyAbbrs());
									}
									catch(std::exception &e)
									{
										cout << "comboUtilSelect=0, LCD menuLevel 1: " << e.what() << endl;
									}
									break;
								case 2:
									try
									{
										if(rotEncOutput == 0)
										{
											ui.writeLcdHeader(menuLevel,combo.getName(),effect.getName());
											ui.writeLcdFxParameter(param);
											ui.writeSoftKeys(softKeyGroupIndex[0],  effect.getParamSoftKeyAbbrs());
										}
										else
										{
											combo.updateParameter(menuLevelIndex[0],menuLevelIndex[1],rotEncOutput);
											mainInt.changeComboParamValue(effect.getControlParameterPair(menuLevelIndex[1]), param.getValueIndex());
											ui.writeLcdFxParameter(param);
										}
									}
									catch(std::exception &e)
									{
										cout << "comboUtilSelect=0, LCD menuLevel 2: " << e.what() << endl;
									}
									break;
								case 3:
									try
									{
									}
									catch(std::exception &e)
									{
										cout << "comboUtilSelect=0, LCD menuLevel 3: " << e.what() << endl;
									}
									break;
								default:;
							}
							rotEncOutput = 0;
							uiChange  = false;
						}
					}
					else if(comboUtilSelect == 1)
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
									if(mainUsbConnected == false)
									{
										comboUtilSelect = 0;
									}

									menuLevel--;
									break;
								case 2:
									if(softKeyGroupIndex[0] > 0)  softKeyGroupIndex[0]--;
									else
									{
										softKeyGroupIndex[0] = 0;
										menuLevelIndex[1] = 0;

										menuLevel--;
									}
									break;
								case 3:
									if(softKeyGroupIndex[1] > 0)
									{
										softKeyGroupIndex[1]--;
									}
									else
									{
										softKeyGroupIndex[1] = 0;
										menuLevelIndex[2] = 0;
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
							cout << "LCD right pushed: " << comboUtilSelect << ":" << menuLevel << ":" << comboIndex <<  endl;
#endif
							uiChange = true;
						}
						else if(buttonPushed == 3) // Rotary encoder switch
						{
							switch(menuLevel)
							{
								case 0:
									// do nothing
									break;
								case 1:
									menuLevelIndex[0] = 0;
									mainInt.saveFlxUtilityData();
									if(mainUsbConnected == false)
									{
										comboUtilSelect = 0;
									}
									menuLevel--;
									break;
								case 2:
									softKeyGroupIndex[0] = 0;
									menuLevelIndex[1] = 0;
									menuLevel--;
									break;
								case 3:
									softKeyGroupIndex[1] = 0;
									menuLevelIndex[2] = 0;
									menuLevel--;
									break;
								default:;
							}
							uiChange = true;
						}
						else if (buttonPushed >= 0) // softkeys
						{
							try
							{
								int softkey = buttonPushed;
								if(softkey > 3) softkey -= 2;
								else softkey--;

								switch(menuLevel)
								{
									case 0:
										if(softkey == 2)
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
										if(4*softKeyGroupIndex[0]+softkey < flxUtil.getFlxUtilityParamVectorSize(menuLevelIndex[0]))
										{
											menuLevelIndex[1] = softkey;
											if(flxUtil.getFlxUtilityParamType(menuLevelIndex[0],menuLevelIndex[1]) == 0) menuLevel = 2;
											else menuLevel = 3; // display options in softkeys
										}

										break;
									case 3:  // utility parameter options may be in softkeys
										if(4*softKeyGroupIndex[1]+softkey < flxUtil.getFlxUtilityParamOptionVectorSize(menuLevelIndex[0],menuLevelIndex[1]))
										{
											menuLevelIndex[2] = softkey+1; // menuLevelIndex[2] = 0 is equivalent to rotaryEnc = 0;
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
					if(comboUtilSelect == 1 && (uiChange ||(menuLevel == 2 && rotEncOutput != 0)))
					{

						switch(menuLevel)
						{
							case 0:
								try
								{
									ui.writeLcdWithSoftKeys("Connected to","Host","",vector<string>{"","","Util",""});
								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=1, LCD menuLevel 0: " << e.what() << endl;
								}
								break;
							case 1:
								try
								{
									ui.writeLcdHeader(menuLevel,"Util","");
									ui.writeSoftKeys(0,flxUtil.getFlxUtilityTypeAbbrList());
								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=1, LCD menuLevel 1: " << e.what() << endl;
								}
								break;
							case 2:
								try
								{

										if(rotEncOutput == 0)
										{
											ui.writeLcdHeader(menuLevel,"Util",flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]));
											ui.writeLcdUtilParameter(flxUtil.getFlxUtilityUtilParameter(menuLevelIndex[0],menuLevelIndex[1]));

											ui.writeSoftKeys(0,flxUtil.getFlxUtilityParamAbbrList(menuLevelIndex[0]));
										}
										else
										{
											flxUtil.changeFlxUtilityValue(menuLevelIndex[0], menuLevelIndex[1],rotEncOutput);
											// send "util value change" command to OfxMain via FIFO
											string typeParamName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]) + "_"
															+ flxUtil.getFlxUtilityParamName(menuLevelIndex[0],menuLevelIndex[1]);
											mainInt.changeFlxUtilityValue(typeParamName,
													flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]).value);
											ui.writeLcdUtilParameter(flxUtil.getFlxUtilityUtilParameter(menuLevelIndex[0], menuLevelIndex[1]));
										}
								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=1, LCD menuLevel 2: " << e.what() << endl;
								}
								break;
							case 3:
								try
								{
										if(menuLevelIndex[2] == 0)
										{
											ui.writeLcdHeader(menuLevel,"Util",flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]));
											ui.writeLcdUtilParameter(flxUtil.getFlxUtilityUtilParameter(menuLevelIndex[0], menuLevelIndex[1]));

											ui.writeSoftKeys(0,flxUtil.getFlxUtilityParamOptionAbbrList(menuLevelIndex[0], menuLevelIndex[1]));
										}
										else
										{
											flxUtil.changeFlxUtilityValue(menuLevelIndex[0], menuLevelIndex[1],menuLevelIndex[2]);
											// send "util value change" command to OfxMain via FIFO
											string typeParamName = flxUtil.getFlxUtilityTypeName(menuLevelIndex[0]) + "_"
															+ flxUtil.getFlxUtilityParamName(menuLevelIndex[0],menuLevelIndex[1]);
											mainInt.changeFlxUtilityValue(typeParamName,
													flxUtil.getFlxUtilityParamValue(menuLevelIndex[0], menuLevelIndex[1]).option);
											ui.writeLcdUtilParameter(flxUtil.getFlxUtilityUtilParameter(menuLevelIndex[0], menuLevelIndex[1]));

										}

								}
								catch(std::exception &e)
								{
									cout << "comboUtilSelect=1, LCD menuLevel 2: " << e.what() << endl;
								}
								break;
							default:;
						}
						menuLevelIndex[2] = 0;
						rotEncOutput = 0;
						uiChange  = false;
					}
				}
			}
		}// blank
	}// keep power on

	return status;
}
