#include "common.h"
#include "i2s_parallel.h"
#include "3d.h"
#include "cct.h"


#include "rectbmp.h"
#include "lenabmp.h"

#define TAG "main"

#define PIN_BTN			0
#define PIN_BIAS_EN		12
#define PIN_DPY_EN		27

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
	

void lcdTask(void *pvParameters) {
    i2s_parallel_buffer_desc_t bufdesc[2];
    i2s_parallel_config_t cfg = {
        .gpio_bus = {
               16, 	// 0 : d0 
					4, 	// 1 : d1
					2, 	// 2 : d2
					15, 	// 3 : d3
					13, 	// 4 : HS
					14, 	// 5 : VS
					-1,	// 6 : unused
					-1},	// 7 : unused
        .gpio_clk = 17,	// XCK

        .bits = I2S_PARALLEL_BITS_8,
        .clkspeed_hz = 2*1000*1000,//resulting pixel clock = 1MHz
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
	
  // this lcd power up sequence must be followed to avoid damage
	delayMs(50);
   i2s_parallel_setup(&I2S1, &cfg);
	delayMs(50);
	digitalWrite(PIN_BIAS_EN, 1); // enable lcd bias voltage V0 after clocks are available
	delayMs(50); 
	digitalWrite(PIN_DPY_EN, 1);  // enable lcd

	int counter = 0;
	int drawState = 0;
	
   while(1) {
      delayMs(20); 
		counter++;

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
				ESP_LOGI(TAG, "txt : %dus\r\n", elapsedUs);
				i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
				drawBufID ^= 1;
				}
         if (counter > 500) {
            drawState = 1;
            counter = 0;
            }
			break;

			case 1 :
			if ((counter % 100) == 0) {
				cct_SetMarker();
				CNFGClearScreen(0);
				uint8_t* pImg = (counter/100)&1 ? (uint8_t*)lenaBitmap : (uint8_t*)rectBitmap;
				CNFGLoadBitmap(pImg);
				uint32_t elapsedUs = cct_ElapsedTimeUs();
				ESP_LOGI(TAG, "bmp : %dus\r\n", elapsedUs);
				i2s_parallel_flip_to_buffer(&I2S1, drawBufID);
				drawBufID ^= 1;
				}
         if (counter > 500) {
            drawState = 2;
            counter = 0;
            }
			break;

			case 2 :
			cct_SetMarker();
			DrawSphere(counter%240);
			uint32_t elapsedUs = cct_ElapsedTimeUs();
			//ESP_LOGI(TAG,"sph : %dus\r\n", elapsedUs);
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
   pinMode(PIN_DPY_EN, OUTPUT);
   digitalWrite(PIN_DPY_EN, 0);
   pinMode(PIN_BIAS_EN, OUTPUT);
   digitalWrite(PIN_BIAS_EN, 0);
   
   pinMode(PIN_BTN,INPUT_PULLUP);

	xTaskCreatePinnedToCore(&lcdTask, "lcdTask", 2048, NULL, 20, NULL, 1);
   }



