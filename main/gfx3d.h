#ifndef _3D_H_
#define _3D_H_

// graphics for controller-less monochrome LCD display with 4bit data bus,
// hsync, vsync and fr signals.  The fr signal is generated externally
// in hw from the vsync signal. So we need to encode the 4bit data + hsync + vsync
// on a 6bit wide parallel bus to the LCD. For this we use the ESP32 i2s bus
// in parallel LCD 8bit mode.

#define BIT_HS	((uint8_t)(1<<4))
#define BIT_VS	((uint8_t)(1<<5))

#define NUM_COLS 		240
#define NUM_ROWS 		160
#define NUM_ROW_BYTES 	((NUM_COLS/4) + 4) // see CNFGClearScreen 

#define FRAME_SIZE (NUM_ROW_BYTES * NUM_ROWS)

#ifdef DOUBLE_BUFFERED
   extern int DrawBufID; // drawable buffer index, the other buffer is being displayed
   extern uint8_t FrameBuffer[2][FRAME_SIZE];
#else
   extern uint8_t FrameBuffer[FRAME_SIZE];
#endif

extern int16_t ProjectionMatrix[16];
extern int16_t ModelviewMatrix[16];
extern int CNFGPenX, CNFGPenY;
extern uint8_t CNFGBGColor;
extern uint8_t CNFGLastColor;

void CNFGTackSegment( int x0, int y0, int x1, int y1 );
int LABS( int x );
void (*CNFGTackPixel)( int x, int y ); //Unsafe plot pixel.
void LocalToScreenspace( int16_t * coords_3v, int16_t * o1, int16_t * o2 );
int16_t tdSIN( uint8_t iv );
int16_t tdCOS( uint8_t iv );

//  0 	: White
//  1   : Black
void CNFGColor( uint8_t col ); 
void CNFGClearScreen(uint8_t pattern);
void CNFGLoadBitmap(uint8_t* pImg);

void  tdTranslate( int16_t * f, int16_t x, int16_t y, int16_t z );		//Operates ON f
void  tdScale( int16_t * f, int16_t x, int16_t y, int16_t z );			//Operates ON f
void  tdRotateEA( int16_t * f, int16_t x, int16_t y, int16_t z );		//Operates ON f

void  CNFGDrawText( const char * text, int scale );
void  CNFGDrawBar(  int x1, int y1, int x2, int y2 );
void  CNFGTackRectangle( short x1, short y1, short x2, short y2 );
void  tdMultiply( int16_t * fin1, int16_t * fin2, int16_t * fout );
void  tdPTransform( int16_t * pin, int16_t * f, int16_t * pout );
void  td4Transform( int16_t * pin, int16_t * f, int16_t * pout );
void  MakeTranslate( int x, int y, int z, int16_t * out );
void  Perspective( int fovx, int aspect, int zNear, int zFar, int16_t * out );
void  tdIdentity( int16_t * matrix );
void  MakeYRotationMatrix( uint8_t angle, int16_t * f );
void  MakeXRotationMatrix( uint8_t angle, int16_t * f );
void  DrawGeoSphere();
void  Draw3DSegment( int16_t * c1, int16_t * c2 );
void CNFGClearLine(uint8_t pattern, int ln);

int16_t  tdPerlin2D( int16_t x, int16_t y );
int16_t  tdFLerp( int16_t a, int16_t b, int16_t t );
int16_t  tdNoiseAt( int16_t x, int16_t y );

void gfx_putChar(uint8_t ch);
void gfx_putCharX2(uint8_t ch );
void gfx_printSz(int row, int c,  char* sz);
void gfx_printSzX2(int row, int c,  char* sz);
void gfx_printf(int row, int c, char* format, ...);
void gfx_printlnf(int row, const char* format, ...);
void gfx_putLNum(char ch);
void gfx_printSzLNum(int row, int col, char* sz);

#endif



