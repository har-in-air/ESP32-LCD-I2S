#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "i2s_parallel.h"
#include "3d.h"
#include "cct.h"


#define delayMs(ms) vTaskDelay(ms / portTICK_PERIOD_MS)


#include "rectbmp.h"
#include "lenabmp.h"

int 	drawBufID;
uint8_t frameBuffer[2][FRAME_SIZE];

	
#define PIN_BTN			0
#define PIN_LED			23
#define PIN_BIAS_EN		22
#define PIN_DPY_EN		21


void lcdTask(void *pvParameters) {
			
    i2s_parallel_buffer_desc_t bufdesc[2];
    i2s_parallel_config_t cfg = {
        .gpio_bus = {15, 	// 0 : d0 
					2, 		// 1 : d1
					4, 		// 2 : d2
					16, 	// 3 : d3
					17, 	// 4 : HS
					5, 		// 5 : VS
					-1,		// 6 : no connection
					-1},	// 7 : no connection
        .gpio_clk = 19, 	// XCK
        .bits = I2S_PARALLEL_BITS_8,
        .clkspeed_hz = 2*1000*1000,
        .bufa = &bufdesc[0],
        .bufb = &bufdesc[1]
    };

    bufdesc[0].memory = frameBuffer[0];
    bufdesc[0].size = FRAME_SIZE;

    bufdesc[1].memory = frameBuffer[1]; 
    bufdesc[1].size = FRAME_SIZE;
	// make sure both front and back buffers have encoded syncs
    drawBufID = 0; 
	CNFGClearScreen(0);
    drawBufID = 1; 
	CNFGClearScreen(0);
	
	gpio_set_level(PIN_BIAS_EN, 1); // enable lcd bias voltage V0
	delayMs(50);
	gpio_set_level(PIN_DPY_EN, 1);  // enable lcd
	delayMs(50); 
    i2s_parallel_setup(&I2S1, &cfg);
	
	CNFGLoadBitmap((uint8_t*)lenaBitmap);
	printf("Loaded bitmap into bufID 1\r\n");
        i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
        drawBufID ^= 1;
	CNFGClearScreen(0);
	CNFGColor(1);
	CNFGPenX = 10;
	CNFGPenY = 10;
	CNFGDrawText("12:56PM", 10 );
	
	
	
    while(1) {
		delayMs(5000);
        i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
        drawBufID ^= 1;
		
		}
	}
	
	
void app_main(){
	nvs_flash_init();
    gpio_set_direction(PIN_LED, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(PIN_DPY_EN, GPIO_MODE_DEF_OUTPUT);
	gpio_set_level(PIN_DPY_EN, 0);

    gpio_set_direction(PIN_BIAS_EN, GPIO_MODE_DEF_OUTPUT);
	gpio_set_level(PIN_BIAS_EN, 0);

    gpio_set_direction(PIN_BTN, GPIO_MODE_DEF_INPUT);
    gpio_pullup_en(PIN_BTN);

	xTaskCreatePinnedToCore(&lcdTask, "lcdTask", 2048, NULL, 20, NULL, 1);
}



