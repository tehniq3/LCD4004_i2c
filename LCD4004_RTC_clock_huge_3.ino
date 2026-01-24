/* base info from https://forum.arduino.cc/t/pcf8574-i2c-adapter-modified-for-lcd-4004-with-clear/985110
 * changes by Nicu FLORICA (niq_ro)
 * v.1 - huge numbers based on original article from https://github.com/tehniq3/serialLCD/blob/master/rtc_clock_on_serial_lcd.ino
 * v.1a..1d - used parts from library downloaded from https://www.surenoo.com/products/6791632?variant_sku_code=6791632-5772717-26634711-5803041-26634714-0-0
 * v.1e - return to used library for LCD4004 (initial)
 * v.1f - used millis function for update the seconds and changed number 4
 * v.2 - added rtc module
 * v,2a - check just rtc time -> seconds -> (used millis function)
 * v.2b - added date in last seconds of each seconds
 * v.2c - added temperature or date in last secondes of each seconds 
 * v.2d - smooth transitions, no too much empty screen
 * v.3 - added encoder - adjust clock, date and time,  based on https://github.com/tehniq3/big_font_weather_clock
 */

#include <avr/pgmspace.h>
#include <LiquidCrystal_I2C_4004.h> // original from  https://forum.arduino.cc/t/pcf8574-i2c-adapter-modified-for-lcd-4004-with-clear/985110
#include <Encoder.h> // from http://www.pjrc.com/teensy/td_libs_Encoder.html                                   // a copy can found at https://github.com/tehniq3/LCD4004_i2c
#include "RTClib.h"  // https://github.com/adafruit/rtclib

const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 40;    // Number of character in a row
const byte lcdRows = 4;     // Number of lines
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows); // set the LCD address to 0x27 for a 16 chars and 2 line display


RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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
  0b11111,
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

unsigned long timp;
unsigned long timp0 = 200;
int an, luna, zi, zis; 
float temperatura; 
int zzi, uzi, zluna, uluna, zan, uan;  
int sec7 = 55;
int temp, ztemp, utemp, stemp, rtemp;
byte nesters = 0; // 1 - nothing, 0 - need clear

#define sw1 4  // encoder switch
int PinA = 2;  // encoder right
int PinB = 3;  // encoder left
Encoder knob(PinB, PinA); //encoder connected to pins 2 and 3 (and ground)
//the variables provide the holding values for the set clock routine
int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int sethourstemp;
int setminstemp;
int setsecstemp;
int maxday; // maximum number of days in the given month
// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 3000; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
boolean buttonflag = false; // default value for the button flag
byte mod = 0; // ?
  
void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("rtc clock on LCD4004 display");
  lcd.begin();
  delay(200); // it´s not clear if this delay is really needed
  lcd.home(); // move the cursor to the top left of the screen
  lcd.backlight(); // enable backlight

  delay(1000);
  lcd.home();
  lcd.backlight();

  lcd.createChar(0, custom_char_0); // not used
  lcd.createChar(1,sus);            // used for 1st and 2nd rows
  lcd.createChar(2,jos);            // used for 1st and 2nd rows
  lcd.createChar(3, custom_char_3); // not used
  lcd.createChar(4, custom_char_4); // not used
  lcd.createChar(5,sus);            // used for 3rd and 4th rows
  lcd.createChar(6,jos);            // used for 3rd and 3th rows
  lcd.createChar(7, custom_char_7); // not used
  
  lcd.clear(0);
  lcd.clear(1);
  lcd.setCursor(0, 0);
  lcd.print("adjust rtc clock on LCD4004 display");
  lcd.setCursor(0, 1);
  lcd.print("Custom huge numbers");
  lcd.setCursor(0, 2);
  lcd.print("sw wrote by Nicu FLORICA (niq_ro)");
  lcd.setCursor(0, 3);
  lcd.print("v.3 - 24.01.2025, Craiova, RO");

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find rtc");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("rtc lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the rtc to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the rtc with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the rtc to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the rtc with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  
  delay(3000);
  lcd.clear(0);
  lcd.clear(1);

DateTime now = rtc.now();
sec = now.second();
usec = sec%10;
an = now.year();
luna = now.month();
zi = now.day();
zis = now.dayOfTheWeek();
ora = now.hour();
minut = now.minute();
}

void loop() {

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    
    if (pushlength < pushlengthset) 
    {
      ShortPush ();   
    }  
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength > pushlengthset) 
       {
         lcd.clear(1);lcd.clear(2);
         DateTime now = rtc.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setsecstemp=now.second(),DEC;
         setclock();
         pushlength = pushlengthset;
       };



