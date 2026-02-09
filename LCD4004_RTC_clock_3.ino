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
 * LCD4002_RTC_clock_v1 - moved clock on LCD4002 with i2c interface, based on https://nicuflorica.blogspot.com/2025/10/ceas-rtc-cu-caractetere-mari-si-date.html
 * v1a - smooth transitions between informations, changed clock format 23.45.56 in 23:45:56 
 * v.2 - added name of the day using custon fonts based on https://www.instructables.com/Custom-Large-Font-For-16x2-LCDs/
 * v.2a - corrected display sequence: date - temperature - name of the day
 * v.3 - changed LCD4002 with LCD4004 display (updated big character subrutines)
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


unsigned long timp;
unsigned long timp0 = 200;
int an, luna, zi, zis; 
float temperatura; 
int zzi, uzi, zluna, uluna, zan, uan;  
int sec7 = 55;
int temp, ztemp, utemp, stemp, rtemp;
byte nesters = 0; // 1 - nothing, 0 - need 


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
byte trebuiesters = 0;
int zona = 0;  // 0 - upper part, 1 - lower part

// *******DEFINE CUSTOM CHARACTERS FOR BIG FONT*****************
  byte A[8] =
  {
  B00011,
  B00111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111
  };
  byte B[8] =
  {
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
  };
  byte C[8] =
  {
  B11000,
  B11100,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
  };
  byte D[8] =
  {
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B00111,
  B00011
  };
  byte E[8] =
  {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
  };
  byte F[8] =
  {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11100,
  B11000
  };
  byte G[8] =
  {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
  };
  byte H[8] =
  {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
  };


  
void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("rtc clock on LCD4002 display");
  lcd.begin();
  delay(200); // it´s not clear if this delay is really needed
  lcd.home(); // move the cursor to the top left of the screen
  lcd.backlight(); // enable backlight

   pinMode(sw1,INPUT);//push button on encoder connected to A0 (and GND)
   digitalWrite(sw1,HIGH); //Pull A0 high

  delay(1000);
  lcd.home();
  lcd.backlight();


  
  lcd.createChar(8,A);
  lcd.createChar(6,B);
  lcd.createChar(2,C);
  lcd.createChar(3,D);
  lcd.createChar(7,E);
  lcd.createChar(5,F);
  lcd.createChar(1,G);
  lcd.createChar(4,H);
  lcd.clear(0);
  lcd.clear(1);
  lcd.setCursor(0, 0);
  lcd.print("adjustable RTC clock on LCD4004 display ");
  lcd.setCursor(0, 1);
  lcd.print("sw v.3.0 wrote by Nicu FLORICA (niq_ro) ");

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
  lcd.clear(1); // used just for LCD4004

DateTime now = rtc.now();
sec = now.second();
usec = sec%10;
an = now.year();
luna = now.month();
zi = now.day();
zis = now.dayOfTheWeek();
ora = now.hour();
minut = now.minute();
  zora = ora/10;
  uora = ora%10;
  zminut = minut/10;
  uminut = minut%10;
  zsec = sec/10;
  usec = sec%10;  
zora0 = 11;
uora0 = 11;
zminut0 = 11;
uminut0 = 11;
zsec0 = 11;
usec0 = 11;
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
         lcd.clear(0); lcd.clear(1);
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

  lcd.createChar(8,A);
  lcd.createChar(6,B);
  lcd.createChar(2,C);
  lcd.createChar(3,D);
  lcd.createChar(7,E);
  lcd.createChar(5,F);
  lcd.createChar(1,G);
  lcd.createChar(4,H);
}

  zona  = 0;  // 1st and 2nd rows
  x0 = 0;
  zora = ora/10;
  uora = ora%10;
  zminut = minut/10;
  uminut = minut%10;
  zsec = sec/10;
  usec = sec%10;  

if (zora != zora0)
{
x = x0 + 6;
if (zora == 0)
{
  spatiu();
}
else
  printbig(zora,x,zona);
}

if (uora != uora0)
{
x = x0 + 10;
  printbig(uora,x,zona);

x = x0 + 14;
lcd.setCursor(x,0); 
lcd.write(4);
lcd.setCursor(x,1); 
lcd.write(4);
}

if (zminut != zminut0)
{
x = x0 + 16;
  printbig(zminut,x,zona);
}
if (uminut != uminut0)
{
x = x0 + 20;
  printbig(uminut,x,zona);

  x = x0 + 24;
lcd.setCursor(x,0); 
lcd.write(4);
lcd.setCursor(x,1); 
lcd.write(4);
}

if (zsec != zsec0)
{
x = x0 + 26;
  printbig(zsec,x,zona);
}
if (usec != usec0)
{
x = x0 + 30;
  printbig(usec,x,zona);
}
  zora0 = zora;
  uora0 = uora;
  zminut0 = zminut;
  uminut0 = uminut;
  zsec0 = zsec;
  usec0 = usec;  

