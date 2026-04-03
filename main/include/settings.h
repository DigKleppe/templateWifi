/*
 * settings.h
 *
 *  Created on: Nov 30, 2017
 *      Author: dig
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdbool.h>
#include "esp_system.h"
#include <time.h>
#include <sys/time.h>

#include "cgiScripts.h"

#define MAX_STRLEN				 32
#define USERSETTINGS_CHECKSTR "test"
#define ADVUSERSETTINGS_CHECKSTR "test3"
#define SYSTEMINFO_CHECKSTR "test"


typedef struct {
	char moduleName[MAX_STRLEN+1];
	char checkstr[MAX_STRLEN+1];
}userSettings_t;

typedef struct {
	int fixedIPdigit;
	char checkstr[MAX_STRLEN + 1];
} advancedSettings_t;

typedef struct {
	uint32_t startUps;
	uint32_t spare1;
	uint32_t spare2;
	uint32_t spare3;
	uint32_t spare4;
	char checkstr[MAX_STRLEN + 1];
}systemInfo_t;

typedef struct {
	varType_t varType;
	int size;
	void * pValue;
	int minValue;
	int maxValue;
} settingsDescr_t;

extern userSettings_t userSettings;
extern advancedSettings_t advSettings;
extern systemInfo_t systemInfo;

extern settingsDescr_t settingsDescr[];
extern bool settingsChanged;

#ifdef __cplusplus
extern "C" {
#endif
	esp_err_t saveSettings( void);
	esp_err_t loadSettings( void);
	void setUserDefaults(void);
	void setAdvDefaults(void);
#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_H_ */