if (millis() - timp > 100)
{ 
 
DateTime now = rtc.now();
sec = now.second();
usec = sec%10;
if (usec != usec0)
{
an = now.year();
luna = now.month();
zi = now.day();
zis = now.dayOfTheWeek();
ora = now.hour();
minut = now.minute();
temperatura = rtc.getTemperature();
    Serial.print(an);
    Serial.print('/');
    Serial.print(luna);
    Serial.print('/');
    Serial.print(zi);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[zis]);
    Serial.print(") ");
    Serial.print(ora);
    Serial.print(':');
    Serial.print(minut);
    Serial.print(':');
    Serial.print(sec);
    Serial.println();
    Serial.print("Temperature: ");
    Serial.print(temperatura);
    Serial.println(" C");

  lcd.createChar(0, custom_char_0);  // not used 
  lcd.createChar(1,sus);            // up for 1st and 2nd rows
  lcd.createChar(2,jos);            // down for 1st and 2nd rows
  lcd.createChar(3, custom_char_3); // not used 
  lcd.createChar(4, custom_char_4); // not used 
  lcd.createChar(5,sus);            // up for 3rd and 4th rows
  lcd.createChar(6,jos);            // down for 3rd and 4th rows
  lcd.createChar(7, custom_char_7); // not used 
}

if (nesters == 1)  // 
{
if ((sec == sec7) or (sec == 00))
{
  lcd.clear(0);
  lcd.clear(1);
  nesters = 0;
  

zora0 = 11;
uora0 = 11;
zminut0 = 11;
uminut0 = 11;
zsec0 = 11;
usec0 = 11;
}
}
else
{
  if ((sec == sec7 + 2) or (sec == sec7))
  nesters = 1;
}


//if (sec < sec7)
if ((sec < sec7) and (nesters == 0))
{
  x0 = 3;
  zora = ora/10;
  uora = ora%10;
  zminut = minut/10;
  uminut = minut%10;
  zsec = sec/10;
  usec = sec%10;  
 
//  lcd.clear(0);
//  lcd.clear(1);

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

x = x0 + 10;
lcd.setCursor(x,1); 
lcd.write(255);
lcd.setCursor(x,3); 
lcd.write(255);
}

if (zminut != zminut0)
{
x = x0 + 12;
  cifra(zminut);
}
if (uminut != uminut0)
{
x = x0 + 17;
  cifra(uminut);

  x = x0 + 22;
lcd.setCursor(x,1); 
lcd.write(255);
lcd.setCursor(x,3); 
lcd.write(255);
}

if (zsec != zsec0)
{
x = x0 + 24;
  cifra(zsec);
}
if (usec != usec0)
{
x = x0 + 29;
  cifra(usec);
}
  zora0 = zora;
  uora0 = uora;
  zminut0 = zminut;
  uminut0 = uminut;
  zsec0 = zsec;
  usec0 = usec;  
}

//else    // date
//if (sec == sec7)
if ((sec == sec7) and (nesters == 0))
{
  if (uminut%2 == 0)
  {
  zzi = zi/10;
  uzi = zi%10;
  zluna = luna/10;
  uluna = luna%10;
  zan = (an-2000)/10;
  uan = (an-2000)%10;  
 
//  lcd.clear(0);
//  lcd.clear(1);

x0 = 0;
x = x0 + 2;
  cifra0(zzi);

x = x0 + 6;
  cifra0(uzi);

x = x0 + 10;
lcd.setCursor(x,3); 
lcd.write(255);

x = x0 + 12;
  cifra0(zluna);

x = x0 + 16;
  cifra0(uluna);

x = x0 + 20;
lcd.setCursor(x,3); 
lcd.write(255);

x = x0 + 22;
  cifra0(2);
x = x0 + 26;
  cifra0(0);
x = x0 + 30;
  cifra0(zan);   
x = x0 + 34;
  cifra0(uan);
 }
 else
 {
 temp = (float)(10.*temperatura);
 ztemp = temp/100;
 rtemp = temp%200;
 utemp = rtemp/10;
 stemp = rtemp%10;
 
 //  lcd.clear(0);
 //  lcd.clear(1);

x0 = 0;
x = x0 + 5;
  customplus();

x = x0 + 11;
  cifra(ztemp);

x = x0 + 16;
  cifra(utemp);
  
x = x0 + 21;
lcd.setCursor(x,3);
lcd.print("\x06");  

x = x0 + 23;
  cifra(stemp);

x = x0 + 28;
  customo();

x = x0 + 32;
  customC();
 }
} // date or temperature


timp = millis();
}

delay(100);
}  // end main loop

