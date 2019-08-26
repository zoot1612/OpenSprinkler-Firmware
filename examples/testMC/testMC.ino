#include <Wire.h>
#include <SSD1306.h>
#include <ESP8266WiFi.h>
#include <FS.h>

bool detect_i2c(int addr);

#define PCF_ADDR1 0x21
#define PCF_ADDR2	0x22
#define RTC_ADDR  0x51
#define LCD_ADDR  0x3c

#define NXP_INPUT_REG  0
#define NXP_OUTPUT_REG 2
#define NXP_INVERT_REG 4
#define NXP_CONFIG_REG 6

#define PASS_MSG  "pass"
#define FAIL_MSG  "FAIL!!!!!!"

#define IOEXP_PIN 0x80
#define BUTTON_1  2
#define BUTTON_2  0
#define BUTTON_3  IOEXP_PIN+12

#define HEIGHT 12
#define LINE1 4
#define LINE2 (LINE1+HEIGHT)
#define LINE3 (LINE2+HEIGHT)
#define LINE4 (LINE3+HEIGHT)
#define LINE5	(LINE4+HEIGHT)

SSD1306 display(0x3c, SDA, SCL);
byte buttons=0;
byte PCF_ADDR;

uint16_t i2c_read(uint8_t address, uint8_t reg) {
  if(address==255)  return 0xFFFF;
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  if(Wire.requestFrom(address, (uint8_t)2) != 2) {return 0xFFFF;}
  uint16_t data0 = Wire.read();
  uint16_t data1 = Wire.read();
  return data0+(data1<<8);
}

void i2c_write(uint8_t address, uint8_t reg, uint16_t v){
  if(address==255)  return;
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(v&0xff);
  Wire.write(v>>8);
  Wire.endTransmission();
}

uint8_t readIOEXP(uint8_t pin) {
	return (i2c_read(PCF_ADDR, NXP_INPUT_REG)>>(pin-IOEXP_PIN))&0x01;
}

void drawPassFail(bool result, int line) {
	if(!result) {
		display.fillRect(50, line, 50, HEIGHT);
		display.setColor(BLACK);
	}
	display.drawString(50, line, result?PASS_MSG:FAIL_MSG);
	display.setColor(WHITE);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  display.init();
  display.flipScreenVertically();  
  
  display.drawString(0, LINE2, "Init Flash...");
  display.display();
  if(!SPIFFS.begin()) {
  	display.drawString(0, LINE3, FAIL_MSG);
  	display.display();
  	while(1) { yield(); }
  } else {
  	display.drawString(0, LINE3, PASS_MSG);
		display.display();
	}
	delay(500);  	

  // check I2C components
  display.clear();
  display.drawString(0,LINE1,"PCF8563:");
  drawPassFail(detect_i2c(RTC_ADDR), LINE1);
  display.drawString(0,LINE2,"PCF9555:");
	drawPassFail(detect_i2c(PCF_ADDR1)|detect_i2c(PCF_ADDR2), LINE2);
  display.display();

	if(detect_i2c(PCF_ADDR2)) PCF_ADDR = PCF_ADDR2;
	else PCF_ADDR = PCF_ADDR1;
  i2c_write(PCF_ADDR, NXP_CONFIG_REG, 0x9F00);
  i2c_write(PCF_ADDR, NXP_OUTPUT_REG, 0x9F00);	

	pinMode(BUTTON_1, INPUT_PULLUP);
	pinMode(BUTTON_2, INPUT_PULLUP);
	
  display.drawString(0,LINE3,"Click all buttons:");
  display.drawString(0,LINE4,"ADC: ");
  display.display();
}

void loop() {
  // check buttons
  if(digitalRead(BUTTON_1)==LOW) {
    display.drawString(75,LINE3,"B1");
    buttons|=0x01;
  }
  if(digitalRead(BUTTON_2)==LOW) {
    display.drawString(90,LINE3,"B2");
    buttons|=0x02;
  }
  if(readIOEXP(BUTTON_3)==LOW) {
    display.drawString(105,LINE3,"B3");
    buttons|=0x04;
  }
  if(buttons==0x07) {
    display.drawString(0,LINE5,"===Test Completed===");
  }
  // check ADC
  uint16_t adc = analogRead(A0);
  String sadc(adc);
  display.setColor(BLACK);
  display.fillRect(30,LINE4,100,10);
  display.setColor(WHITE);  
  display.drawString(30,LINE4, sadc);
  drawPassFail((adc<13), LINE4);
  display.display();
  delay(100);
}
