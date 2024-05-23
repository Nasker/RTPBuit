#include "RTPOled.hpp"

RTPOled::RTPOled(){}

void RTPOled::init(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
}

void RTPOled::introAnimation(int &x, String text){
  display.clearDisplay();
  display.setCursor(x, 0);
  display.setTextSize(8);
  display.print(text);
  display.display();
  x -= 10;
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine){
  if (lastLines != firstLine+secondLine+thirdLine){
    display.clearDisplay();
    display.setTextSize(TEXT_SIZE); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.setCursor(calcOffsetToCenterText(firstLine), 0);
    display.println(firstLine);
    display.setCursor(calcOffsetToCenterText(secondLine), TEXT_SIZE * 10);
    display.println(secondLine);
    display.setCursor(calcOffsetToCenterText(thirdLine), TEXT_SIZE * 20);
    display.println(thirdLine);
    display.display();
    lastLines = firstLine+secondLine+thirdLine;
  }
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}

int RTPOled::calcOffsetToCenterText(String textLine){
  //calculates the offset to center in the display the given textLine
  return textLine.length() * TEXT_SIZE * 10 / 2;
}