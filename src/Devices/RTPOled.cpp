#include "RTPOled.hpp"
#include "Fonts/all_fonts.h"

RTPOled::RTPOled(){}

void RTPOled::init(){
  display.begin(SH1106_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setRotation(2);
  display.setTextSize(TEXT_SIZE);
  display.setFont(&FreeSansBold24pt7b);
  display.setTextColor(WHITE);
}

void RTPOled::introAnimation(int &x, String text){
  display.clearDisplay();
  display.setCursor(x, -10);
  display.print(text);
  display.display();
  x -= 10;
}

void RTPOled::setAfterIntro(){
  display.setFont(&FreeSans9pt7b);
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine){
  if (lastLines != firstLine+secondLine+thirdLine){
    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.setCursor(calcOffsetToCenterText(firstLine), TEXT_SIZE * 15);
    display.println(firstLine);
    display.setCursor(calcOffsetToCenterText(secondLine), TEXT_SIZE * 30);
    display.println(secondLine);
    display.setCursor(calcOffsetToCenterText(thirdLine), TEXT_SIZE * 45);
    display.println(thirdLine);
    display.display();
    lastLines = firstLine+secondLine+thirdLine;
  }
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}

int RTPOled::calcOffsetToCenterText(String textLine){
  int textWidth = textLine.length() * TEXT_SIZE * 12;
  return (SCREEN_WIDTH - textWidth) / 2;
}