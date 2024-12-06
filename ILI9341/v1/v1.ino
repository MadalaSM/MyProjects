// Define a structure to hold LCD pinout
struct LCD_PINOUT {
  uint8_t RST;  // Reset pin
  uint8_t CS;   // Chip Select pin
  uint8_t RS;   // Register Select pin
  uint8_t SDA;  // Serial Data pin
  uint8_t SCL;  // Serial Clock pin
  uint8_t LED;  // LED backlight pin
  
  // Function to retrieve a pin value based on its index
  uint8_t get_pin(uint8_t pin) {
    switch (pin) {
      case 0: return RST;
      case 1: return CS;
      case 2: return RS;
      case 3: return SDA;
      case 4: return SCL;
      case 5: return LED;
      default: return 255; // Return an invalid value for unknown pins
    }
  };
  uint8_t count_pin = 6; // Total number of pins in the structure
};

// Initialize the LCD_PINOUT structure with ESP32 pin numbers
LCD_PINOUT lcdPinout = {22, 15, 21, 13, 14, 23};

// Write a data byte to the LCD
void dataWrite(uint8_t message) {
  digitalWrite(lcdPinout.RS, HIGH);  // Set RS to HIGH for data mode
  digitalWrite(lcdPinout.CS, LOW);   // Activate the chip by pulling CS LOW
  writeData(message);                // Write the data byte
  digitalWrite(lcdPinout.CS, HIGH);  // Deactivate the chip
}

// Write a command byte to the LCD
void commandWrite(uint8_t message) {
  digitalWrite(lcdPinout.CS, LOW);   // Activate the chip
  digitalWrite(lcdPinout.RS, LOW);   // Set RS to LOW for command mode
  writeData(message);                // Write the command byte
  digitalWrite(lcdPinout.CS, HIGH);  // Deactivate the chip
}

// Initialize the LCD with specific settings
void initLCD() {
  delay(15); // Wait for LCD to stabilize

  // Perform a hardware reset sequence
  digitalWrite(lcdPinout.RST, HIGH);
  delay(5);
  digitalWrite(lcdPinout.RST, LOW);  // Reset the LCD
  delay(15);
  digitalWrite(lcdPinout.RST, HIGH);
  delay(15);

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

// Set the address range for drawing
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

// Draw a single pixel on the LCD
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  Address_set(y, y + 1, x, x + 1); // Set address range for the pixel
  dataWrite(color >> 8);           // Write high byte of the color
  dataWrite(color);                // Write low byte of the color
}

// Send a byte of data bit-by-bit to the LCD
void writeData(uint8_t value) {
  bool bitValue;
  for (int i = 7; i >= 0; i--) {
    bitValue = (value >> i) & 0b1; // Extract each bit
    digitalWrite(lcdPinout.SCL, LOW);  // Set clock LOW
    digitalWrite(lcdPinout.SDA, bitValue); // Set data line
    digitalWrite(lcdPinout.SCL, HIGH); // Set clock HIGH
  }
}

void setup() {
  // Configure LCD's pins as outputs and set their initial state
  for (int i = 0; i < lcdPinout.count_pin; i++) {
    pinMode(lcdPinout.get_pin(i), OUTPUT); // Set pin mode to OUTPUT
    digitalWrite(lcdPinout.get_pin(i), HIGH); // Initialize pins to HIGH
  }

  initLCD(); // Initialize the LCD

  // set drawable area for the example
  Address_set(0, 320, 0, 480); // Set address range for the display
}

void loop() {
  // example drawing
  static uint16_t i = 0;
  dataWrite(i);
  i++;
}
