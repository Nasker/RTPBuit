#include "RTPOled.hpp"

RTPOled::RTPOled(){}

void RTPOled::init(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine){
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(firstLine);
  display.setCursor(0, 20);
  display.println(secondLine);
  display.setCursor(0, 40);
  display.println(thirdLine);
  display.display();
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}