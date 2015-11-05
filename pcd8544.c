#include "C:\users\student\Desktop\etc\pcd8544.h"

void LCDWrite(uint8_t dc, uint8_t data) {
	// Check if write is command or data
	if (dc)
		LCD_DC_HI;    // Data
	else
		LCD_DC_LO;    // Command
	
	// Signal LCD to start receiving byte
	LCD_SCE_LO;
	for (uint8_t i = 0; i < 8; i++) {
		// Determine if bit to send is 1 or 0
		if (data&(1<<7))
			LCD_DN_HI;
		else
			LCD_DN_LO;
		
		// Oscillate clock for next bit
		LCD_SCLK_LO;
		LCD_SCLK_HI;
		
		// Shift to next bit
		data <<= 1;
	}
	
	// Signal LCD to end receiving
	LCD_SCE_HI;
	LCD_DN_LO;
}

void gotoXY(unsigned char x, unsigned char y) {
	LCDWrite(0, 0x80 | x);  // Column.
	LCDWrite(0, 0x40 | y);  // Row.  ?
}

void LCDChar(unsigned char c) {
	// Signal to start write
	LCDWrite(LCD_D, 0x00);
	
	// Send each byte
	for (uint8_t i = 0; i < 5; i++)
		LCDWrite(LCD_D, pgm_read_byte(ascii_table+(c-0x20)*5+i));
	
	// Signal to end write
	LCDWrite(LCD_D, 0x00);
}

void setPixel(int x, int y) {
  if (x > 84 || y > 48) { return; }

  // The LCD has 6 rows, with 8 pixels per  row.
  // 'y_mod' is the row that the pixel is in.
  // 'y_pix' is the pixel in that row we want to enable/disable
  int y_mod = (int)(y >> 3);	// >>3 divides by 8
  int y_pix = (y-(y_mod << 3));// <<3 multiplies by 8
  int val = 1 << y_pix;

  // Write the updated pixel out to the LCD
  gotoXY(x,y_mod);
  LCDWrite (1,val);


  	
}

void LCDSprite(unsigned char* str) {
	// Signal to start write
	LCDWrite(LCD_D, 0x00);
	
	// Send each byte
	for (uint8_t i = 0; i < 5; i++)
		LCDWrite(LCD_D, (str[i]));
	
	// Signal to end write
	LCDWrite(LCD_D, 0x00);
}

void LCDStr(const char* str) {
	while (pgm_read_byte(str)) {	
		// Iterate through each char to send
		LCDChar(pgm_read_byte(str));
		str++;
	}
}

void LCDClear(void) {
	// Clear all pixels on LCD
	for (int i = 0; i < LCD_WIDTH*LCD_HEIGHT/8; i++)
		LCDWrite(LCD_D, 0x00);
}


void LCDInit(void) {
	
	/*// configure the LCD pins as outputs
	DDRA |= (1<<1);   THIS IS DONE
	DDRA |= (1<<0);   ON THE 
	DDRD |= (1<<2);   MAIN() 
	DDRD |= (1<<3);   ALREADY
	DDRD |= (1<<4);
	DDRD |= (1<<5);*/
	
	// initialize the LCD pins
	LCD_SCE_HI;
	//LCD_BACKLIGHT_HI;
	LCD_RST_LO;
	LCD_RST_HI;
	
	LCDWrite(LCD_C, 0x21);  // extended instruction set
	LCDWrite(LCD_C, 0xA9);	// set Vop to 5V
	LCDWrite(LCD_C, 0x04);	// set the temperature coefficient
	LCDWrite(LCD_C, 0x15);	// set the bias system
	// active mode, horizontal addressing, basic instruction set
	LCDWrite(LCD_C, 0x20);
	LCDWrite(LCD_C, 0x0C);  // normal mode
	LCDClear();             // clear the LCD
}

//This takes a large array of bits and sends them to the LCD
void LCDBitmap(const unsigned char my_array[]) {
	for (unsigned short index = 0 ; index < (LCD_WIDTH * LCD_HEIGHT / 8) ; index++)
		LCDWrite(LCD_D, my_array[index]);
}


void LCDWriteMap(unsigned char my_array[], unsigned short map_position) {
	unsigned short index = 0;

	for(unsigned char row = 0; row < 6; ++row) {
		for(index = row*420+map_position; index < row*420+84+map_position; ++index) {
			LCDWrite(LCD_D, my_array[index]);
		}
	}

}