// 0xFF = fill 100%, 0xA0 = empty 100%
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
  lcd.print("\xA0\x02\xFF\xA0"); // empty, down, fill,empty
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
  lcd.print("\x02\x01\x01\x02");  // down, up, up, down
  lcd.setCursor(x,1); 
  lcd.print("\x01\xA0\xA0\xFF");  // up, empty, empty, fill
  lcd.setCursor(x,2); 
  lcd.print("\xA0\x06\x05\xA0");  // empty, down, up, empty
  lcd.setCursor(x,3); 
  lcd.print("\xFF\x06\x06\x06");  // fill, down, down, down    
}

void custom3()
{ // uses segments to build the number 3 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02"); 
  lcd.setCursor(x,1); 
  lcd.print("\xA0\x02\x02\x01");  
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");    
}

void custom4()
{ // uses segments to build the number 4 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
//  lcd.print("\x02\x01\xA0\xFF");
  lcd.print("\xA0\xFF\xA0\xFF");
  lcd.setCursor(x,1); 
//  lcd.print("\xFF\x02\x02\xFF"); 
  lcd.print("\xFF\xA0\xA0\xFF"); 
  lcd.setCursor(x,2); 
//  lcd.print("\xA0\xA0\xA0\xFF");
  lcd.print("\x05\x05\x05\xFF");  
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xA0\xA0\xFF");   
}

void custom5()
{ // uses segments to build the number 5 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF\x01\x01\x01");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\x02\x02\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");   
}

void custom6()
{ // uses segments to build the number 6 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\x02\x02\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");   
}

void custom7()
{ // uses segments to build the number 7 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF\x01\x01\xFF");
  lcd.setCursor(x,1); 
  lcd.print("\xA0\xA0\x02\x01"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\x06\x05\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xFF\xA0\xA0");   
}

void custom8()
{ // uses segments to build the number 8 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01\x02\x02\x01"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");   
}

void custom9()
{ // uses segments to build the number 9 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01\x02\x02\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");   
}

void spatiu()
{ // uses segments to clear a number - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xA0\xA0\xA0\xA0");
  lcd.setCursor(x,1); 
  lcd.print("\xA0\xA0\xA0\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xA0\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xA0\xA0\xA0");    
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

// 0xFF = fill 100%, 0xA0 = empty 100%
void thin0 ()
{ // uses segments to build the number 0 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");  
  lcd.setCursor(x,1); 
  lcd.print("\xFF\xA0\xFF");
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05"); 
}

void thin1()
{ // uses segments to build the number 1 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xA0\xFF\xA0"); // empty, fill, empty
  lcd.setCursor(x,1); 
  lcd.print("\x01\xFF\xA0");  
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xFF\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\x06\xFF\x06");    
}

void thin2()
{ // uses segments to build the number 2 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");  // down, up, down
  lcd.setCursor(x,1); 
  lcd.print("\xA0\xA0\xFF");  // empty, empty, fill
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xFF\xA0");  // empty, fill, empty
  lcd.setCursor(x,3); 
  lcd.print("\xFF\x06\x06");  // fill, down, down    
}

void thin3()
{ // uses segments to build the number 3 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02"); 
  lcd.setCursor(x,1); 
  lcd.print("\xA0\x02\x01");  
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05");    
}

void thin4()
{ // uses segments to build the number 4 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF\xA0\xFF");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\xA0\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\x05\x05\xFF");  
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xA0\xFF");   
}

void thin5()
{ // uses segments to build the number 5 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF\x01\x01");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\x02\xA0");  
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05");   
}

void thin6()
{ // uses segments to build the number 6 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\x02\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05");   
}

void thin7()
{ // uses segments to build the number 7 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xFF\x01\xFF");
  lcd.setCursor(x,1); 
  lcd.print("\xA0\xA0\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xFF\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xFF\xA0\xA0");   
}

void thin8()
{ // uses segments to build the number 8 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01\x02\x01"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05");   
}

void thin9()
{ // uses segments to build the number 9 - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\x01\x02\xFF"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xFF"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x05");   
}

void spatiu0()
{ // uses segments to clear a number - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("\xA0\xA0\xA0");
  lcd.setCursor(x,1); 
  lcd.print("\xA0\xA0\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xA0\xA0");    
}

// subrutina de afisare a numerelor
void cifra0(int numar)
{
  switch (numar)
  {
    case 0:
    thin0();
    break;

    case 1:
    thin1();
    break;

    case 2:
    thin2();
    break;

    case 3:
    thin3();
    break;

    case 4:
    thin4();
    break;

    case 5:
    thin5();
    break;

    case 6:
    thin6();
    break;

    case 7:
    thin7();
    break;

    case 8:
    thin8();
    break;

    case 9:
    thin9();
    break;
  }
}

void customo()
{ // uses segments to build the degree sign
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x02");  // down-up-down
  lcd.setCursor(x,1); 
  lcd.print("\x01\x02\x01");  // up-down-up
}

void customC()
{ // uses segments to build the letter C 
  lcd.setCursor(x,0); 
  lcd.print("\x02\x01\x01\x02");
  lcd.setCursor(x,1); 
  lcd.print("\xFF\xA0\xA0\xA0"); 
  lcd.setCursor(x,2); 
  lcd.print("\xFF\xA0\xA0\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\x05\x06\x06\x05");   
}

void customplus()
{ // uses segments to build the + sign 
  lcd.setCursor(x,0); 
  lcd.print("\xA0\xA0\x02\xA0\xA0");
  lcd.setCursor(x,1); 
  lcd.print("\x02\x02\xFF\x02\x02"); 
  lcd.setCursor(x,2); 
  lcd.print("\xA0\xA0\xFF\xA0\xA0"); 
  lcd.setCursor(x,3); 
  lcd.print("\xA0\xA0\xA0\xA0\xA0");   
}

// that parts is for adjust the clock/time

//sets the clock
void setclock (){
   setyear ();
   lcd.clear (1);   lcd.clear (2);
   setmonth ();
   lcd.clear (1);   lcd.clear (2);
   setday ();
   lcd.clear (1);   lcd.clear (2);
   sethours ();
   lcd.clear (1);   lcd.clear (2);
   setmins ();
   lcd.clear(1);lcd.clear(2);
   setsecs ();
   lcd.clear(1);lcd.clear(2);  
   
   rtc.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,sethourstemp,setminstemp,setsecstemp));
   delay (1000);
  zora0 = 11;
  uora0 = 11;
  zminut0 = 11;
  uminut0 = 11;
  zsec0 = 11;
  usec0 = 11;  
  
}

// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(sw1);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}
// The following subroutines set the individual clock parameters
int setyear () {
//lcd.clear(1);lcd.clear(2);
    lcd.setCursor (0,0);
    lcd.print ("Set Year");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) { //bit of software de-bounce
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setyeartemp=setyeartemp + knobval;
    if (setyeartemp < 2014) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2014;
    }
    lcd.print (setyeartemp);
    lcd.print("  "); 
    setyear();
}
  
int setmonth () {
//lcd.clear(1);lcd.clear(2);
   lcd.setCursor (0,0);
    lcd.print ("Set Month");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setmonthtemp=setmonthtemp + knobval;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 1;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=12;
    }
    lcd.print (setmonthtemp/10);
    lcd.print (setmonthtemp%10);
    lcd.print("  "); 
    setmonth();
}

int setday () {
  if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
    maxday = 28;
  }
//lcd.clear(1);lcd.clear(2);  
   lcd.setCursor (0,0);
    lcd.print ("Set Day");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setdaytemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setdaytemp=setdaytemp+ knobval;
    if (setdaytemp < 1) {
      setdaytemp = 1;
    }
    if (setdaytemp > maxday) {
      setdaytemp = maxday;
    }
    lcd.print (setdaytemp/10);
    lcd.print (setdaytemp%10);
    lcd.print("  "); 
    setday();
}

int sethours () {
//lcd.clear(1);lcd.clear(2);
    lcd.setCursor (0,0);
    lcd.print ("Set Hours");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstemp=sethourstemp + knobval;
    if (sethourstemp < 1) {
      sethourstemp = 1;
    }
    if (sethourstemp > 23) {
      sethourstemp=23;
    }
    lcd.print (sethourstemp/10);
    lcd.print (sethourstemp%10);
    lcd.print("  "); 
    sethours();
}

int setmins () {
//lcd.clear(1);lcd.clear(2);
   lcd.setCursor (0,0);
    lcd.print ("Set Mins");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
      setminstemp = 0;
    }
    if (setminstemp > 59) {
      setminstemp=59;
    }
    lcd.print (setminstemp/10);
    lcd.print (setminstemp%10);
    lcd.print("  "); 
    setmins();
}

int setsecs () {
//lcd.clear(1);lcd.clear(2);
   lcd.setCursor (0,0);
    lcd.print ("Set Seconds");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setsecstemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setsecstemp=setsecstemp + knobval;
    if (setsecstemp < 0) {
      setsecstemp = 0;
    }
    if (setsecstemp > 59) {
      setsecstemp=59;
    }
    lcd.print (setsecstemp/10);
    lcd.print (setsecstemp%10);
    lcd.print("  "); 
    setsecs();
}


void ShortPush () {
  //This displays the calculated sunrise and sunset times when the knob is pushed for a short time.
lcd.clear(1);
lcd.clear(2);

 lcd.print ("Nicu FLORICA (niq_ro) is the best !");
delay(3000);
lcd.clear(1);
lcd.clear(2);
  zora0 = 11;
  uora0 = 11;
  zminut0 = 11;
  uminut0 = 11;
  zsec0 = 11;
  usec0 = 11;  
}
