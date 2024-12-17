#include "ILI9341.h"

ILI9341 lcd(22, 15, 21, 13, 14, 23);

void setup() {
  lcd.init(); // Initialize the LCD

  // set drawable area for the example
  lcd.Address_set(0, 320, 0, 480); // Set address range for the display
}

void loop() {
  // example drawing
  static uint16_t i = 0;
  for(int x=0; x<320*8; x++){
    lcd.dataWrite(i);
  }
  i++;
}
