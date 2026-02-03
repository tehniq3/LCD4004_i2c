# LCD4004_i2c
based on info from https://forum.arduino.cc/t/pcf8574-i2c-adapter-modified-for-lcd-4004-with-clear/985110 and https://www.crystalfontz.com/products/document/5152/CFAH4004A1-TFH-JT_Datasheet.pdf

My articles:
- 1st article: https://nicuflorica.blogspot.com/2026/01/afisaj-alfanumeric-lcd4004-controlat.html
- 2nd article: (https://nicuflorica.blogspot.com/2026/01/afisaj-alfanumeric-lcd4004-controlat_25.html
- 3rd article: Nicu FLORICA (niq_ro): https://nicuflorica.blogspot.com/2026/02/ceas-rtc-cu-cifre-uriase-pe-afisaj-i2c.html

![schematic](https://github.com/tehniq3/LCD4004_i2c/blob/main/docs/LCD4004_i2c_wirring_sch.png)

NOTE:
- Best results (at list for yellow-green display) was to decreased background LEDs current with series 470Ω resistor.
- For higher contrast Vo (pin 12 from LCD) dissconnect from i2c interfce but connect to GND. 

![RTC clock](https://github.com/tehniq3/LCD4004_i2c/blob/main/docs/RTC_LCD4004_i2c_wirring_sch.png)


![thin](https://github.com/tehniq3/LCD4004_i2c/blob/main/docs/thin.png)

![thick](https://github.com/tehniq3/LCD4004_i2c/blob/main/docs/thick.png)



