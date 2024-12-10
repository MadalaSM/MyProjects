#include "stm32f411xe.h"

void delay(uint8_t time);
void dataWrite(uint8_t message);
void commandWrite(uint8_t message);
void initLCD();
void Address_set(int16_t y1, int16_t y2, int16_t x1, int16_t x2);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void writeData(uint8_t value);


void delay(uint8_t time){
	for(int i=0; i<time*1000; i++){
		}
}

void dataWrite(uint8_t message) {
  GPIOB->BSRR = 0b1<<10;

	GPIOB->BSRR = 0b1<<(9+16);

	writeData(message);
	GPIOB->BSRR = 0b1<<9;

}

void commandWrite(uint8_t message) {
  GPIOB->BSRR = 0b1<<(10+16);

	GPIOB->BSRR = 0b1<<(9+16);

	writeData(message);
	GPIOB->BSRR = 0b1<<9;

}

void initLCD() {
  delay(150); // Wait for LCD to stabilize

  // Perform a hardware reset sequence
  GPIOB->BSRR = 0b1<<8;
  delay(250);
  GPIOB->BSRR = 0b1<<(8+16);
  delay(150);
  GPIOB->BSRR = 0b1<<8;
  delay(150);

  // Example initialization commands
  commandWrite(0xC5); // VCOM Control 1
  dataWrite(0x54);    // High VCOM
  dataWrite(0x00);    // Low VCOM

  commandWrite(0x36); // Memory Access Control
  dataWrite(0b0000100); // Example data for access control

  commandWrite(0x3A); // Set pixel format
  dataWrite(0x55);    // 16-bit RGB

  commandWrite(0x11); // Exit Sleep mode
  delay(10);          // Delay for stability

  commandWrite(0x29); // Turn on display
  commandWrite(0x2C); // Prepare for memory write
}

void Address_set(int16_t y1, int16_t y2, int16_t x1, int16_t x2) {
  commandWrite(0x2A); // Column Address Set
  dataWrite(y1 >> 8); // High byte of y1
  dataWrite(y1);      // Low byte of y1
  dataWrite(y2 >> 8); // High byte of y2
  dataWrite(y2);      // Low byte of y2

  commandWrite(0x2B); // Page Address Set
  dataWrite(x1 >> 8); // High byte of x1
  dataWrite(x1);      // Low byte of x1
  dataWrite(x2 >> 8); // High byte of x2
  dataWrite(x2);      // Low byte of x2

  commandWrite(0x2C); // Memory Write
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {
  Address_set(y, y + 1, x, x + 1); // Set address range for the pixel
  dataWrite(color >> 8);           // Write high byte of the color
  dataWrite(color);                // Write low byte of the color
}

void writeData(uint8_t value) {
  GPIOB->BSRR = value | ((value ^ 0xff) << 16);

	GPIOB->BSRR = 0b1<<12;

  GPIOB->BSRR = 0b1<<(12+16);
}

int main(void){
	RCC->AHB1ENR = 0b111;
	GPIOC->MODER |= 0b00000100000000000000000000000000;
	GPIOC->MODER &= 0b11110111111111111111111111111111;
	GPIOB->MODER = 0x55555555;
	GPIOB->BSRR = 0xffff;
	initLCD();
	Address_set(240, 0, 300, 0);
	GPIOC->ODR &= 0b1101111111111111;
	while(1){
		dataWrite(0b0);
	}
}
