#include "RTPNeoTrellis.hpp"
#include "RTPMainUnit.hpp"
#include "RTPTypeColors.h"
#include "ColorFunctions.h"


Adafruit_NeoTrellis RTPNeoTrellis::myTrellis;
RTPMainUnit* RTPNeoTrellis::mainUnit;

const int convertMatrix[16] = {0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15};

RTPNeoTrellis::RTPNeoTrellis(){
  pinMode(TRELLIS_INT_PIN, INPUT);
}

TrellisCallback RTPNeoTrellis::blink(keyEvent evt){  
  String evtString;
  ControlCommand callbackCommand;
  callbackCommand.controlType = TRELLIS;
  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING){
    callbackCommand.commandType = PRESSED;
    callbackCommand.value = convertMatrix[evt.bit.NUM];
    mainUnit->actOnControlsCallback(callbackCommand);
  }
    
  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING){
    callbackCommand.commandType = RELEASED;
    callbackCommand.value = convertMatrix[evt.bit.NUM];
    mainUnit->actOnControlsCallback(callbackCommand);
  }
  return 0;
}

void RTPNeoTrellis::begin(RTPMainUnit* _mainUnit){
  RTPNeoTrellis::mainUnit = _mainUnit;
  if(!myTrellis.begin()){
    Serial.println("could not start trellis");
    while(1);
  }
  else
    Serial.println("trellis started");

  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    myTrellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    myTrellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    myTrellis.registerCallback(i, RTPNeoTrellis::blink);
  }
}

void RTPNeoTrellis::introAnimation(){
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++)
    myTrellis.pixels.setPixelColor(i, random(0,255), random(0,255), random(0,255));
  myTrellis.pixels.show();
}

void RTPNeoTrellis::read(){
  if(!digitalRead(TRELLIS_INT_PIN))
    myTrellis.read(false);
  myTrellis.pixels.show();
}

void RTPNeoTrellis::writeSequenceStates(RTPSequenceNoteStates seqStates, int color, bool show){
  for(int i=0; i<SEQ_BLOCK_SIZE; i++){
      int pixelColor = seqStates.val[i] ? colorScaler(color, seqStates.velocity[i], 127)  :  0;
      myTrellis.pixels.setPixelColor(convertMatrix[i], pixelColor);
    }
  if(show)
    myTrellis.pixels.show();
}

void RTPNeoTrellis::writeSceneStates(RTPSequencesState sequencesState){
  for(int i=0; i<SCENE_BLOCK_SIZE; i++)
      myTrellis.pixels.setPixelColor(convertMatrix[i], sequencesState.sequenceState[i].state ? sequencesState.sequenceState[i].color : 0);
  myTrellis.pixels.show();
}

void RTPNeoTrellis::writeBuitCCStates(RTPSequencesState ccStates, int color){
  for(int i=0; i<N_BUITS_CC; i++)
      myTrellis.pixels.setPixelColor(convertMatrix[i], ccStates.sequenceState[i].state ? color : 0);
  myTrellis.pixels.show();
}

void RTPNeoTrellis::writeSequenceSettingsPage(SequenceSettings sequenceSettings){
  for(int i=0; i<SCENE_BLOCK_SIZE; i++)
    myTrellis.pixels.setPixelColor(i, 0);

  // Pad 0 — Type: show the canonical type colour
  const uint32_t typeColors[] = {
    DRUM_COLOR, BASS_SYNTH_COLOR, MONO_SYNTH_COLOR,
    POLY_SYNTH_COLOR, CONTROL_TRACK_COLOR, HARMONY_TRACK_COLOR
  };
  uint8_t t = sequenceSettings.type;
  uint32_t typeCol = (t < 6) ? typeColors[t] : 0xFFFFFF;
  myTrellis.pixels.setPixelColor(convertMatrix[0], typeCol);

  // Pad 1 — MIDI Channel: spread hue across 16 channels (index 0-30, step 2)
  uint8_t ch = sequenceSettings.midiChannel;
  if (ch < 1) ch = 1;
  if (ch > 16) ch = 16;
  uint32_t chCol = colorMapper((ch - 1) * 2);
  myTrellis.pixels.setPixelColor(convertMatrix[1], chCol);

  // Pad 2 — Color: show the chosen colour from the wheel
  myTrellis.pixels.setPixelColor(convertMatrix[2], colorMapper(sequenceSettings.color));

  // Pad 3 — Length: white scaled by number of pages (1-16 -> dim to bright)
  uint8_t pages = sequenceSettings.lenght;
  if (pages < 1) pages = 1;
  if (pages > 16) pages = 16;
  uint8_t brightness = (uint8_t)(pages * 15 + 15);  // 30..255
  myTrellis.pixels.setPixelColor(convertMatrix[3], myTrellis.pixels.Color(brightness, brightness, brightness));

  // Pad 4 — Input: distinct color per input source
  {
    const uint32_t inputColors[] = {
      0x00FFFF,  // 0: Any (cyan)
      0x0000FF,  // 1: USB Device (blue)
      0x00FF00,  // 2: USB Host ALL (green)
      0xFFFF00,  // 3: DIN (yellow)
      0xFFFFFF,  // 4: ALL (white)
      0x00FF40,  // 5: USB Host 1
      0x00CC00,  // 6: USB Host 2
      0x008800,  // 7: USB Host 3
      0x004400   // 8: USB Host 4
    };
    uint8_t inp = sequenceSettings.input;
    uint32_t iCol = (inp <= 8) ? inputColors[inp] : 0x000000;
    myTrellis.pixels.setPixelColor(convertMatrix[4], iCol);
  }

  // Pad 5 — Output: distinct color per output destination
  {
    const uint32_t portColors[] = {
      0xFF0000,  // 0: Default (red / routing table)
      0x0000FF,  // 1: USB Device (blue)
      0x00FF00,  // 2: USB Host ALL (green)
      0xFFFF00,  // 3: DIN (yellow)
      0xFFFFFF,  // 4: ALL (white)
      0x00FF40,  // 5: USB Host 1 (green-cyan)
      0x00CC00,  // 6: USB Host 2 (darker green)
      0x008800,  // 7: USB Host 3 (dim green)
      0x004400   // 8: USB Host 4 (very dim green)
    };
    uint8_t p = sequenceSettings.port;
    uint32_t pCol = (p <= 8) ? portColors[p] : 0x000000;
    myTrellis.pixels.setPixelColor(convertMatrix[5], pCol);
  }

  myTrellis.pixels.show();
}

