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
  _contentFont = u8g2_font_DigitalDisco_tf;
  display.setFont(_contentFont);
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

// Update only the transversal status strip. Cheap: just stores the snapshot and
// re-evaluates dirty, so it can be polled every loop without triggering redraws.
void RTPOled::setHud(const HudModel& hud){
  _pend.hud = hud;
  _refreshDirty();
}

// Set the trellis-pad legend for this screen. Last-write-wins before flush; a
// new _queue() clears it, so presenters call this after their print* call.
void RTPOled::setPadLegend(const PadHint* hints, uint8_t count, int8_t focusedPad){
  if (count > MAX_PAD_HINTS) count = MAX_PAD_HINTS;
  _pend.numHints = 0;
  for (uint8_t i = 0; i < count; i++) {
    _pend.hints[i] = hints[i];
    _pend.numHints++;
  }
  _pend.focusedHint = focusedPad;
  _refreshDirty();
}

void RTPOled::setGrid(uint16_t fillMask, uint8_t count, int8_t highlight){
  _pend.gridFill   = fillMask;
  _pend.gridCount  = (count > 16) ? 16 : count;
  _pend.gridCursor = highlight;
  _pend.showGrid   = true;
  _refreshDirty();
}

// Record the latest requested frame and mark dirty. No I2C happens here, so
// input handlers stay cheap; the actual redraw is done once per loop in flush().
void RTPOled::_queue(const String& l1, const String& l2, const String& l3, const String& l4,
                     bool fourLine, bool isRecording, SequenceDisplayState state, bool blink, bool useState){
  _pend.line[0] = l1; _pend.line[1] = l2; _pend.line[2] = l3; _pend.line[3] = l4;
  _pend.fourLine    = fourLine;
  _pend.isRecording = isRecording;
  _pend.state       = state;
  _pend.blink       = blink;
  _pend.useState    = useState;
  _pend.numHints    = 0;          // a fresh content frame resets the legend
  _pend.focusedHint = -1;
  _pend.showGrid    = false;      // ... and the cell grid
  _pend.gridFill    = 0;
  _pend.gridCount   = 0;
  _pend.gridCursor  = -1;
  _refreshDirty();
}

void RTPOled::_refreshDirty(){
  _dirty = !_framesEqual(_pend, _shown);
}

bool RTPOled::_framesEqual(const Frame& a, const Frame& b) const{
  if (a.fourLine != b.fourLine || a.useState != b.useState ||
      a.isRecording != b.isRecording || a.state != b.state ||
      a.blink != b.blink || a.hud != b.hud ||
      a.numHints != b.numHints || a.focusedHint != b.focusedHint ||
      a.showGrid != b.showGrid || a.gridFill != b.gridFill ||
      a.gridCount != b.gridCount || a.gridCursor != b.gridCursor)
    return false;
  for (int i = 0; i < 4; i++)
    if (a.line[i] != b.line[i]) return false;
  for (uint8_t i = 0; i < a.numHints; i++)
    if (a.hints[i].pad != b.hints[i].pad ||
        strcmp(a.hints[i].label, b.hints[i].label) != 0)
      return false;
  return true;
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
    _shown = _act;                // this frame is now on screen
    Wire.begin();                 // re-init once per frame (recovery); leaves 100 kHz
  }
}

void RTPOled::_drawFrame(){
    _drawStatusStrip();

    // Title (signature font), then compact content lines below the strip.
    display.setFont(_contentFont);
    display.setCursor(calcOffsetToCenterText(_act.line[0]), 28);
    display.println(_act.line[0]);

    display.setFont(u8g2_font_9x6LED_tr);
    if (_act.numHints > 0) {
      // Legend screen: title + one value line, then the pad legend grid.
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 41);
      display.println(_act.line[1]);
      _drawPadLegend();
    } else if (_act.showGrid) {
      // Grid screen: title + name line (lower, more air), then a cell row.
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 41);
      display.println(_act.line[1]);
      _drawCellRow();
    } else if (_act.fourLine) {
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 39);
      display.println(_act.line[1]);
      display.setCursor(calcOffsetToCenterText(_act.line[2]), 49);
      display.println(_act.line[2]);
      display.setCursor(calcOffsetToCenterText(_act.line[3]), 59);
      display.println(_act.line[3]);
    } else {
      display.setCursor(calcOffsetToCenterText(_act.line[1]), 42);
      display.println(_act.line[1]);
      display.setCursor(calcOffsetToCenterText(_act.line[2]), 54);
      display.println(_act.line[2]);
    }
}

