/*
  Speedcube Timer (with INPUT_PULLUP, debounce, and LCD startup fix)
  Place hands on both buttons (to ground). When both are released, timer starts.
  When both buttons are pressed, timer stops and displays final time until Arduino reset.
  Uses millis() for timing and formats time up to minutes.
*/
#include <LiquidCrystal.h>

// LCD pins
const int lcd_rs = 12;
const int lcd_e  = 6;
const int lcd_D4 = 11;
const int lcd_D5 = 10;
const int lcd_D6 = 9;
const int lcd_D7 = 7;
LiquidCrystal lcd(lcd_rs, lcd_e, lcd_D4, lcd_D5, lcd_D6, lcd_D7);

// Hand buttons: wired between pin and GND, using internal pull-ups
const int leftHandPin  = A0;
const int rightHandPin = A1;

// State variables
bool running = false;
bool done    = false;
unsigned long startTime = 0;
unsigned long elapsed   = 0;

// Debounce delay in milliseconds
const unsigned long debounceDelay = 50;

void setup() {
  // Wait for LCD power stability
  delay(500);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Speedcube Timer");
  delay(1000);
  lcd.clear();

  // Initialize button pins with pull-ups
  pinMode(leftHandPin, INPUT_PULLUP);
  pinMode(rightHandPin, INPUT_PULLUP);

  lcd.print("Place hands...");
}

void loop() {
  // Read button states (LOW = pressed)
  bool leftPressed  = (digitalRead(leftHandPin) == LOW);
  bool rightPressed = (digitalRead(rightHandPin) == LOW);

  if (!running && !done) {
    // Waiting for start: must press both to arm
    if (leftPressed && rightPressed) {
      delay(debounceDelay); // debounce
      if (digitalRead(leftHandPin) == LOW && digitalRead(rightHandPin) == LOW) {
        lcd.clear();
        lcd.print("Ready...");

        // Wait until both are released to start
        while (digitalRead(leftHandPin)==LOW || digitalRead(rightHandPin)==LOW) {
          delay(1);
        }
        delay(debounceDelay); // debounce
        startTime = millis();
        running = true;
        lcd.clear();
      }
    }
  }

  if (running) {
    // Update running time
    unsigned long now = millis();
    elapsed = now - startTime;
    displayTime(elapsed);

    // Stop when both pressed
    if (digitalRead(leftHandPin)==LOW && digitalRead(rightHandPin)==LOW) {
      delay(debounceDelay); // debounce
      if (digitalRead(leftHandPin)==LOW && digitalRead(rightHandPin)==LOW) {
        running = false;
        done    = true;
        displayTime(elapsed);
      }
    }
  }

  if (done) {
    // Do nothing: hold final time until Arduino reset
  }
}

// Display elapsed time in mm:ss.mmm or sss.mmm
void displayTime(unsigned long ms) {
  unsigned int minutes = ms / 60000;
  unsigned int seconds = (ms % 60000) / 1000;
  unsigned int millisec = ms % 1000;

  char buf[16];
  if (minutes > 0) {
    sprintf(buf, "%u:%02u.%03u", minutes, seconds, millisec);
  } else {
    sprintf(buf, "%u.%03u", seconds, millisec);
  }

  lcd.setCursor(0, 0);
  lcd.print(buf);
}
