/* base info from https://forum.arduino.cc/t/pcf8574-i2c-adapter-modified-for-lcd-4004-with-clear/985110
 * changes by Nicu FLORICA (niq_ro)
 * v.1 - huge numbers based on original article from https://github.com/tehniq3/serialLCD/blob/master/RTC_clock_on_serial_lcd.ino
 * v.1a..1d - used parts from library downloaded from https://www.surenoo.com/products/6791632?variant_sku_code=6791632-5772717-26634711-5803041-26634714-0-0
 * v.1e - return to used library for LCD4004 (initial)
 */

#include <avr/pgmspace.h>
#include <LiquidCrystal_I2C_4004.h>

const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 40;    // Number of character in a row
const byte lcdRows = 4;     // Number of lines
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows); // set the LCD address to 0x27 for a 16 chars and 2 line display

int x = 0;
int x0 = 3;
int numar = 0;
int zora, uora, ora;
int zminut, uminut, minut;
int zsec, usec, sec;
int zora0 = 11;
int uora0 = 11;
int zminut0 = 11;
int uminut0 = 11;
int zsec0 = 11;
int usec0 = 11;

// *******DEFINE CUSTOM CHARACTERS FOR BIG FONT*****************
  byte sus[8] =
  {
  0b11111,
  0b11111,
  0b11111,
  0b01111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
  };
  byte jos[8] =
  {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111
  };

// dummy characters (not used on display, but for fill the LCD memory)
byte custom_char_0[8] =  {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}; 
byte custom_char_1[8] =  {0b01110, 0b11011, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111}; 
byte custom_char_2[8] =  {0b00000, 0b10101, 0b01110, 0b11111, 0b01110, 0b10101, 0b00000, 0b00000}; 
byte custom_char_3[8] =  {0b00011, 0b00101, 0b11001, 0b10001, 0b10001, 0b11001, 0b00101, 0b00011}; 
byte custom_char_4[8] =  {0b00011, 0b01101, 0b01001, 0b01001, 0b01001, 0b01011, 0b11011, 0b11000}; 
byte custom_char_5[8] =  {0b00000, 0b00001, 0b00001, 0b00101, 0b00101, 0b10101, 0b10101, 0b10101}; 
byte custom_char_6[8] =  {0b01110, 0b01001, 0b11001, 0b01110, 0b01110, 0b11001, 0b01001, 0b01110}; 
byte custom_char_7[8] =  {0b01010, 0b01010, 0b11111, 0b10001, 0b10001, 0b01110, 0b00100, 0b00100}; 

  
void setup() {

  lcd.begin();
  delay(200); // it´s not clear if this delay is really needed
  lcd.home(); // move the cursor to the top left of the screen
  lcd.backlight(); // enable backlight

  
  delay(1000);
  lcd.home();
  lcd.backlight();

  lcd.createChar(0, custom_char_0);
  lcd.createChar(1,sus);
  lcd.createChar(2,jos);
  lcd.createChar(3, custom_char_3);
  lcd.createChar(4, custom_char_4);
  lcd.createChar(5,sus);
  lcd.createChar(6,jos);
  lcd.createChar(7, custom_char_7); 
  
  lcd.clear(0);
  lcd.clear(1);
  lcd.setCursor(0, 0);
  lcd.print("Test on LCD4004 display");
  lcd.setCursor(0, 1);
  lcd.print("Custom huge numbers");
  lcd.setCursor(0, 2);
  lcd.print("sw wrote by Nicu FLORICA (niq_ro)");
  lcd.setCursor(0, 3);
  lcd.print("v.1 - 17.01.2025, Craiova, RO");
  delay(3000);
  lcd.clear(0);
  lcd.clear(1);

ora = 23;
minut = 42;
sec = 0;
}