if (sec%10 == 0)
{
  zona = 1;  // 2nd and 3rd rows
//  lcd.clear(0);
delay(500);
//  lcd.clear(1);
  
  if (zsec%3 == 0)
  {
  zzi = zi/10;
  uzi = zi%10;
  zluna = luna/10;
  uluna = luna%10;
  zan = (an-2000)/10;
  uan = (an-2000)%10;  
 
//  lcd.clear(0);
  lcd.clear(1);

x0 = 0;
x = x0 + 2;
  printbig(zzi,x,zona);

x = x0 + 6;
  printbig(uzi,x,zona);

x = x0 + 10;
lcd.setCursor(x,3); 
lcd.write(255);

x = x0 + 12;
  printbig(zluna,x,zona);

x = x0 + 16;
  printbig(uluna,x,zona);

x = x0 + 20;
lcd.setCursor(x,3); 
lcd.write(255);

x = x0 + 22;
  printbig(2,x,zona);
x = x0 + 26;
  printbig(0,x,zona);
x = x0 + 30;
  printbig(zan,x,zona);   
x = x0 + 34;
  printbig(uan,x,zona);
 }
 else
   if (zsec%3 == 1)
 {
 temp = (float)(10.*temperatura);
 ztemp = temp/100;
 rtemp = temp%200;
 utemp = rtemp/10;
 stemp = rtemp%10;
// lcd.setCursor(0,0); 
// lcd.print ("Nicu FLORICA (niq_ro) made that !");

 //  lcd.clear(0);
   lcd.clear(1);

x0 = 1;
x = x0 + 5;
//  customplus();

x = x0 + 11;
  printbig(ztemp,x,zona);

x = x0 + 16;
  printbig(utemp,x,zona);
  
x = x0 + 21;
lcd.setCursor(x,3);
lcd.write(255);  

x = x0 + 23;
  printbig(stemp,x,zona);

x = x0 + 28;
  printbig(10,x,zona);

x = x0 + 32;
  printbig(11,x,zona);
 }
 else
  if (zsec%3 == 2)
 {
 // lcd.clear(0);
   lcd.clear(1);
  x = 2;
  WeekDay();
 }
delay(500);
} // date or temperature or day

timp = millis();
}

delay(100);
}  // end main loop


void spatiu()
{ // uses segments to clear a number - https://www.seetron.com/bpk000/bpk000prog.html
  lcd.setCursor(x,0); 
  lcd.print("   ");
  lcd.setCursor(x,1); 
  lcd.print("   ");   
}

// that parts is for adjust the clock/time

//sets the clock
void setclock (){
   setyear ();
   lcd.clear (0);   lcd.clear (1);
   setmonth ();
   lcd.clear (0);   lcd.clear (1);
   setday ();
   lcd.clear (0);   lcd.clear (1);
   sethours ();
   lcd.clear (0);   lcd.clear (1);
   setmins ();
   lcd.clear(0);   lcd.clear(1);
   setsecs ();
   lcd.clear(0);   lcd.clear(1);  
   
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
      sethourstemp = 0;
    }
    if (sethourstemp > 23) {
      sethourstemp = 23;
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
      setsecstemp = 59;
    }
    lcd.print (setsecstemp/10);
    lcd.print (setsecstemp%10);
    lcd.print("  "); 
    setsecs();
}


void ShortPush () {
  //This displays the calculated sunrise and sunset times when the knob is pushed for a short time.
lcd.clear(0);

//lcd.clear(1);
lcd.setCursor (0,0);
 lcd.print ("   Nicu FLORICA (niq_ro) made it !     ");
delay(3000);
lcd.clear(0);
//
//lcd.clear(2);
  zora0 = 11;
  uora0 = 11;
  zminut0 = 11;
  uminut0 = 11;
  zsec0 = 11;
  usec0 = 11;  
  nesters = 0;
}

void printbig(int i, int x, int ecran)
{
  //  prints each segment of the big numbers

  if (i == 0) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);  
      lcd.write(1); 
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(3);  
      lcd.write(4);  
      lcd.write(5);
  }
    else if  (i == 1) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);
      lcd.write(255);
      lcd.print(" ");
      lcd.setCursor(x,1+2*ecran);
      lcd.print(" ");
      lcd.write(255);
      lcd.print(" ");
    }

    else if  (i == 2) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(1);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(3);
      lcd.write(7);
      lcd.write(4);
    }

    else if (i == 3) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(1);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(4);
      lcd.write(7);
      lcd.write(5); 
    }

    else if (i == 4) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(3);
      lcd.write(4);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.print("  ");
      lcd.write(5);
    }

    else if (i == 5) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(255);
      lcd.write(6);
      lcd.write(1);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
    }

    else if (i == 6) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);
      lcd.write(6);
      lcd.print(" ");
      lcd.setCursor(x,1+2*ecran);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
    }

    else if (i == 7) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(1);
      lcd.write(1);
      lcd.write(5);
      lcd.setCursor(x,1+2*ecran);
      lcd.print(" ");
      lcd.write(8);
      lcd.print(" ");
    }
    
    else if (i == 8) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
    }

    else if (i == 9) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.print(" ");
      lcd.write(4);
      lcd.write(5);
    }
