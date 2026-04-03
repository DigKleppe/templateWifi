/*

 . /home/dig/esp/esp-idf/export.sh
 idf.py monitor -p /dev/ttyUSB2./export

 -s ${openocd_path}/share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f target/esp32.cfg -c "program_esp
 /mnt/linuxData/projecten/git/thermostaat/SensirionSCD30/build//app.bin 0x10000 verify"

 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "esp_freertos_hooks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "ledTask.h"
#include "main.h"
#include "p1parser.h"
#include "scripts.h"
#include "settings.h"
#include "wifiConnect.h"

static const char *TAG = "main";

esp_err_t init_spiffs(void);
extern bool settingsChanged; // from settingsScreen
uint32_t stackWm[5];
uint32_t upTime;
uint32_t timeStamp = 1; // global timestamp for logging

void uartRxTask(void *arg);
void uartTxTask(void *arg);

extern const char _3PhaseSimData[];

extern "C" {
void app_main() {
	esp_err_t err;
	TickType_t xLastWakeTime;

	startLEDs();

	esp_rom_gpio_pad_select_gpio(LED_PIN);
	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
	gpio_set_drive_capability(LED_PIN, GPIO_DRIVE_CAP_0);
	gpio_set_level(LED_PIN, 0);

	esp_rom_gpio_pad_select_gpio(LED_INV_PIN);
	gpio_set_direction(LED_INV_PIN, GPIO_MODE_OUTPUT);
	gpio_set_drive_capability(LED_INV_PIN, GPIO_DRIVE_CAP_0);
	gpio_set_level(LED_PIN, 1);

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	ESP_LOGI(TAG, "\n **************** start *****************\n");

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		ESP_LOGI(TAG, "nvs flash erased");
	}
	ESP_ERROR_CHECK(err);

	ESP_ERROR_CHECK(init_spiffs());

	err = loadSettings();
	getSensorNameScript(NULL, 0); // pull in  scripts

	wifiConnect();

	// do {
	// 	toggle = !toggle;
	// 	gpio_set_level(LED_PIN, toggle);
	// 	gpio_set_level(LED_INV_PIN, !toggle);
	// 	vTaskDelay(500 / portTICK_PERIOD_MS);
	// } while (connectStatus != IP_RECEIVED);

	gpio_set_level(LED_PIN, 0);
	gpio_set_level(LED_INV_PIN, 1);

#ifndef SIMULATE
	xTaskCreate(uartRxTask, "uartRxTask", 1024 * 3, NULL, configMAX_PRIORITIES - 1, NULL);
//	xTaskCreate(uartTxTask, "uartTxTask", 1024, NULL, configMAX_PRIORITIES-1, NULL);  // test only
#endif

	xLastWakeTime = xTaskGetTickCount();

	while (1) {
		vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_PERIOD_MS);
		upTime++;
		timeStamp++;

#ifdef SIMULATE
		parseP1data((char *)_3PhaseSimData, strlen(_3PhaseSimData));
#endif
		switch ((int)connectStatus) {
		case CONNECTING:
			D1color = COLOR_RED;
			break;

		case WPS_ACTIVE:
			D1Flash = true;
			D1color = COLOR_BLUE;
			break;

		case CONNECTED:
			D1color = COLOR_YELLOW;
			break;

		case CONNECT_READY:
		case IP_RECEIVED:
			D1Flash = false;
			D1color = COLOR_GREEN;
			break;
		
		case CONNECT_AP:
			D1color = COLOR_GREEN;
			D1Flash = true;
			break;
		break;

		case CONNECT_READY_AP:
			D1color = COLOR_CYANE;
			D1Flash = false;
			break;

		default:
			break;
		}
	}
}
}
