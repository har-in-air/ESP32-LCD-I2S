# ESP32-LCD-I2S
This is an esp-idf project that demonstrates use of the Espressif ESP32 I2S peripheral to drive a controller-less 240 x 160 monochrome lcd with 4bit data, clock, hsync and vsync, without cpu intervention.

A demo video of the working lcd interface is available in the \docs directory.

## Features
The LCD has internal memory only for one line of pixels. It requires 4 1-bit horizontal pixels per clock, a horizontal sync pulse (HS) to latch a line of pixels, a one line wide vertical sync pulse (VS) per frame, and a frame toggle signal (FR) that alternates level every frame.

We use the I2S peripheral in parallel LCD mode with 8-bit bus width. The physical data bus is actually 6bits wide, and encodes 4bits of monochrome pixels plus the HS and VS signals. The I2S peripheral also generates the pixel clock.  

External hardware generates the FR signal from the VS signal. We use an edge-triggered D latch (74LVC1G80) with feedback from the Q' output to the D input. The VS signal is connected to the clock pulse input of the latch, and the FR signal is taken from the Q' output.

We double-buffer a frame of pixels in ESP32 RAM. The I2S peripheral continuously reads out the display frame buffer without CPU intervention. The application software updates the draw frame buffer, and can flip this at any time. This avoids artifacts in the LCD display from partial updates. 


## Credits
3D graphics code from https://github.com/cnlohr/channel3 modified for our purpose.	

Sample I2S parallel code and specific information on the 8bit mode from postings by Espressif contributor ESP_Sprite in these threads on the ESP32 forum : 

https://www.esp32.com/viewtopic.php?f=17&t=3188 

https://www.esp32.com/viewtopic.php?f=13&t=3256.



