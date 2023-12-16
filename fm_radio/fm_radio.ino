#include <Wire.h>
#include <TEA5767.h> //https://github.com/big12boy/TEA5767 
#include <Adafruit_GFX.h> //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h> //https://github.com/adafruit/Adafruit_SSD1306
#include <EEPROM.h> // Include EEPROM library to store frequency

#include "welcome_logo.h"

long lmillis = 0;

#define CLK 2
#define DT 3
#define SW 4
#define OLED_RESET 5 // Change this pin according to your setup
#define EEPROM_ADDR 0 // Address in EEPROM to store the frequency
#define LED_PIN 6 //LED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TEA5767 radio = TEA5767();

float frequency = 106.7; // Default frequency
int signalStrength = 0;

int currentStateCLK;
int lastStateCLK;

unsigned long lastButtonPress = 0;

#define LOGO_HEIGHT   64
#define LOGO_WIDTH    128

void setup() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLK);
  Serial.begin(9600);
  
  // Retrieve last tuned frequency from EEPROM
  EEPROM.get(EEPROM_ADDR, frequency);
  
  Serial.print("Frequency: ");
  Serial.println(frequency);
  Wire.begin();
  radio.setFrequency(frequency);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  showWelcomeScreen();

  // Wait for some time to allow the radio module to tune in
  delay(2000);

  // Update signal strength and display main screen
  signalStrength = radio.getSignalLevel();
  updateDisplay(frequency, signalStrength);
}

void loop() {   

  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(DT) != currentStateCLK) {
      frequency += 0.1;
      if (frequency > 108.1) {
        frequency = 87.5;
      }
      radio.setFrequency(frequency);
    } else {
      frequency -= 0.1;
      if (frequency < 87.5) {
        frequency = 108;
      }
      radio.setFrequency(frequency);
    }

    signalStrength = radio.getSignalLevel(); // Get signal strength
    
    updateDisplay(frequency, signalStrength);
  }

  lastStateCLK = currentStateCLK;

  int btnState = digitalRead(SW);
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
    }
    lastButtonPress = millis();
  }

  delay(1);
}

// Function to update display with frequency and signal strength
void updateDisplay(float freq, int strength) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont(NULL);
  display.setCursor(39, 1);
  display.println("FM RADIO");
  display.drawLine(120, 10, 8, 10, 1);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 15);
  display.println(freq, 1);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont(NULL);
  display.setCursor(85, 15);
  display.println("Mhz");
  display.drawLine(120, 33, 8, 33, 1);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont(NULL);
  display.setCursor(5, 56);
  display.drawLine(40, 45, 80, 45, 1);
  display.println("Developed By ShanukJ");

  // draw a signal level box (x ,y, w ,h)
  display.drawRect(85, 23, 30, 6, WHITE);
  uint8_t sl = strength;
  for (uint8_t x = 0; x < sl; x++) {
    display.fillRect(85 + x, 23, 1, 6, WHITE);
  }
  display.display();

  bool stereo = radio.isStereo();
  if (stereo) {
    digitalWrite(LED_PIN, HIGH); // Turn on the LED
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off the LED
  }

  // Save current frequency to EEPROM
  EEPROM.put(EEPROM_ADDR, freq);
}

// Function to display a welcome screen
void showWelcomeScreen() {
 // Display welcome screen
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    epd_bitmap_radio, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000); // Display the welcome screen for 2 seconds
  display.clearDisplay(); // Clear the display after 2 seconds
}

// TEA5767 FM Radio Module: https://s.click.aliexpress.com/e/_Dm6IihX 
// Arduino Nano: https://s.click.aliexpress.com/e/_DlCrJc1 
// OLED Display Module: https://s.click.aliexpress.com/e/_DdRoR0p 
// Rotary Encoder: https://s.click.aliexpress.com/e/_DB7joUl 
// Amplifier Module TDA2030A: https://s.click.aliexpress.com/e/_DCfpBN3

