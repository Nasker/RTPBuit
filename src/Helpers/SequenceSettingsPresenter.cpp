#include "SequenceSettingsPresenter.hpp"
#include "Sequencer/RTPEventNoteSequence.h"
#include "Midi/UsbHostManager.hpp"

SequenceSettingsPresenter::SequenceSettingsPresenter(IDisplay& display, IButtonMatrix& trellis,
    ISequencer& sequencer, RTPSequencer& concreteSequencer, RecordingManager& recordingManager)
    : _display(display), _trellis(trellis), _sequencer(sequencer), _concreteSequencer(concreteSequencer),
      _recordingManager(recordingManager) {}

void SequenceSettingsPresenter::presentSequenceSettings(){
    SequenceSettings s = _concreteSequencer.getSelectedSequenceSettings();
    String paramName  = _sequencer.getParameterName();
    int    paramValue = _sequencer.getParameterValue();

    String valueStr;
    if (paramName == "Type") {
        valueStr = _sequencer.getSequenceTypeName();
    } else if (paramName == "Midi CH") {
        valueStr = "CH " + String(paramValue);
    } else if (paramName == "Color") {
        valueStr = "Col " + String(paramValue);
    } else if (paramName == "Lenght") {
        valueStr = String(paramValue) + " pages";
    } else if (paramName == "Output") {
        const char* portNames[] = {"Default", "USB", "USB Host", "DIN", "ALL",
                                    "Host 1", "Host 2", "Host 3", "Host 4"};
        valueStr = resolvePortDisplayName(paramValue, portNames);
    } else if (paramName == "Input") {
        const char* inputNames[] = {"Any", "USB", "USB Host", "DIN", "ALL",
                                     "Host 1", "Host 2", "Host 3", "Host 4"};
        valueStr = resolvePortDisplayName(paramValue, inputNames);
    } else {
        valueStr = String(paramValue);
    }

    _display.printThreeLines("Seq Settings", paramName, valueStr);
    _trellis.writeSequenceSettingsPage(s);
}

String SequenceSettingsPresenter::resolvePortDisplayName(int paramValue, const char* names[]) {
    String valueStr = (paramValue >= 0 && paramValue <= 8) ? String(names[paramValue]) : String("?");
    if (!_usbHostManager) return valueStr;
    if (paramValue == 2) {
        uint8_t count = _usbHostManager->getDeviceCount();
        if (count == 1) {
            for (uint8_t i = 0; i < 4; i++) {
                if (_usbHostManager->isDeviceConnected(i)) {
                    valueStr = _usbHostManager->getDeviceName(i);
                    break;
                }
            }
        } else if (count > 1) {
            valueStr = String(count) + " USB devs";
        }
    }
    if (paramValue >= 5 && paramValue <= 8) {
        uint8_t idx = paramValue - 5;
        if (_usbHostManager->isDeviceConnected(idx)) {
            valueStr = _usbHostManager->getDeviceName(idx);
        } else {
            valueStr = "Host " + String(idx + 1) + " (none)";
        }
    }
    return valueStr;
}

void SequenceSettingsPresenter::showSequence(){
    String sequenceType = _sequencer.getSequenceTypeName();
    int midiChannel = _sequencer.getMidiChannel();
    int currentPage = _sequencer.getCurrentPage() + 1;
    int totalPages = _concreteSequencer.getSelectedSequenceSettings().lenght;

    SequenceDisplayState state = getSequenceDisplayState();

    static uint8_t blinkCounter = 0;
    blinkCounter++;
    bool blinkState = (blinkCounter / 8) % 2 == 0;

    _display.printFourLinesWithState(
        sequenceType,
        "Sequence " + String(_sequencer.getCurrentSequence() + 1),
        "Page " + String(currentPage) + " of " + String(totalPages),
        "Ch " + String(midiChannel),
        state,
        blinkState
    );
    _trellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor());
}

void SequenceSettingsPresenter::presentScene(){
    SequenceDisplayState state = _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;

    _display.printFourLinesWithState(
        "Scene",
        "Scene " + String(_sequencer.getCurrentScene() + 1),
        "",
        "",
        state,
        false
    );
    _trellis.writeSceneStates(_sequencer.getSequencesState());
}

void SequenceSettingsPresenter::presentSceneSettings(int8_t focusedPad){
    bool playing  = _sequencer.isPlaying();
    int  nScenes  = _sequencer.getNumScenes();
    int  curScene = _sequencer.getCurrentScene() + 1;

    _trellis.clearAllButtons();

    _trellis.setButtonColor(0, _trellis.getColorDim(_trellis.getColorBlue(), 200));
    _trellis.setButtonColor(1, _trellis.getColorDim(_trellis.getColorRed(), 180));
    _trellis.setButtonColor(2, playing
        ? _trellis.getColorDim(_trellis.getColorGreen(), 40)
        : _trellis.getColorGreen());

    bool canRemove = !playing && nScenes > 1;
    _trellis.setButtonColor(3, canRemove
        ? _trellis.getColorRed()
        : _trellis.getColorDim(_trellis.getColorRed(), 40));

    _trellis.setButtonColor(4, _trellis.getColorWhite());

    if (focusedPad >= 0 && focusedPad <= 4)
        _trellis.setButtonColor(focusedPad, _trellis.getColorWhite());

    _trellis.show();

    _display.printThreeLines(
        "Scene Settings",
        "Scene " + String(curScene) + "/" + String(nScenes),
        playing ? "Playing" : "Stopped"
    );
}

SequenceDisplayState SequenceSettingsPresenter::getSequenceDisplayState(){
    if (_recordingManager.isRecording()) {
        return SequenceDisplayState::Recording;
    } else if (_recordingManager.isWaiting()) {
        return SequenceDisplayState::Waiting;
    } else if (_sequencer.isRecording()) {
        return SequenceDisplayState::Waiting;
    } else {
        return _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;
    }
}

void SequenceSettingsPresenter::presentTransport(float bpm, SyncMode syncMode){
    String modeStr = (syncMode == SyncMode::Internal) ? "INT" : "EXT";
    String playState = _sequencer.isPlaying() ? "Playing" : "Stopped";
    String bpmStr = (syncMode == SyncMode::Internal) ? String(bpm, 0) + " BPM" : "";

    _display.printThreeLines(playState, bpmStr, modeStr);
    writeTransportPage();
}

void SequenceSettingsPresenter::writeTransportPage(){
    _trellis.clearAllButtons();

    if (_sequencer.isPlaying()) {
        _trellis.setButtonColor(0, _trellis.getColorGreen());
    } else {
        _trellis.setButtonColor(0, _trellis.getColorDim(_trellis.getColorGreen(), 128));
    }

    if (!_sequencer.isPlaying()) {
        _trellis.setButtonColor(1, _trellis.getColorBlue());
    } else {
        _trellis.setButtonColor(1, _trellis.getColorDim(_trellis.getColorBlue(), 128));
    }

    _trellis.setButtonColor(2, _trellis.getColorRed());
    _trellis.setButtonColor(3, _trellis.getColorYellow());
    _trellis.setButtonColor(4, _trellis.getColorWhite());

    _trellis.show();
}