void RTPNeoTrellis::writeTransportPage(int color){
  for(int i=0; i<SCENE_BLOCK_SIZE; i++){
    myTrellis.pixels.setPixelColor(i, color);
  }
  myTrellis.pixels.show();
}

void RTPNeoTrellis::moveCursor(int cursorPos){
  myTrellis.pixels.setPixelColor(convertMatrix[cursorPos], CURSOR_COLOR);
  myTrellis.pixels.show();
}

// Individual button control for transport state
void RTPNeoTrellis::setButtonColor(int buttonIndex, uint32_t color){
  if(buttonIndex >= 0 && buttonIndex < NEO_TRELLIS_NUM_KEYS){
    myTrellis.pixels.setPixelColor(convertMatrix[buttonIndex], color);
  }
}

void RTPNeoTrellis::clearButton(int buttonIndex){
  if(buttonIndex >= 0 && buttonIndex < NEO_TRELLIS_NUM_KEYS){
    myTrellis.pixels.setPixelColor(convertMatrix[buttonIndex], 0);
  }
}

void RTPNeoTrellis::clearAllButtons(){
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    myTrellis.pixels.setPixelColor(i, 0);
  }
}

void RTPNeoTrellis::show(){
  myTrellis.pixels.show();
}

// Color helpers - using NeoPixel color format (GRB)
uint32_t RTPNeoTrellis::colorGreen(){
  return myTrellis.pixels.Color(0, 255, 0);  // GRB format
}

uint32_t RTPNeoTrellis::colorRed(){
  return myTrellis.pixels.Color(255, 0, 0);  // GRB format
}

uint32_t RTPNeoTrellis::colorYellow(){
  return myTrellis.pixels.Color(255, 255, 0);  // GRB format
}

uint32_t RTPNeoTrellis::colorBlue(){
  return myTrellis.pixels.Color(0, 0, 255);  // GRB format
}

uint32_t RTPNeoTrellis::colorWhite(){
  return myTrellis.pixels.Color(255, 255, 255);  // GRB format
}

uint32_t RTPNeoTrellis::colorOff(){
  return 0;
}

uint32_t RTPNeoTrellis::colorDim(uint32_t color, uint8_t brightness){
  // brightness is 0-255
  uint8_t r = (uint8_t)(color >> 16) & 0xFF;
  uint8_t g = (uint8_t)(color >> 8) & 0xFF;
  uint8_t b = (uint8_t)color & 0xFF;
  
  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;
  
  return myTrellis.pixels.Color(r, g, b);
}

// Full spectrum hue sweep: 0=red, 4=green, 8=cyan, 12=blue, 13=purple, 15=magenta
// HSV S=255, V=180, GRB NeoPixel format
uint32_t RTPNeoTrellis::colorForPage(uint8_t page){
  // Full hue sweep: page 0-15 maps evenly across 0-255 (red→yellow→green→cyan→blue→purple→magenta)
  uint8_t hue = (uint8_t)((page * 256) / 16);  // 0, 16, 32 ... 240
  
  // HSV to RGB (S=255, V=180)
  uint8_t region = hue / 43;
  uint8_t remainder = (hue - (region * 43)) * 6;
  uint8_t q = (180 * (255 - remainder)) >> 8;
  uint8_t t = (180 * remainder) >> 8;
  
  uint8_t r, g, b;
  switch (region) {
    case 0:  r = 180; g = t;   b = 0;   break;
    case 1:  r = q;   g = 180; b = 0;   break;
    case 2:  r = 0;   g = 180; b = t;   break;
    case 3:  r = 0;   g = q;   b = 180; break;
    case 4:  r = t;   g = 0;   b = 180; break;
    default: r = 180; g = 0;   b = q;   break;
  }
  return myTrellis.pixels.Color(r, g, b);
}

// Slot color: same hue as its page, bright if file exists, very dim if empty
uint32_t RTPNeoTrellis::colorForSlot(uint8_t page, bool exists){
  uint32_t base = colorForPage(page);
  return exists ? base : colorDim(base, 30);
}