// Mini 4-column grid echoing the trellis layout: each active pad is a labeled
// cell at its physical position (col = pad%4, row = pad/4). The focused pad is
// drawn filled/inverted so the user sees which control the rotary will adjust.
void RTPOled::_drawPadLegend(){
    const int cellW = 30, cellH = 9, gapX = 2, top = 45, rowH = 9;
    display.setFont(u8g2_font_5x7_tr);
    for (uint8_t i = 0; i < _act.numHints; i++) {
      const PadHint& h = _act.hints[i];
      int col = h.pad % 4, row = h.pad / 4;
      if (row > 1) continue;                 // legend covers the two active trellis rows
      int x = 1 + col * (cellW + gapX);
      int y = top + (1 - row) * rowH;        // trellis row 0 is the bottom row -> mirror it
      bool focused = (h.pad == _act.focusedHint);
      if (focused) {
        display.drawRBox(x, y, cellW, cellH, 2);
        display.setDrawColor(0);
      } else {
        display.drawRFrame(x, y, cellW, cellH, 2);
      }
      int tw = display.getStrWidth(h.label);
      display.setCursor(x + (cellW - tw) / 2, y + cellH - 2);
      display.print(h.label);
      if (focused) display.setDrawColor(1);
    }
}

// Single centered row of cells for the position/context view. Cell size adapts
// to the count (bigger when few, smaller when many). Filled = has content, the
// gridCursor cell gets a contrasting inner frame so it reads in either state.
void RTPOled::_drawCellRow(){
    uint8_t n = _act.gridCount;
    if (n == 0) return;
    if (n > 16) n = 16;
    const int gap = 3, maxW = 122, top = 50;
    int cell = (maxW - (n - 1) * gap) / n;   // widest cell that fits
    if (cell > 11) cell = 11;                // cap so few cells stay chunky
    if (cell < 4)  cell = 4;
    int rowW = n * cell + (n - 1) * gap;
    int left = (SCREEN_WIDTH - rowW) / 2;
    for (uint8_t i = 0; i < n; i++) {
      int x = left + i * (cell + gap);
      bool on  = (_act.gridFill >> i) & 1;
      bool cur = (i == _act.gridCursor);
      if (on) {
        display.drawBox(x, top, cell, cell);
        if (cur) { display.setDrawColor(0); display.drawFrame(x + 1, top + 1, cell - 2, cell - 2); display.setDrawColor(1); }
      } else {
        display.drawFrame(x, top, cell, cell);
        if (cur) display.drawFrame(x + 1, top + 1, cell - 2, cell - 2);
      }
    }
}

// Transversal status strip: transport glyph + clock + scene + sequence + channel.
// Same on every screen so the current context is always visible.
void RTPOled::_drawStatusStrip(){
    const HudModel& h = _act.hud;

    // Transport glyph (top-left)
    switch (h.transport) {
      case SequenceDisplayState::Playing:
        display.drawTriangle(1, 1, 1, 9, 8, 5);
        break;
      case SequenceDisplayState::Stopped:
        display.drawBox(1, 1, 8, 8);
        break;
      case SequenceDisplayState::Waiting:
        if (h.blink) display.drawDisc(5, 5, 4);
        else         display.drawCircle(5, 5, 4);
        break;
      case SequenceDisplayState::Recording:
        display.drawDisc(5, 5, 4);
        break;
    }

    // Four fields after the glyph: BPM-or-EXT, beat meter, scene, active seqs.
    display.setFont(u8g2_font_5x7_tr);
    const int baseY = 8;                       // text baseline within the strip

    String bpmStr   = h.syncInternal ? String((int)h.bpm) : String("EXT");
    String sceneStr = "S" + String(h.scene + 1);
    String actStr   = String(h.activeSeq);
    const int beatW    = 4 * 4 + 3;            // 4 cells of 4px + 1px gaps = 19
    const int actGlyphW = 5;                   // 2x2 mini-grid glyph

    int w0 = display.getStrWidth(bpmStr.c_str());
    int w1 = beatW;
    int w2 = display.getStrWidth(sceneStr.c_str());
    int w3 = actGlyphW + 1 + display.getStrWidth(actStr.c_str());

    // Justify: first field at the left margin, last flush right, equal gaps.
    const int left = 13, right = SCREEN_WIDTH - 1;
    int gap = (right - left - (w0 + w1 + w2 + w3)) / 3;
    int x0 = left;
    int x1 = x0 + w0 + gap;
    int x2 = x1 + w1 + gap;
    int x3 = x2 + w2 + gap;

    display.setCursor(x0, baseY); display.print(bpmStr);

    // Beat meter: 4 cells, the current beat filled.
    for (uint8_t i = 0; i < 4; i++) {
      int dx = x1 + i * 5;
      if (i == (h.beat & 3)) display.drawBox(dx, 2, 4, 6);
      else                   display.drawFrame(dx, 2, 4, 6);
    }

    display.setCursor(x2, baseY); display.print(sceneStr);

    // Active sequences: 2x2 mini-grid glyph + count.
    display.drawBox(x3,     2, 2, 2); display.drawBox(x3 + 3, 2, 2, 2);
    display.drawBox(x3,     5, 2, 2); display.drawBox(x3 + 3, 5, 2, 2);
    display.setCursor(x3 + actGlyphW + 1, baseY); display.print(actStr);

    display.drawHLine(0, 12, SCREEN_WIDTH);
}

u8g2_int_t RTPOled::calcOffsetToCenterText(String textLine){
  u8g2_int_t textWidth = display.getStrWidth(textLine.c_str());
  return (SCREEN_WIDTH - textWidth) / 2;
}