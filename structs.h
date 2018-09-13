/*
 * structs.h
 *
 *  Created on: Jul 14, 2017
 *      Author: buildrooteclipse
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

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

using namespace std;

struct SoftKeyElement{
	string name;
	string abbr;
	int index;
};

struct PedalStatus{
	int globalComboIndex;
	int hostGuiActive;
	char currentStatus[20];
};

#endif /* STRUCTS_H_ */
