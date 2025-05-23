#pragma once

#include "RTPOled.hpp"
#include "RTPNeoTrellis.hpp"
#include "RTPSequencer.h"
#include "RTPSequencerManager.hpp"
#include "MatrixBuitControlChanger.hpp"
#include "Structs.h"
#include "RTPTypeColors.h"
#include "RTPSDManager.hpp"
#include "BuitPersistenceManager.hpp"


class BuitDevicesManager {
    RTPOled _oled;
    RTPNeoTrellis& _neoTrellis;
    RTPSequencer& _sequencer;
    BuitPersistenceManager _persistenceManager;
    MatrixBuitControlChanger _matrixBuitCC;

public:
    BuitDevicesManager(RTPNeoTrellis& neoTrellis, RTPSequencer& sequencer);
    void initSetup();
    void introAnimations();
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(ControlCommand command);

    void selectScene(ControlCommand command);
    void selectSequence(ControlCommand command);

    void presentScene();
    void presentSequence();
    void showSequence();
    void presentTransport();
    void presentBuitCC();
    void presentSequenceSettings();

    void editScene(ControlCommand command);
    void editSequence(ControlCommand command);
    void useTransport(ControlCommand command);
    void editCurrentNote(ControlCommand command);

    void displayCursorInSequence(ControlCommand command);

    void changeScene(ControlCommand command);
    void nudgePage(ControlCommand command);

    void selectParameter(ControlCommand command);
    void rotateParameter(ControlCommand command);

    void editCurrentNote(ControlCommand command);
    void editBuitCC(ControlCommand command);
    void sendBuitCC(ControlCommand command);

    int getSelectedSequenceMidichannel();
    bool isSelectedSequenceRecording();
    void toggleSelectedSequenceRecording();

    void saveSequencer(const String& fileName);
    void loadSequencer(const String& fileName);
private:
    void writeSequenceToNeoTrellis(RTPSequenceNoteStates sequenceStates, int color);
    void writeSceneToNeoTrellis(RTPSequencesState sequencesState);
    void writeTransportPage();
};