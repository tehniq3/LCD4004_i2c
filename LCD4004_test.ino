#include <avr/pgmspace.h>
#include <LiquidCrystal_I2C_4004.h>

#define LED 13

byte textLen;

const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 40;    // Number of character in a row
const byte lcdRows = 4;     // Number of lines
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows); // set the LCD address to 0x27 for a 16 chars and 2 line display

const unsigned int scrollDelay = 100;   // Miliseconds before scrolling next char
const unsigned int demoDelay = 2000;
void xyz(void);
void setup() {

  lcd.begin();
  delay(1000);
  lcd.home();
  lcd.backlight();
  pinMode(LED, OUTPUT);
  lcd.setCursor(0, 0); lcd.print("...blink......");
  lcd.blink(); delay(2000);
  lcd.setCursor(0, 0); lcd.print("...noBlink....");
  lcd.noBlink(); delay(2000);
  lcd.setCursor(0, 0); lcd.print("...cursor.....");
  lcd.cursor(); delay(2000);
  lcd.setCursor(0, 0); lcd.print("...noCursor...");
  lcd.noCursor(); delay(2000);
  lcd.setCursor(0, 0); lcd.print(".backlight off..");
  lcd.noBacklight(); delay(2000);
  lcd.setCursor(0, 0); lcd.print(".backlight on..");
  lcd.backlight(); delay(2000);

}
void loop() {

  lcd.clear(0);
  lcd.clear(1);
  lcd.setCursor(0, 0);
  lcd.print("Coordenadas 0,0");
  lcd.setCursor(0, 1);
  blink_led();
  lcd.print("Coordenadas 0,1");
  blink_led();
  lcd.setCursor(0, 2);
  lcd.print("Coordenadas 0,2");
  blink_led();
  lcd.setCursor(0, 3);
  lcd.print("Coordenadas 0,3");
  blink_led();
  xyz(0);
  xyz(1);
  lcd.clear(1);
  lcd.clear(0);
  lcd.setCursor(20, 0);
  lcd.print("Coordenadas 20,0");
  lcd.setCursor(20, 1);
  lcd.print("Coordenadas 20,1");
  delay(5000);
  lcd.clear(0);
  lcd.setCursor(20, 2);
  lcd.print("Coordenadas 20,2");
  lcd.setCursor(20, 3);
  lcd.print("Coordenadas 20,3");
  delay(5000);
  lcd.clear(1);
  lcd.setCursor(0, 0);
  lcd.print(".Teste.");
  for (int x = 0; x < 34; x++)
  {
    lcd.setCursor(x, 0);
    lcd.print(" Teste ");
    blink_led();
  }
  for (int x = 33; x > 0; x--)
  {
    lcd.setCursor(x, 0);
    lcd.print(" Teste ");
    blink_led();
  }


}

void xyz(uint8_t nlcd) {
  // Scroll text to the right back to original position
  for (byte positionCounter = 0; positionCounter < lcdCols; positionCounter++) {
    lcd.scrollDisplayLeft(nlcd);
    delay(scrollDelay);
  }


  // Scroll entire text in a row to the left outside the screen
  for (byte positionCounter = 0; positionCounter < textLen; positionCounter++) {
    lcd.scrollDisplayLeft(nlcd);
    delay(scrollDelay);
  }
  // Scroll hidden text through entire row to the right outside the screen
  for (byte positionCounter = 0; positionCounter < textLen + lcdCols; positionCounter++) {
    lcd.scrollDisplayRight(nlcd);
    delay(scrollDelay);
  }
  // Scroll text to the right back to original position
  for (byte positionCounter = 0; positionCounter < lcdCols; positionCounter++) {
    lcd.scrollDisplayLeft(nlcd);
    delay(scrollDelay);
  }

}
void blink_led() {
  for (int i = 0; i < 5; i++) {
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  }
}