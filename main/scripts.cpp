/*
 * dummyscripts.cpp
 *
 *  Created on: Oct 27, 2023
 *      Author: dig
 */


// called from CGI
#include <stdio.h>
#include <string.h>

#include "settings.h"
#include "scripts.h"
#include "p1parser.h"


extern int scriptState;

int getSensorNameScript(char *pBuffer, int count) {
	if ( pBuffer == NULL)
		return 0;

	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len += sprintf(pBuffer + len, "Actueel,Nieuw\n");
		len += sprintf(pBuffer + len, "%s\n", userSettings.moduleName);
		return len;
		break;
	default:
		break;
	}
	return 0;
}

int getInfoValuesScript(char *pBuffer, int count) {
	switch (scriptState) {
	case 0:
		scriptState++;
		strcpy(pBuffer,"Onderdeel,Waarde\r");
		strcat (pBuffer,p1OutBuffer);
		return strlen(pBuffer);
		break;
	default:
		break;
	}
	return 0;
}



// @formatter:off
char tempName[MAX_STRLEN];

const CGIdesc_t writeVarDescriptors[] = {
//		{ "Temperatuur", &calValues.temperature, FLT, NR_NTCS },
//		{ "moduleName",tempName, STR, 1 }
};

