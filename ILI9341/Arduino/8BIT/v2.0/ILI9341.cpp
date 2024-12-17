/*
Explanation About Used Registers
  0xC5: VCOM Control 1
    Define baseline voltage level for the pixels
    purposes:
      Viewing angles
      Contrast ratio
      Preventing flickering
  
  0x36: Memory Access Control
    used to define how the display controller maps the data from its internal memory (GRAM) to the pixels on the screen
    purposes:
      Screen Orientation:
        Allows you to rotate the display (0°, 90°, 180°, or 270°) by mapping rows and columns accordingly.
      Mirroring:
        Enables horizontal or vertical mirroring for flipping the display image.
      Row/Column Exchange (R/B):
        Swaps rows and columns, effectively rotating the screen.
      Color Order (BGR):
        Defines the byte order of pixel colors (RGB or BGR).
      Scan Direction:
        Controls the direction in which pixels are refreshed.

  0x3A: COLMOD: Pixel Format Set
    used to configure the pixel format (color depth) of the interface and the display
    purposes:
      Pixel Format Selection:
        Defines the number of bits per pixel (BPP) for the color format.
        Options include 12-bit, 16-bit, 18-bit, and sometimes 24-bit color modes.
      Interface Compatibility:
        Ensures the controller matches the format of the data sent by your microcontroller.
      Color Accuracy vs. Memory/Speed:
        Higher bit depths (e.g., 18-bit) provide better color accuracy but require more data and processing.
        Lower bit depths (e.g., 16-bit) are faster but with fewer colors.

  0x11: Sleep Out
    used to wake the display from sleep mode because by default it is in sleep mode

  0x29: Display ON
    used to turn on the display and enable the display output because by default it is OFF

  0x2C: Memory Write
    used to send pixel data to the display's frame memory (GRAM)
    purposes:
      Write Pixel Data:
        Transfers raw pixel data (color information) to the display memory.
        The data format depends on the pixel format set via 0x3A (COLMOD: Pixel Format Set).
      Define Pixels in Active Area:
        Works in conjunction with Column Address Set (0x2A) and Page Address Set (0x2B) to specify the area to write pixels.

  0x2A: Column Address Set
    used to specify the horizontal range (columns) of the display's frame memory (GRAM) where pixel data will be written
    purposes:
      Set Horizontal Range:
        Specifies the left and right boundaries (columns) of the area to update.
      Define Active Window:
        Used together with 0x2B (Page Address Set) to set a rectangular area for pixel operations.
        Essential for partial screen updates to improve performance.
      Prepare for Pixel Data Writes:
        After defining the column range, pixel data written with 0x2C (Memory Write) will only affect the specified columns.

  0x2B: Page Address Set
    used to specify the vertical range (rows) of the display's frame memory (GRAM) where pixel data will be written
    Purpose of Register 0x2B:
      Set Vertical Range:
        Specifies the top and bottom boundaries (rows) of the area to update.
      Define Active Window:
        Used with 0x2A to create a rectangular region for pixel operations.
        Reduces the amount of data needed for partial updates.
      Prepare for Pixel Data Writes:
        Pixel data sent with 0x2C (Memory Write) will only affect the specified rows within the column range set by 0x2A.
*/

#include <Arduino.h>
#include "ILI9341.h"

ILI9341::ILI9341(uint8_t RST, uint8_t CS, uint8_t RS, uint8_t SDA, uint8_t SCL, uint8_t LED){
  //Save all pins as private
  _RST = RST;
  _CS = CS;
  _RS = RS;
  _SDA = SDA;
  _SCL = SCL;
  _LED = LED;
}

//Initialization of the lcd
void ILI9341::init() {
  //Set all pins as outputs
  pinMode(_RST, OUTPUT); // Set pin mode to OUTPUT
  pinMode(_CS, OUTPUT); // Set pin mode to OUTPUT
  pinMode(_RS, OUTPUT); // Set pin mode to OUTPUT
  pinMode(_SDA, OUTPUT); // Set pin mode to OUTPUT
  pinMode(_SCL, OUTPUT); // Set pin mode to OUTPUT
  pinMode(_LED, OUTPUT); // Set pin mode to OUTPUT

  //Set all pins high(5V)
  digitalWrite(_RST, 1); // Set pin mode to 1
  digitalWrite(_CS, 1); // Set pin mode to 1
  digitalWrite(_RS, 1); // Set pin mode to 1
  digitalWrite(_SDA, 1); // Set pin mode to 1
  digitalWrite(_SCL, 1); // Set pin mode to 1
  digitalWrite(_LED, 1); // Set pin mode to 1

  //Delay for stabiliity
  delay(15);

  //performing hardware reset sequence to reset all registers
  digitalWrite(_RST, HIGH);
  delay(5);
  digitalWrite(_RST, LOW);  // Reset the LCD
  delay(15);
  digitalWrite(_RST, HIGH);
  delay(15);

  //Initialization commands
  // VCOM Control 1
  commandWrite(0xC5);
  dataWrite(0x54);    // High VCOM
  dataWrite(0x00);    // Low VCOM

  // Memory Access Control
  commandWrite(0x36);
  dataWrite(0b0000100);

  // COLMOD: Pixel Format Set
  commandWrite(0x3A);
  dataWrite(0x55);    // 16-bit RGB

  // Sleep Out
  commandWrite(0x11);

  delay(10); // Delay for stability

  // Display ON
  commandWrite(0x29);
}

// Write a data byte to the LCD
void ILI9341::dataWrite(uint8_t message) {
  digitalWrite(_RS, HIGH);  // Set RS to HIGH for data mode
  digitalWrite(_CS, LOW);   // Activate the chip by pulling CS LOW
  writeData(message);                // Write the data byte
  digitalWrite(_CS, HIGH);  // Deactivate the chip
}

// Write a command byte to the LCD
void ILI9341::commandWrite(uint8_t message) {
  digitalWrite(_CS, LOW);   // Activate the chip
  digitalWrite(_RS, LOW);   // Set RS to LOW for command mode
  writeData(message);                // Write the command byte
  digitalWrite(_CS, HIGH);  // Deactivate the chip
}

// Set the address range for drawing
void ILI9341::Address_set(int16_t y1, int16_t y2, int16_t x1, int16_t x2) {
  // Column Address Set
  commandWrite(0x2A);
  dataWrite(y1 >> 8); // High byte of y1
  dataWrite(y1);      // Low byte of y1
  dataWrite(y2 >> 8); // High byte of y2
  dataWrite(y2);      // Low byte of y2

  // Page Address Set
  commandWrite(0x2B);
  dataWrite(x1 >> 8); // High byte of x1
  dataWrite(x1);      // Low byte of x1
  dataWrite(x2 >> 8); // High byte of x2
  dataWrite(x2);      // Low byte of x2

  // Memory Write
  commandWrite(0x2C);
}

// Draw a single pixel on the LCD
void ILI9341::drawPixel(int16_t x, int16_t y, uint16_t color) {
  Address_set(y, y + 1, x, x + 1); // Set address range for the pixel
  dataWrite(color >> 8);           // Write high byte of the color
  dataWrite(color);                // Write low byte of the color
}

// Send a byte of data bit-by-bit to the LCD
void ILI9341::writeData(uint8_t value) {
  bool bitValue;
  for (int i = 7; i >= 0; i--) {
    bitValue = (value >> i) & 0b1; // Extract each bit
    digitalWrite(_SCL, LOW);  // Set clock LOW
    digitalWrite(_SDA, bitValue); // Set data line
    digitalWrite(_SCL, HIGH); // Set clock HIGH
  }
}