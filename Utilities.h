/*
 * Utilities.h
 *
 *  Created on: Jul 17, 2017
 *      Author: buildrooteclipse
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <algorithm>
#include <linux/types.h>
#include <signal.h>
#include <json/json.h>



void clearBuffer(char *buffer, int bufferSize);

void pabort(const char *s);


#endif /* UTILITIES_H_ */