else if (i == 10) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x,1+2*ecran);
      lcd.print(" ");
   //   lcd.write(1);
      lcd.print(" ");
      lcd.print(" ");
    }
    
      if (i == 11) {
      lcd.setCursor(x,0+2*ecran);
      lcd.write(8);  
      lcd.write(1); 
      lcd.write(1);
      lcd.setCursor(x,1+2*ecran);
      lcd.write(3);  
      lcd.write(4);  
      lcd.write(4);
     }
  }

void customA(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255);
}

void customB(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(7);
  lcd.write(2);
}

void customC(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}

void customD(int ecran)
{
  lcd.setCursor(x,0+2*ecran); 
  lcd.write(255);  
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran); 
  lcd.write(255);  
  lcd.write(4);  
  lcd.write(5);
}

void customE(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(7);
  lcd.write(7); 
}

void customF(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
}

void customG(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
}

void customH(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(4);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255); 
}

void customI(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(1);
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void customJ(int ecran)
{
  lcd.setCursor(x+2,0+2*ecran);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}

void customK(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void customL(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(4);
  lcd.write(4);
}

void customM(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(3);
  lcd.write(5);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
}

void customN(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(2);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(3);
  lcd.write(5);
}


void customO(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void customP(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
}

void customQ(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void customR(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void customS(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(8);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}

void customT(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(1);
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(254);
  lcd.write(255);
}

void customU(int ecran)
{
  lcd.setCursor(x,0+2*ecran); 
  lcd.write(255);  
  lcd.write(254); 
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void customV(int ecran)
{
  lcd.setCursor(x,0+2*ecran); 
  lcd.write(3);  
  lcd.write(254);
  lcd.write(254); 
  lcd.write(5);
  lcd.setCursor(x+1,1+2*ecran); 
  lcd.write(2);  
  lcd.write(8);
}

void customW(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(3);
  lcd.write(8);
  lcd.write(2);
  lcd.write(5);
}

void customX(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(8);
  lcd.write(254);
  lcd.write(2); 
}

void customY(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x+1,1+2*ecran);
  lcd.write(255);
}

void customZ(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(1);
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(8);
  lcd.write(7);
  lcd.write(4);
}

void customqm(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(254);
  lcd.write(7);
}

void customsm(int ecran)
{
  lcd.setCursor(x,0+2*ecran);
  lcd.write(255);
  lcd.setCursor(x,1+2*ecran);
  lcd.write(7);
}

void Sunday()
{
     customS(zona);
     x = x + 4;
     customU(zona);
     x = x + 4;
     customN(zona);
     x = x + 5;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);
}

void Monday()
{
     customM(zona);
     x = x + 4;
     customO(zona);
     x = x + 4;
     customN(zona);
     x = x + 5;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);
}

void Tuesday()
{
     customT(zona);
     x = x + 4;
     customU(zona);
     x = x + 4;
     customE(zona);
     x = x + 4;
     customS(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);
}

void Wednesday()
{
     customW(zona);
     x = x + 5;
     customE(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customN(zona);
     x = x + 5;
     customE(zona);
     x = x + 4;
     customS(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);  
}

void Thursday()
{
     customT(zona);
     x = x + 4;
     customH(zona);
     x = x + 4;
     customU(zona);
     x = x + 4;
     customR(zona);
     x = x + 4;
     customS(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);   
}

void Friday()
{
     customF(zona);
     x = x + 4;
     customR(zona);
     x = x + 4;
     customI(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);
}

void Saturday()
{
     customS(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customT(zona);
     x = x + 4;
     customU(zona);
     x = x + 4;
     customR(zona);
     x = x + 4;
     customD(zona);
     x = x + 4;
     customA(zona);
     x = x + 4;
     customY(zona);
}

void WeekDay()
{
  if (zis == 0)
     Sunday();
  else
    if (zis == 1)
     Monday();
  else
    if (zis == 2)
     Tuesday();
  else
    if (zis == 3)
     Wednesday();
  else
    if (zis == 4)
     Thursday();
  else
    if (zis == 5)
     Friday();
  else
    if (zis == 6)
     Saturday();
}
