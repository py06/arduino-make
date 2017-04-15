/* Thermometer usecase

 My setup:
 - funduino uno
 - SMA420564 4 digits / 7 segments leds display
 - LM35DZ temperature sensor (with analog external ref at 3.3V)

 Copyright 2017 Pierre-Yves Kerbrat

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <stdio.h>
#include <util/delay.h>

#define AREF_VOLTAGE 3.3
#define TEMP_PIN 1
#define LED_PIN 0
#define BACKLIGHT 13
#define LIGHT_SENSOR A0

int led_state = LOW;

/* initialize the library with the numbers of the interface pins */
/* LiquidCrystal lcd(12, 11, 5, 4, 3, 2); */
struct LiquidCrystal lcd;
struct dht DHT;

void setup()
{
	Serial.begin(9600);
	analogReference(EXTERNAL); /*tell analog input to use an external
	       			voltage ref and tie AREF pin to some
	       			voltage (3.3 in my case) */
	lcd_init(&lcd, 0, 12, 10, 11, 9, 8, 7, 6, 5, 4, 3, 2);

	pinMode(BACKLIGHT, OUTPUT);
	digitalWrite(BACKLIGHT, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
	// set up the LCD's number of columns and rows:
	lcd.ops.begin(&lcd, 16, 2, LCD_5x8DOTS);

	/* Configure digital pin 0 as output */
	pinMode(LED_PIN, OUTPUT);
	led_state = LOW;
	digitalWrite(LED_PIN, led_state);

	lcd.ops.clear(&lcd);                  // start with a blank screen
	lcd.ops.setCursor(&lcd, 0, 0);           // set cursor to column 0, row 0 (the first row)
	lcd.ops.print(&lcd, "LM35 v3");    // change text to whatever you like. keep it clean!
	lcd.ops.setCursor(&lcd, 0, 1);           // set cursor to column 0, row 1
	lcd.ops.print(&lcd, "peewhy.net");
	_delay_ms(2000);

}

//static float read_temp()
//{
//	unsigned int valeur_brute = analogRead(TEMP_PIN);
//	float temperature = valeur_brute * (AREF_VOLTAGE / 1023.0 * 10000.0);
//
//	return temperature;
//}
//

static void update_temp(struct LiquidCrystal *lcd, float temp, float hum, int light)
{
	char str[16];
	int valint, valdec;
	lcd->ops.clear(lcd);
	lcd->ops.setCursor(lcd, 0,0);

	memset(str, 0, 16);
	valint = (int)temp;
	valdec = (int)(temp * 10) - (valint * 10);
	sprintf(str, "t: %d.%d C l: %d", valint, valdec, light);
	lcd->ops.print(lcd, str);
	lcd->ops.setCursor(lcd, 0,1);

	memset(str, 0, 16);
	valint = (int)hum;
	valdec = (int)(hum * 10) - (valint * 10);
	sprintf(str, "h: %d.%d %%", valint, valdec);
	lcd->ops.print(lcd, str);
}

int main(int argc, char *argv[])
{
	static unsigned long timer;
	static int deciSeconds = 0;
	float temperature = 0;
	int sts = 0;
	int light = 0;

	init();
#if defined(USBCON)
	USBDevice.attach();
#endif
	setup();
	Serial.println(millis());
	Serial.println(" 2 \n");
	timer = millis();

	for(;;) {
		//temperature = read_temp();
		sts = DHT.ops.read11(&DHT, TEMP_PIN);
		light = analogRead(LIGHT_SENSOR);

		if (millis() >= timer) {
			deciSeconds++; /* 1000 ms is equal to 10 deciSecond */
			timer += 1000;
			if (deciSeconds == 10000) /* Reset to 0 after 1000 seconds. */
				deciSeconds = 0;
			if (led_state == HIGH)
				led_state = LOW;
			else
				led_state = HIGH;
			digitalWrite(LED_PIN, led_state);
			update_temp(&lcd, DHT.temperature, DHT.humidity, light);
		}
		if (serialEventRun) serialEventRun();
	}
}

