#include "SequenceSettingsPresenter.hpp"
#include "Sequencer/RTPEventNoteSequence.h"
#include "Midi/UsbHostManager.hpp"
#include <cstring>

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
        RTPEventNoteSequence* seq = _concreteSequencer.getActiveSequence();
        valueStr = resolvePortDisplayName(paramValue, portNames,
                                          seq ? seq->getUsbHostLabel() : "");
    } else if (paramName == "Input") {
        const char* inputNames[] = {"Selected", "USB", "USB Host", "DIN", "ALL",
                                     "Host 1", "Host 2", "Host 3", "Host 4"};
        RTPEventNoteSequence* seq = _concreteSequencer.getActiveSequence();
        valueStr = resolvePortDisplayName(paramValue, inputNames,
                                          seq ? seq->getInputUsbHostLabel() : "");
    } else if (paramName == "Div") {
        // Grid step rate: index -> musical note division (24 PPQN)
        const char* divNames[] = {"1/1", "1/2", "1/4", "1/4T", "1/8", "1/8T",
                                   "1/16", "1/16T", "1/32", "1/32T", "1/64"};
        valueStr = (paramValue >= 0 && paramValue <= 10)
            ? String(divNames[paramValue]) : String(paramValue);
    } else {
        valueStr = String(paramValue);
    }

    // Trailing * = staged but not yet committed (press the encoder to apply).
    if (_sequencer.hasPendingParameterEdit()) {
        valueStr += "*";
        // Overlay the draft onto the trellis page so rotating previews the
        // parameter's visual (colour, length brightness, channel hue, ...).
        switch (_sequencer.getParameterIndex()) {
            case TYPE:          s.type = paramValue; break;
            case MIDI_CHANNEL:  s.midiChannel = paramValue; break;
            case COLOR:         s.color = paramValue; break;
            case LENGTH:        s.lenght = paramValue; break;
            case INPUT_PORT:    s.input = paramValue; break;
            case PORT:          s.port = paramValue; break;
            case CLOCK_DIVIDER: s.clockDivider = paramValue; break;
        }
    }

    _display.printThreeLines("Seq Settings", valueStr, "");
    static const PadHint hints[] = {
        {0,"TYP"},{1,"CH"},{2,"COL"},{3,"LEN"},{4,"IN"},{5,"OUT"},{6,"DIV"}
    };
    _display.setPadLegend(hints, 7, (int8_t)_sequencer.getParameterIndex());
    _trellis.writeSequenceSettingsPage(s);
}

String SequenceSettingsPresenter::resolvePortDisplayName(int paramValue, const char* names[], const char* hostLabel) {
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
        // The stored label may resolve to a different slot after re-enumeration.
        const char* label = hostLabel;
        int8_t resolved = (label[0]) ? _usbHostManager->findDeviceByLabel(label) : -1;
        if (resolved >= 0) {
            valueStr = _usbHostManager->getDeviceName((uint8_t)resolved);
        } else if (_usbHostManager->isDeviceConnected(idx)) {
            valueStr = _usbHostManager->getDeviceName(idx);
        } else if (label[0]) {
            // Device absent: show which device the patch expects.
            const char* product = strrchr(label, '|');
            valueStr = String(product ? product + 1 : label) + " (gone)";
        } else {
            valueStr = "Host " + String(idx + 1) + " (none)";
        }
    }
    return valueStr;
}

void SequenceSettingsPresenter::showSequence(){
    String sequenceName = _concreteSequencer.getSelectedSequenceDisplayName();
    int totalPages = _concreteSequencer.getSelectedSequenceSettings().lenght;

    // Line 1 = where we are, line 2 = the sequence name. The cell row below
    // shows the sequence's pages (filled = has notes, ringed = current page),
    // so no "Pg x/y" text is needed.
    _display.printThreeLines("Sequence Edit", sequenceName, "");

    uint16_t mask = 0;
    RTPEventNoteSequence* seq = _concreteSequencer.getActiveSequence();
    if (seq) {
        auto& notes = seq->getEventNoteSequence();
        for (size_t i = 0; i < notes.size(); i++)
            if (notes[i].eventState()) mask |= (uint16_t)(1 << (i / SEQ_BLOCK_SIZE));
    }
    _display.setGrid(mask, (uint8_t)totalPages, (int8_t)_sequencer.getCurrentPage());

    _trellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor());
}

void SequenceSettingsPresenter::presentScene(){
    String sceneName = _concreteSequencer.getCurrentSceneName();
    if (sceneName.length() == 0) sceneName = "Scene " + String(_sequencer.getCurrentScene() + 1);

    // Line 1 = where we are, line 2 = the scene name. The cell row below shows
    // the sibling scenes (filled = has enabled sequences, ringed = current).
    _display.printThreeLines("Scene Edit", sceneName, "");

    int nScenes = _sequencer.getNumScenes();
    uint16_t mask = 0;
    for (int i = 0; i < nScenes && i < 16; i++) {
        RTPScene* s = _concreteSequencer.getScene(i);
        if (!s) continue;
        RTPSequencesState ss = s->getSequencesState();
        for (uint8_t j = 0; j < 16; j++)
            if (ss.sequenceState[j].state) { mask |= (1 << i); break; }
    }
    _display.setGrid(mask, (uint8_t)nScenes, (int8_t)_sequencer.getCurrentScene());
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

    String sceneName = _concreteSequencer.getCurrentSceneName();
    _display.printThreeLines(
        "Scene Settings",
        sceneName.length() > 0 ? sceneName
                               : "Scene " + String(curScene) + "/" + String(nScenes),
        playing ? "Playing" : "Stopped"
    );
    static const PadHint hints[] = {
        {0,"LOAD"},{1,"SAVE"},{2,"ADD"},{3,"DEL"},{4,"MUTE"}
    };
    _display.setPadLegend(hints, 5, focusedPad);
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

void SequenceSettingsPresenter::presentTransport(int8_t focusedPad){
    // What the rotary adjusts for the focused pad (mirrors TransportState).
    static const char* rotaryTarget[] = {"Swing", "", "Quant", "BPM", "Volume"};
    String value = (focusedPad >= 0 && focusedPad <= 4) ? rotaryTarget[focusedPad] : "";

    _display.printThreeLines("Transport", value, "");
    static const PadHint hints[] = {
        {0,"PLAY"},{1,"STOP"},{2,"REC"},{3,"TAP"},{4,"MODE"}
    };
    _display.setPadLegend(hints, 5, focusedPad);
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
