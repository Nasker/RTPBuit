#include "RTPNeoTrellis.hpp"
#include "RTPMainUnit.hpp"


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
    callbackCommand.value = evt.bit.NUM;
    mainUnit->actOnControlsCallback(callbackCommand);
  }
  myTrellis.pixels.show();
  
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
}

void RTPNeoTrellis::writeSequenceStates(RTPSequenceNoteStates seqStates, int color, bool show=true){
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
  Serial.printf("CH: %d, color: %d, type: %d, length: %d\n", 
  sequenceSettings.midiChannel, sequenceSettings.color, sequenceSettings.type, sequenceSettings.lenght);
  for(int i=0; i<SCENE_BLOCK_SIZE; i++)
    myTrellis.pixels.setPixelColor(i, 0);
  myTrellis.pixels.setPixelColor(convertMatrix[0], sequenceSettings.type);
  myTrellis.pixels.setPixelColor(convertMatrix[1], sequenceSettings.midiChannel);
  myTrellis.pixels.setPixelColor(convertMatrix[2], colorMapper(sequenceSettings.color));
  myTrellis.pixels.setPixelColor(convertMatrix[3], sequenceSettings.lenght);
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