#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdbool.h>

#include "esp_log.h"

#include "ledTask.h"
#include "led_strip.h"
#include "wifiConnect.h"

static const char *TAG = "ledTask";

#define LED_TYPE LED_MODEL_WS2812
#define LED_GPIO1 GPIO_NUM_48 // on s3 board   47 // on s3 mini board
#define LED_GPIO2 GPIO_NUM_11 // on keyledprint

#define BRIGHTNESS 20

static const rgb_t colors[] = {
	{.r = 0x0f, .g = 0x0f, .b = 0x0f}, // wit
	{.r = 0x00, .g = 0x00, .b = 0x2f}, // blauw
	{.r = 0x00, .g = 0x2f, .b = 0x00}, // groen
	{.r = 0x2f, .g = 0x00, .b = 0x00}, // rood
	{.r = 0x2f, .g = 0x2f, .b = 0x00}, // geel
	{.r = 0x2f, .g = 0x00, .b = 0x2f}, // paars
	{.r = 0x00, .g = 0x2f, .b = 0x2f}, // cyaan
	{.r = 0x00, .g = 0x00, .b = 0x00}, // uit
};

static const rgb_t colorsOnboardLed[] = {
	{.r = 0x0f, .g = 0x0f, .b = 0x0f}, // wit
	{.r = 0x00, .g = 0x00, .b = 0x2f}, // blauw
	{.r = 0x2f, .g = 0x00, .b = 0x00}, // rood
	{.r = 0x00, .g = 0x2f, .b = 0x00}, // groen
	{.r = 0x2f, .g = 0x2f, .b = 0x00}, // geel
	{.r = 0x2f, .g = 0x00, .b = 0x2f}, // paars
	{.r = 0x00, .g = 0x2f, .b = 0x2f}, // cyaan
	{.r = 0x00, .g = 0x00, .b = 0x00}, // uit
};

LEDcolor_t D1color, D2color; // leds on ledprint
bool D1Flash, D2Flash;
int D2nrFlashes;

// S3 wemos mini on board LED
// https://www.wemos.cc/en/latest/_static/files/sch_s3_mini_v1.0.0.pdf



esp_err_t led_strip_set_pixelColor(led_strip_handle_t strip, uint32_t index, rgb_t color) {
    return led_strip_set_pixel(strip, index, color.r , color.g, color.b);
}


void LEDtask(void *pvParameters) {
	int c = 0;
	int flashes = 0;
	int flashPauze = 0;

	static led_strip_handle_t strip;
	/* LED strip initialization with the GPIO and pixels number*/

	led_strip_config_t strip_config = {
		.strip_gpio_num = LED_GPIO1, // on s3 board
		.max_leds = 2,
		.led_model = LED_TYPE,										 // set the LED strip model
		.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // set the color component format
		.flags = {false},											 // set to 1 if the output signal need to be inverted
	};
	led_strip_rmt_config_t rmt_config = {
		.clk_src = RMT_CLK_SRC_DEFAULT,	   // set the RMT clock source, if not set, default to APB_CLK
		.resolution_hz = 10 * 1000 * 1000, // 10MHz
		.mem_block_symbols = 64,		   // set the number of symbols that can be stored in one RMT channel, if not set, default to 48
		.flags = {false},
	};

	ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
	/* Set all LED off to clear all pixels */
	led_strip_clear(strip);

	D1color = COLOR_GREEN;
	D2color = COLOR_GREEN;

	while (1) {
		led_strip_set_pixelColor(strip, 0, colors[D1color]);

		if (flashPauze == 0)
			led_strip_set_pixelColor(strip, 1, colors[D2color]);

		led_strip_refresh(strip);
		vTaskDelay(pdMS_TO_TICKS(300));

		if (D1Flash) {
			led_strip_set_pixelColor(strip, 0, colors[COLOR_OFF]);
		}
		if (D2Flash) {
			led_strip_set_pixelColor(strip, 1, colors[COLOR_OFF]);
		}

		if ((flashPauze == 0) && (D2nrFlashes > 0)) {
			if (flashes < 0)
				flashes = D2nrFlashes;
			if (flashes > 0) {
				led_strip_set_pixelColor(strip, 1, colors[COLOR_OFF]);
				flashes--;
			}
			if (flashes == 0) { // last flash given, pause
				flashPauze = 4;
				flashes--;
			}
		}
		if (flashPauze > 0)
			flashPauze--;

		led_strip_refresh(strip);

		vTaskDelay(pdMS_TO_TICKS(300));
	}
}

void startLEDs() {
	xTaskCreate(LEDtask, "LEDtask", 3000, NULL, tskIDLE_PRIORITY, NULL);
}