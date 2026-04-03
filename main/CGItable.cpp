/*
 * CGItable.cpp
 *
 *  Created on: Aug 5, 2024
 *      Author: dig
 */
#include "httpd.h"
#include "cgiScripts.h"
//#include "sensorTask.h"
#include "CGIcommonScripts.h"
#include "CGItable.h"
#include "scripts.h"


#include <cstddef>

const tCGI CGIurls[] = {
	{"/cgi-bin/getDayLogMeasValues", (tCGIHandler_t) readCGIvalues, (CGIresponseFileHandler_t) getDayLogScript},
	{"/cgi-bin/getHourLogMeasValues", (tCGIHandler_t) readCGIvalues, (CGIresponseFileHandler_t) getHourLogScript},
	{"/cgi-bin/getInfoValues", (tCGIHandler_t) readCGIvalues, (CGIresponseFileHandler_t) getInfoValuesScript},
	// {"/cgi-bin/getRTMeasValues", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getRTMeasValuesScript},
	// {"/cgi-bin/getSensorInfo", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getSensorInfoScript},
	// {"/cgi-bin/clearLog", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)clearLogScript},
	// {"/cgi-bin/getCommonInfo", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getCommonInfoScript},
	// {"/cgi-bin/getSettings", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getSettingsScript},
	// {"/cgi-bin/getAdvSettings", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getAdvSettingsScript},
	// {"/cgi-bin/setUserDefaults", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)setUserDefaultsScript},
	// {"/cgi-bin/setAdvUserDefaults", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)setAdvUserDefaultsScript},
	{"/cgi-bin/forgetWifi", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)forgetWifiScript},
	{NULL, NULL, NULL} // last
};
