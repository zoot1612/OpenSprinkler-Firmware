#include <time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SSD1306.h>
#include "OpenSprinkler.h"
#include "espconnect.h"

OpenSprinkler OS;

void setup() {
  // init
  WiFi.persistent(false); // turn off persistent, fixing flash crashing issue

  OS.begin();
	OS.nboards = 3;
	OS.nstations = OS.nboards * 8;
	OS.options[OPTION_BOOST_TIME] = 150;
  byte i;
  if(OS.hw_type==HW_TYPE_LATCH) {
		for(i=8;i<24;i++) {
			OS.lcd.clear();
			OS.lcd.setCursor(0, 0);
			OS.lcd.print("Open ");
			OS.lcd.print(i+1);
		  OS.set_station_bit(i, 1);
		  OS.apply_all_station_bits();
		  delay(250);
			OS.lcd.clear();
			OS.lcd.setCursor(0, 0);
			OS.lcd.print("Close ");		  
			OS.lcd.print(i+1);
		  OS.set_station_bit(i, 0);
		  OS.apply_all_station_bits();
		  delay(250);
		  if(i==15) { 
		  	OS.lcd.clear();
		  	OS.lcd.setCursor(0, 0);
		  	OS.lcd.print("Switch block...");
		  	delay(8000); // 8 seconds delay for switching plug
		  }
		}
  } else {
		for(i=8;i<24;i++) {
			OS.lcd.clear();
			OS.lcd.setCursor(0, 0);
			OS.lcd.print(i+1);
		  OS.set_station_bit(i, 1);
		  OS.apply_all_station_bits();
		  delay(600);
		  OS.set_station_bit(i, 0);
		  delay(50);
		}
		OS.apply_all_station_bits();
	}
	OS.lcd.clear();
	OS.lcd.setCursor(0, 0);
	OS.lcd.print("Test complete");
  // turn on first station
  //
}


void loop() {
  
}
