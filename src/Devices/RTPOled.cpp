#include "RTPOled.hpp"

RTPOled::RTPOled(){}

void RTPOled::init(){
  Wire.end();
  display.begin();
  Wire.begin();
  display.setFont(u8g2_font_fub35_tf);
}

void RTPOled::introAnimation(int &x, String text){
  Wire.end();
  display.firstPage();
  do {
    display.setCursor(x, 55);
    display.print(text);
  } while (display.nextPage());
  x -= 10;
  Wire.begin();
}

void RTPOled::setAfterIntro(){
  display.setFont(u8g2_font_DigitalDisco_tf);
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine){
  if (lastLines != firstLine+secondLine+thirdLine){
    Wire.end();
    display.firstPage();
    do {
      //display.drawFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      display.setCursor(calcOffsetToCenterText(firstLine), 10);
      display.println(firstLine);
      display.drawHLine(0, 15, SCREEN_WIDTH);
      display.setCursor(calcOffsetToCenterText(secondLine), 30);
      display.println(secondLine);
      display.setCursor(calcOffsetToCenterText(thirdLine), 45);
      display.println(thirdLine);
    } while (display.nextPage());
    lastLines = firstLine+secondLine+thirdLine;
    Wire.begin();
  }
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}

int RTPOled::calcOffsetToCenterText(String textLine){
  int textWidth = textLine.length() * 10; // Adjust based on font width
  return (SCREEN_WIDTH - textWidth) / 2;
}