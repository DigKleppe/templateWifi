/*
 * Log.cpp
 *
 *  Created on: Mar 11, 2026
 *      Author: dig
 */
#include "Log.h"
#include <cstdint>
#include <cstdlib>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "Log";

Log::Log(int _maxLogValues, int _logTypeSize) {
	maxValues = _maxLogValues;
	logTypeSize = _logTypeSize;

#ifdef CONFIG_SPIRAM
	pLogBuffer =(uint8_t *)  heap_caps_malloc((maxValues * _logTypeSize), MALLOC_CAP_SPIRAM);
#else
	pLogBuffer = (uint8_t *)malloc(maxValues * logTypeSize);
#endif
	if (pLogBuffer == 0)
		// ESP_LOGE(TAG, "No space for Logbuffer!");
		printf("No space for Logbuffer!");
	else {
		memset(pLogBuffer, 0, (maxValues * logTypeSize));
	}
	logRxIdx = 0;
	logTxIdx = 0;
	actValues = 0;
}

// esp_err_t Log::add(log_t logValue) {
int Log::add(void *logValue) {
	if (pLogBuffer == 0)
		return -1; //  ESP_ERR_NO_MEM;
	memcpy(&pLogBuffer[logTxIdx], logValue, logTypeSize);
	logTxIdx += logTypeSize;

	if (logTxIdx >= maxValues * logTypeSize)
		logTxIdx = 0;
	if (actValues < maxValues)
		actValues++;
	return 0; //  ESP_OK;
}

void Log::clear() {
	if (pLogBuffer == 0)
		return;
	logRxIdx = 0;
	logTxIdx = 0;
	actValues = 0;
	memset(pLogBuffer, 0, (maxValues * logTypeSize));
}

// search oldest log in  cyclic buffer
// returns nr of logs to send (read)

int Log::getNrLogsToSend() {
	if (pLogBuffer == 0)
		return -1;

	if (actValues < maxValues) // buffer filled from 0
		logRxIdx = 0;
	else
		logRxIdx = logTxIdx; // cyclic buffer , logTxidx points to oldest value

	ESP_LOGI(TAG, "logRxidx:%d , actvalues: %d", logRxIdx, actValues);

	return actValues;
}

void *Log::readNext() {
	void *p = &pLogBuffer[logRxIdx];
	logRxIdx += logTypeSize;
	if (logRxIdx >= maxValues * logTypeSize)
		logRxIdx = 0;
	return p;
}

