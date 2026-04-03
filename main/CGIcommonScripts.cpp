#include "cgiScripts.h"
#include "settings.h"
#include "softwareVersions.h"
#include "wifiConnect.h"

#include "esp_netif_ip_addr.h"

#include <string.h>

#include "esp_log.h"

static const char *TAG = "commonscripts";

extern int switches;

#include "esp_log.h"
extern int scriptState;
extern int resetCause;
extern int pingFailedCntr;
static int rssi;

const char firmWareVersion[] = FIRMWARE_VERSION;

const CGIdesc_t writeVarDescriptorTable[] = {
	//	{"Badkamerventilatie nalooptijd (min)", &userSettings.bathRoomFanTime, INT, 1},
	{NULL, NULL, INT, 1},
};

const CGIdesc_t advancedWriteVarDescriptorTable[] = {
	{"Laaste IPdigit", &advSettings.fixedIPdigit, INT, 1},
	{NULL, NULL, INT, 1},
};


// const CGIdesc_t advancedWriteVarDescriptorTable[] = {
// 	{"Laaste IPdigit", &advSettings.fixedIPdigit, INT, 1},
// 	{NULL, NULL, INT, 1},
// };

const CGIdesc_t commonInfoTable[] = {{"Firmwareversie", (void *)firmWareVersion, STR, 1},
									 {"RSSI", (void *)&rssi, INT, 1},
							//		 {"Opstarts", &systemInfo.startUps, INT, 1},
									 // {"PingTimeouts",&systemInfo.pingTimeOuts, INT, 1 },
									 // {"PingFailed",&pingFailedCntr, INT, 1 },
									 // {"SensorTimeouts",&systemInfo.sensorTimeOuts, INT, 1 },
									 // {"ConnectRetries",&connectRetries, INT, 1 },
									 // {"Disconnects",&disconnects, INT, 1 },
									 // {"ResetOorzaak", &resetCause, INT, 1},
									 {NULL, NULL, INT, 0}};

int getCGItable(const CGIdesc_t *descr, char *pBuffer, int count) {
	int len = 0;
	do {
		len += sprintf(pBuffer + len, "%s,", descr->name);
		switch (descr->type) {
		case INT:
			len += sprintf(pBuffer + len, "%d\n", *(int *)descr->pValue);
			break;
		case FLT:
			len += sprintf(pBuffer + len, "%1.2f\n", *(float *)descr->pValue);
			break;
		case STR:
			len += sprintf(pBuffer + len, "%s\n", (char *)descr->pValue);
			break;

		case IPADDR: {
			esp_ip4_addr_t addr = (esp_ip4_addr_t) * (uint32_t *)descr->pValue;
			len += sprintf(pBuffer + len, IPSTR, IP2STR(&addr));
			len += sprintf(pBuffer + len, "\n");
		} break;

		default:
			break;
		}
		descr++;
	} while (descr->nrValues >0);
	return len;
}

int getSettingsScript(char *pBuffer, int count) {
	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len = sprintf(pBuffer, "Parameter, Waarde,Stel in\n");
		len += getCGItable(writeVarDescriptorTable, pBuffer + len, count);
		return len;
		break;
	default:
		break;
	}
	return 0;
}

int getAdvSettingsScript(char *pBuffer, int count) {
	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len = sprintf(pBuffer, "Parameter, Waarde,Stel in\n");
		len += getCGItable(advancedWriteVarDescriptorTable, pBuffer + len, count);
		return len;
		break;
	default:
		break;
	}
	return 0;
}

int getCommonInfoScript(char *pBuffer, int count) {
	int len = 0;
	switch (scriptState) {
	case 0:
		scriptState++;
		len = sprintf(pBuffer, "Parameter, Waarde\n");
		rssi = getRssi();
		len += getCGItable(commonInfoTable, pBuffer + len, count);
		return len;
		break;
	default:
		break;
	}
	return 0;
}

int saveSettingsScript(char *pBuffer, int count) {
	saveSettings();
	return 0;
}

int cancelSettingsScript(char *pBuffer, int count) {
	loadSettings();
	return 0;
}

int setUserDefaultsScript(char *pBuffer, int count) {
	setUserDefaults();
	return 0;
}

int setAdvUserDefaultsScript(char *pBuffer, int count) {
	setAdvDefaults();
	return 0;
}

// int checkUpdatesScript(char *pBuffer, int count) {
// 	forceUpdate = true;
// 	return 0;
// }

int forgetWifiScript(char *pBuffer, int count) {
	strcpy(wifiSettings.SSID, ESP_WIFI_SSID); // Asus test router
	strcpy(wifiSettings.pwd, ESP_WIFI_PASS);
	wifiSettings.gw = (esp_ip4_addr_t)0;
	saveSettings();
	esp_restart();
	return 0;
}

void parseCGIWriteData(char *buf, int received) {
	bool save = false;
	if ( buf == NULL)
		return;	
	if (strncmp(buf, "setVal:", 7) == 0) { // values are written , in sensirionTasks write these to SCD30
		if (readActionScript(&buf[7], writeVarDescriptorTable) >= 0) {
			save = true;
		}
		if (readActionScript(&buf[7], advancedWriteVarDescriptorTable) >= 0) {
			save = true;
		}
	}
	if (save)
		saveSettings();
}
