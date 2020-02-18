# ESP32-LCD-I2S
This is an esp-idf project that demonstrates use of the Espressif ESP32 I2S peripheral to drive a controller-less 240 x 160 monochrome lcd with 4bit data, clock, hsync and vsync, without using cpu cycles.

Built on Ubuntu 16.04LTS amdx64 platform with esp-idf commit 84788230392d0918d3add78d9ccf8c2bb7de3152,  2018 March 21.

## Notes
The LCD has internal memory only for one line of pixels. It requires 4 1-bit horizontal pixels per clock, a horizontal sync pulse (HS) to latch a line of pixels, a one line wide vertical sync pulse (VS) per frame, and a frame toggle signal (FR) that alternates level every frame.

To determine the required signalling on the bus interface, a logic analyzer was used to capture the bus signals with the LCD being driven by a PIC24FJ256DA206 microcontroller with built-in graphics controller (i.e. known working interface). Some screen captures of the bus signals are in the /docs directory.

We use the I2S peripheral in parallel LCD mode with 8-bit bus width. The physical data bus is actually 6bits wide, and encodes 4bits of monochrome pixels plus the HS and VS signals. The I2S peripheral also generates the pixel clock.  

External hardware generates the FR signal from the VS signal. We use an edge-triggered D latch (74LVC1G80) with feedback from the Q' output to the D input. The VS signal is connected to the clock pulse input of the latch, and the FR signal is taken from the Q' output.

The software driver has a double-buffered frame option ((#define DOUBLE_BUFFERED in config.h). If enabled we double-buffer a frame of pixels in ESP32 RAM (**display** and **draw** frame buffers). The I2S peripheral continuously reads out the **display** frame buffer without CPU intervention. The application software updates the **draw** frame buffer, and can swap this with the display buffer at any time. On receiving the buffer swap signal, the driver will continue readout from the  display buffer until the end of frame and then swap the display and draw buffers. This avoids visible artifacts from slow drawing. There are two demo videos in the /docs directory - one using single buffer (draw and display from one buffer), and one with double-buffering. You will see the benefit of double-buffering with the 3D graphics animation - it does not have any flickering or ghosting artifacts.

The ESP32 I2S peripheral has a 32bit internal bus. Unfortunately, in 8bit mode, bytes have to be sent to the peripheral in the order 2,3,0,1 so that they appear at the external I2S bus interface in the order 0,1,2,3. So this has to be taken into account when drawing into the frame buffer and encoding the HS and VS signals.

Another issue is that (as seen in the logic analyzer screen captures of the known working interface) the LCD expects a short (< 1/2 clock period) +ve HS pulse along with the last 4bit packet for the line, and latches the line on the HS pulse falling edge. But the I2S parallel bus encoded HS pulse is 1 clock wide, synchronous to the clock edge. The LCD ignores the data in all packets with HS = 1 through the full clock period, and latches the previous buffered data packets when HS returns to 0. So the workaround is to generate 4 additional dummy packets with HS encoded as 1, after transmitting a full row of pixels with HS=0. The reason for sending not just one, but four additional I2S packets = 4 bytes = 32bits is because we need to be in I2S peripheral byte order sync for the next row.  


## Credits
3D graphics code from https://github.com/cnlohr/channel3 modified for our purpose.	

Sample I2S parallel code and specific information on the 8bit mode from postings by Espressif contributor ESP_Sprite in these threads on the ESP32 forum : 

https://www.esp32.com/viewtopic.php?f=17&t=3188 

https://www.esp32.com/viewtopic.php?f=13&t=3256.



