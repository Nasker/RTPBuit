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

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, bool isRecording){
  if (lastLines != firstLine+secondLine+thirdLine+fourthLine+String(isRecording)){
    Wire.end();
    display.firstPage();
    do {
      // Draw recording indicator if needed
      if (isRecording) {
        // Draw a filled circle in the top right corner to indicate recording
        // Moved up slightly for better visual weighting
        display.drawDisc(SCREEN_WIDTH - 8, 6, 4);
      }
      
      // Draw the four lines of text
      display.setCursor(calcOffsetToCenterText(firstLine), 10);
      display.println(firstLine);
      display.drawHLine(0, 15, SCREEN_WIDTH);
      
      display.setCursor(calcOffsetToCenterText(secondLine), 27); // Moved down from 25
      display.println(secondLine);
      
      display.setCursor(calcOffsetToCenterText(thirdLine), 44); // Moved down more
      display.println(thirdLine);
      
      display.setCursor(calcOffsetToCenterText(fourthLine), 60); // Moved down more
      display.println(fourthLine);
    } while (display.nextPage());
    
    lastLines = firstLine+secondLine+thirdLine+fourthLine+String(isRecording);
    Wire.begin();
  }
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, SequenceDisplayState state, bool blinkState){
  String stateStr = String(static_cast<int>(state)) + String(blinkState);
  if (lastLines != firstLine+secondLine+thirdLine+fourthLine+stateStr){
    Wire.end();
    display.firstPage();
    do {
      // Draw indicator based on state
      switch (state) {
        case SequenceDisplayState::Playing:
          // Play triangle: pointing right at top right
          display.drawTriangle(
            SCREEN_WIDTH - 12, 2,    // top vertex
            SCREEN_WIDTH - 12, 10,   // bottom vertex
            SCREEN_WIDTH - 4, 6      // right vertex (point)
          );
          break;
          
        case SequenceDisplayState::Stopped:
          // Stop square: filled rectangle at top right
          display.drawBox(SCREEN_WIDTH - 12, 2, 8, 8);
          break;
          
        case SequenceDisplayState::Waiting:
          // Blinking REC circle: filled when blinkState true, outline when false
          if (blinkState) {
            display.drawDisc(SCREEN_WIDTH - 8, 6, 4);  // Filled circle
          } else {
            display.drawCircle(SCREEN_WIDTH - 8, 6, 4); // Outline only
          }
          break;
          
        case SequenceDisplayState::Recording:
          // Solid REC circle
          display.drawDisc(SCREEN_WIDTH - 8, 6, 4);
          break;
      }
      
      // Draw the four lines of text
      display.setCursor(calcOffsetToCenterText(firstLine), 10);
      display.println(firstLine);
      display.drawHLine(0, 15, SCREEN_WIDTH);
      
      display.setCursor(calcOffsetToCenterText(secondLine), 27);
      display.println(secondLine);
      
      display.setCursor(calcOffsetToCenterText(thirdLine), 44);
      display.println(thirdLine);
      
      display.setCursor(calcOffsetToCenterText(fourthLine), 60);
      display.println(fourthLine);
    } while (display.nextPage());
    
    lastLines = firstLine+secondLine+thirdLine+fourthLine+stateStr;
    Wire.begin();
  }
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}

u8g2_int_t RTPOled::calcOffsetToCenterText(String textLine){
  u8g2_int_t textWidth = display.getStrWidth(textLine.c_str());
  return (SCREEN_WIDTH - textWidth) / 2;
}