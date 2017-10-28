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

// from https://github.com/cnlohr/channel3	
void SetupMatrix() {
	tdIdentity( ProjectionMatrix );
	tdIdentity( ModelviewMatrix );
	Perspective( 600, 250, 50, 8192, ProjectionMatrix );
	}
		
// from https://github.com/cnlohr/channel3	
void DrawSphere(int frameIndex) {	
	CNFGPenX = 0;
	CNFGPenY = 0;
	CNFGClearScreen(0);
	CNFGColor( 1 );
	tdIdentity( ModelviewMatrix );
	tdIdentity( ProjectionMatrix );
	int x = 0;
	int y = 0;
	CNFGDrawText( "Matrix-based 3D engine", 3 );
	SetupMatrix();
	tdRotateEA( ProjectionMatrix, -20, 0, 0 );
	tdRotateEA( ModelviewMatrix, frameIndex, 0, 0 );
	int sphereset = (frameIndex / 120);
	if( sphereset > 2 ) sphereset = 2;
	for( y = -sphereset; y <= sphereset; y++ ){
		for( x = -sphereset; x <= sphereset; x++ ){
			if( y == 2 ) continue;
			ModelviewMatrix[11] = 1000 + tdSIN( (x + y)*40 + frameIndex*2 );
			ModelviewMatrix[3] = 500*x;
			ModelviewMatrix[7] = 500*y+800;
			DrawGeoSphere();
			}
		}
	}
	
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
	
	delayMs(50);
    i2s_parallel_setup(&I2S1, &cfg);
	delayMs(50);
	gpio_set_level(PIN_BIAS_EN, 1); // enable lcd bias voltage V0 after clocks are available
	delayMs(50); 
	gpio_set_level(PIN_DPY_EN, 1);  // enable lcd

	int counter = 0;
	int drawState = 0;
	
    while(1) {
        delayMs(20); 
		counter++;
        gpio_set_level(PIN_LED, counter&1);

		switch (drawState) {
			case 0 :
			default : 
			if ((counter % 100) == 0) {
				cct_SetMarker();
				CNFGClearScreen(0);
				CNFGColor(1);
				CNFGPenX = rand()%12;
				CNFGPenY = rand()%60;
				char sztext[10];
				sprintf(sztext,"%02d:%02d%s",CNFGPenX, CNFGPenY, rand()&1 ? "pm" : "am");
				CNFGDrawText(sztext, 3 + (rand()%8) );
				uint32_t elapsedUs = cct_ElapsedTimeUs();
				printf("txt : %dus\r\n", elapsedUs);
				i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
				drawBufID ^= 1;
				}
			break;

			case 1 :
			if ((counter % 100) == 0) {
				cct_SetMarker();
				CNFGClearScreen(0);
				uint8_t* pImg = (counter/100)&1 ? (uint8_t*)lenaBitmap : (uint8_t*)rectBitmap;
				CNFGLoadBitmap(pImg);
				uint32_t elapsedUs = cct_ElapsedTimeUs();
				printf("bmp : %dus\r\n", elapsedUs);
				i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
				drawBufID ^= 1;
				}
			break;

			case 2 :
			cct_SetMarker();
			DrawSphere(counter%240);
			uint32_t elapsedUs = cct_ElapsedTimeUs();
			printf("sph : %dus\r\n", elapsedUs);
			i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
			drawBufID ^= 1;
			break;
			}
		
		
        if (!gpio_get_level(PIN_BTN)) {
			delayMs(50); // debounce button
			if (!gpio_get_level(PIN_BTN)) {
				drawState++;
				if (drawState > 2) drawState = 0;
				delayMs(300);
				}	 
			}
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



