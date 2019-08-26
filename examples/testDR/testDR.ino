#include <Wire.h>
#include <SSD1306.h>
#include <ESP8266WiFi.h>
#include "OpenSprinkler.h"

OpenSprinkler OS;

void show_voltage() {
  OS.lcd.setCursor(0,2);
  OS.lcd.clear(2,2);
  float adc = analogRead(A0);
	float vread = (adc*(47+1.5)/1.5/1024);
	if(vread<4.7 || vread>5.4) {
		OS.lcd.fillRect(0, 48, 128, 16);
		OS.lcd.setColor(BLACK);
		OS.lcd.print(vread);
		OS.lcd.print("V");
		OS.lcd.setColor(WHITE);
	} else {
		OS.lcd.print(vread);
		OS.lcd.print("V");
	}
}

bool detect_i2c(int addr);

void setup() {
  // init
  WiFi.persistent(false); // turn off persistent, fixing flash crashing issue
	WiFi.mode(WIFI_OFF);
	
  Serial.begin(115200);

  OS.begin();
  OS.options[OPTION_BOOST_TIME]=125;
  OS.lcd.clear();
  OS.lcd.setCursor(0,-1);
  OS.lcd.print("....Testing....");
  OS.lcd.setCursor(0,0);
  
  OS.lcd.print("HW TYPE: ");
  if(detect_i2c(DCDR_I2CADDR)) OS.lcd.print("DC");
  else if(detect_i2c(ACDR_I2CADDR)) OS.lcd.print("AC");
  else OS.lcd.print("FAIL!!!");

  for(byte i=0;i<8;i++) {
    OS.lcd.setCursor(0,1);
    OS.lcd.clear(1,1);
    OS.lcd.print("Test zone: ");
    OS.lcd.print(i+1);

    OS.set_station_bit(i, 1);
    OS.apply_all_station_bits();
    show_voltage();

    unsigned long t = millis();
    while(t+300>millis()) {
      delay(50);
    }
    OS.set_station_bit(i, 0);
  }
  OS.options[OPTION_BOOST_TIME]=0;
  for(byte i=0;i<8;i++) {
  	OS.set_station_bit(i, 1);
  }
  OS.apply_all_station_bits();
  
  OS.lcd.setCursor(0,-1);
  OS.lcd.clear(-1,-1);
  OS.lcd.print("Test Completed.");

}

void loop() {

}