void loop() {
 lcd.createChar(0, custom_char_0);
  lcd.createChar(1,sus);
  lcd.createChar(2,jos);
  lcd.createChar(3, custom_char_3);
  lcd.createChar(4, custom_char_4);
  lcd.createChar(5,sus);
  lcd.createChar(6,jos);
  lcd.createChar(7, custom_char_7); 
  
//  lcd.clear(0);
//  lcd.clear(1);
  


sec = sec + 1;
if (sec >= 60) 
  {
  sec = 0;
  minut = minut + 1;
  }

//minut = minut + 1;
if (minut >= 60) 
  {
  minut = 0;
  ora = ora + 1;
  }
if (ora >= 24) 
  {
  minut = 0;
  ora = 0;
  sec = 0;
  }

  zora = ora/10;
  uora = ora%10;
  zminut = minut/10;
  uminut = minut%10;
  zsec = sec/10;
  usec = sec%10;  

if (zora != zora0)
{
x = x0 + 0;
if (zora == 0)
{
  spatiu();
}
else
  cifra(zora);
}

if (uora != uora0)
{
x = x0 + 5;
  cifra(uora);
}

x = x0 + 10;
lcd.setCursor(x,1); 
lcd.write(255);
lcd.setCursor(x,3); 
lcd.write(255);

if (zminut != zminut0)
{
x = x0 + 12;
  cifra(zminut);
}
if (uminut != uminut0)
{
x = x0 + 18;
  cifra(uminut);
}

x = x0 + 23;
lcd.setCursor(x,1); 
lcd.write(255);
lcd.setCursor(x,3); 
lcd.write(255);

if (zsec != zsec0)
{
x = x0 + 25;
  cifra(zsec);
}
if (usec != usec0)
{
x = x0 + 30;
  cifra(usec);
}
  zora = zora0;
  uora = uora0;
  zminut = zminut0;
  uminut = zminut0;
  zsec = zsec0;
  usec = usec0;  

  delay(1000);


}  // end main loop

void custom0()
{ // uses segments to build the number 0 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\xA0\xA0\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05"); 
}

void custom1()
{ // uses segments to build the number 1 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xA0\x02\xFF\xA0"); // empty
  lcd.setCursor(x,1); 
  lcd.print("\x01\xA0\xFF\xA0");  // solid
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xFF\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\x06\x06\xFF\x06");    
}

void custom2()
{ // uses segments to build the number 2 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02");  // down
  lcd.print("\x01");  // up
  lcd.print("\x01");  // up
  lcd.print("\x02");  // down
  lcd.setCursor(x,1); 
  lcd.print("\x01");    // up
  lcd.print("\xA0");  // empty
  lcd.print("\xA0");  // empty
  lcd.print("\xFF"); // solid
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\x06");  
  lcd.print("\x05");
  lcd.print("\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xFF");
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x06");    
}

void custom3()
{ // uses segments to build the number 3 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02"); 
  lcd.print("\x01");   
  lcd.print("\x01");
  lcd.print("\x02"); 
  lcd.setCursor(x,1); 
  lcd.print("\xA0");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\x01");  
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05");
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x05");    
}

void custom4()
{ // uses segments to build the number 4 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02"); 
  lcd.print("\x01");  
  lcd.print("\xA0");
  lcd.print("\xFF");
  lcd.setCursor(x,1); 
  lcd.print("\xFF");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF");   
}

void custom5()
{ // uses segments to build the number 5 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF"); 
  lcd.print("\x01");  
  lcd.print("\x01");
  lcd.print("\x01");
  lcd.setCursor(x,1); 
  lcd.print("\xFF");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05");  
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x05");   
}

void custom6()
{ // uses segments to build the number 6 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02"); 
  lcd.print("\x01");  
  lcd.print("\x01");
  lcd.print("\x02");
  lcd.setCursor(x,1); 
  lcd.print("\xFF");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05");  
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x05");   
}

void custom7()
{ // uses segments to build the number 7 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF"); 
  lcd.print("\x01");  
  lcd.print("\x01");
  lcd.print("\xFF");
  lcd.setCursor(x,1); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\x02");  
  lcd.print("\x01"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\x02");  
  lcd.print("\x05");
  lcd.print("\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0");  
  lcd.print("\xFF");  
  lcd.print("\xA0");
  lcd.print("\xA0");   
}

void custom8()
{ // uses segments to build the number 8 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02"); 
  lcd.print("\x01");  
  lcd.print("\x01");
  lcd.print("\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\x01"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05");  
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x05");   
}

void custom9()
{ // uses segments to build the number 9 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02"); 
  lcd.print("\x01");  
  lcd.print("\x01");
  lcd.print("\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01");  
  lcd.print("\x02");  
  lcd.print("\x02");  
  lcd.print("\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05");  
  lcd.print("\x06");  
  lcd.print("\x06");
  lcd.print("\x05");   
}

void spatiu()
{ // uses segments to clear a number - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xA0"); 
  lcd.print("\xA0");  
  lcd.print("\xA0");
  lcd.print("\xA0");
  lcd.setCursor(x,1); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");  
  lcd.print("\xA0");    
}

// subrutina de afisare a numerelor
void cifra(int numar)
{
  switch (numar)
  {
    case 0:
    custom0();
    break;

    case 1:
    custom1();
    break;

    case 2:
    custom2();
    break;

    case 3:
    custom3();
    break;

    case 4:
    custom4();
    break;

    case 5:
    custom5();
    break;

    case 6:
    custom6();
    break;

    case 7:
    custom7();
    break;

    case 8:
    custom8();
    break;

    case 9:
    custom9();
    break;
  }
}
