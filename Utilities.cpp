/*
 * Utilities.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: buildrooteclipse
 */

#include "Utilities.h"

namespace std
{




void clearBuffer(char *buffer, int bufferSize)
{
	for(int i = 0; i < bufferSize; i++)
	{
		buffer[i] = 0;
	}
}

void pabort(const char *s)
{
	perror(s);
}

string getCompactedJSONData(Json::Value data)
{
	string reducedData;
	reducedData = data.toStyledString();
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\n'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\r'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\t'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), ' '), reducedData.end());

	return reducedData;
}
}
