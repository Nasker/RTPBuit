#include "RTPOled.hpp"

RTPOled::RTPOled(){}

void RTPOled::init(){
  Wire.end();
  display.begin();
  display.setBusClock(400000);   // fast-mode I2C: ~4x quicker full redraws
  Wire.begin();
  Wire.setClock(400000);         // Wire.begin() resets to 100 kHz; restore for the seesaw
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
  Wire.begin();                  // back to 100 kHz for the seesaw
}

void RTPOled::setAfterIntro(){
  display.setFont(u8g2_font_DigitalDisco_tf);
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine){
  _queue(firstLine, secondLine, thirdLine, "", false, false, SequenceDisplayState::Stopped, false, false);
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, bool isRecording){
  _queue(firstLine, secondLine, thirdLine, fourthLine, true, isRecording, SequenceDisplayState::Stopped, false, false);
}

void RTPOled::printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, SequenceDisplayState state, bool blinkState){
  _queue(firstLine, secondLine, thirdLine, fourthLine, true, false, state, blinkState, true);
}

void RTPOled::printToScreen(ControlCommand command){
    printToScreen("->ID: " + String(command.controlType),"->CMD: " + String(command.commandType),"->VAL: " + String(command.value));
}

// Record the latest requested frame and mark dirty. No I2C happens here, so
// input handlers stay cheap; the actual redraw is done once per loop in flush().
void RTPOled::_queue(const String& l1, const String& l2, const String& l3, const String& l4,
                     bool fourLine, bool isRecording, SequenceDisplayState state, bool blink, bool useState){
  String key = l1 + l2 + l3 + l4 +
               (useState ? String(static_cast<int>(state)) + String(blink) : String(isRecording));
  if (key == lastLines) return;   // already on screen
  _pend.line[0] = l1; _pend.line[1] = l2; _pend.line[2] = l3; _pend.line[3] = l4;
  _pend.fourLine    = fourLine;
  _pend.isRecording = isRecording;
  _pend.state       = state;
  _pend.blink       = blink;
  _pend.useState    = useState;
  _pend.key         = key;
  _dirty            = true;
}

// Incremental render: firstPage()/drawing are RAM-only; only nextPage() pushes
// a page over I2C. Sending one page per call keeps each loop's blocking I2C to
// ~1 page (~3 ms at 400 kHz) instead of a full ~23 ms frame, and lets the
// trellis share the bus between pages.
void RTPOled::flush(){
  if (!_rendering) {
    if (!_dirty) return;
    _dirty = false;
    _act = _pend;                 // snapshot so a new _queue can't tear the frame
    _rendering = true;
    display.firstPage();          // reset to page 0 (RAM only)
  }
  _drawFrame();                   // fill the current page buffer (RAM only)
  bool more = display.nextPage(); // u8g2 sends this page at 400 kHz
  Wire.setClock(100000);          // hand the bus back to the seesaw at a safe speed
  if (!more) {                    // false once all pages sent
    _rendering = false;
    lastLines = _act.key;
    Wire.begin();                 // re-init once per frame (recovery); leaves 100 kHz
  }
}

void RTPOled::_drawFrame(){
    // Top-right indicator
    if (_act.useState) {
      switch (_act.state) {
        case SequenceDisplayState::Playing:
          display.drawTriangle(
            SCREEN_WIDTH - 12, 2,
            SCREEN_WIDTH - 12, 10,
            SCREEN_WIDTH - 4, 6
          );
          break;
        case SequenceDisplayState::Stopped:
          display.drawBox(SCREEN_WIDTH - 12, 2, 8, 8);
          break;
        case SequenceDisplayState::Waiting:
          if (_act.blink) display.drawDisc(SCREEN_WIDTH - 8, 6, 4);
          else            display.drawCircle(SCREEN_WIDTH - 8, 6, 4);
          break;
        case SequenceDisplayState::Recording:
          display.drawDisc(SCREEN_WIDTH - 8, 6, 4);
          break;
      }
    } else if (_act.isRecording) {
      display.drawDisc(SCREEN_WIDTH - 8, 6, 4);
    }

    display.setCursor(calcOffsetToCenterText(_act.line[0]), 10);
    display.println(_act.line[0]);
    display.drawHLine(0, 15, SCREEN_WIDTH);

    if (_act.fourLine) {
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 27);
      display.println(_act.line[1]);
      display.setCursor(calcOffsetToCenterText(_act.line[2]), 44);
      display.println(_act.line[2]);
      display.setCursor(calcOffsetToCenterText(_act.line[3]), 60);
      display.println(_act.line[3]);
    } else {
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 30);
      display.println(_act.line[1]);
      display.setCursor(calcOffsetToCenterText(_act.line[2]), 45);
      display.println(_act.line[2]);
    }
}

u8g2_int_t RTPOled::calcOffsetToCenterText(String textLine){
  u8g2_int_t textWidth = display.getStrWidth(textLine.c_str());
  return (SCREEN_WIDTH - textWidth) / 2;
}