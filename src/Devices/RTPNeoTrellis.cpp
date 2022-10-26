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

void RTPNeoTrellis::read(){
  if(!digitalRead(TRELLIS_INT_PIN))
    myTrellis.read(false);
}

void RTPNeoTrellis::writeSequenceStates(RTPSequenceNoteStates seqStates, int color, bool show=true){
  for(int i=0; i<SEQ_BLOCK_SIZE; i++)
      myTrellis.pixels.setPixelColor(convertMatrix[i], seqStates.val[i] ? color : 0);
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