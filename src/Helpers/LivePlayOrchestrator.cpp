#include "LivePlayOrchestrator.hpp"
#include "Sequencer/RTPEventNoteSequence.h"
#include "constants.h"
#include "RTPTypeColors.h"

LivePlayOrchestrator::LivePlayOrchestrator(IDisplay& display, IButtonMatrix& trellis,
    ISequencer& sequencer, RTPSequencer& concreteSequencer,
    RecordingManager& recordingManager, LivePlayManager& livePlayManager)
    : _display(display), _trellis(trellis), _sequencer(sequencer),
      _concreteSequencer(concreteSequencer), _recordingManager(recordingManager),
      _livePlayManager(livePlayManager) {}

uint8_t LivePlayOrchestrator::getSelectedSequenceType(){
    return _sequencer.getSequenceType();
}

uint32_t LivePlayOrchestrator::getSelectedSequenceColor(){
    return _sequencer.getSequenceColor();
}

uint8_t LivePlayOrchestrator::getLiveVelocity(){
    return _sequencer.getLiveVelocity();
}

bool LivePlayOrchestrator::isSelectedSequenceRecording(){
    return _sequencer.isRecording();
}

void LivePlayOrchestrator::setTrellisButtonColor(uint8_t index, uint32_t color){
    _trellis.setButtonColor(index, color);
}

void LivePlayOrchestrator::showTrellis(){
    _trellis.show();
}

void LivePlayOrchestrator::printToScreen(String firstLine, String secondLine, String thirdLine){
    _display.printThreeLines(firstLine, secondLine, thirdLine);
}

void LivePlayOrchestrator::handleLiveTrellisPressed(uint8_t pad) {
    uint8_t seqType = getSelectedSequenceType();
    uint8_t liveVel = getLiveVelocity();

    if (seqType == DRUM_PART) {
        uint8_t note = BASE_NOTE + pad;
        _sequencer.playLiveNoteOn(note, liveVel, 0);
        if (isSelectedSequenceRecording())
            recorderNoteOn(note, liveVel);
        setTrellisButtonColor(pad, 0xFFFFFF);
        return;
    }

    if (pad < 12) {
        uint8_t rootNote = BASE_NOTE + pad;
        uint8_t chordType;

        if (seqType == POLY_SYNTH) {
            ChordAction action = _livePlayManager.beginChord(rootNote, pad);
            for (uint8_t i = 0; i < action.stopCount; i++)
                _sequencer.playLiveNoteOff(action.notesToStop[i], action.chordType);
            for (uint8_t i = 0; i < action.playCount; i++)
                _sequencer.playLiveNoteOn(action.notesToPlay[i], liveVel, action.chordType);
            chordType = action.chordType;
        } else {
            _livePlayManager.releaseAllChords();
            ChordAction action = _livePlayManager.beginChord(rootNote, pad);
            _sequencer.playLiveNoteOn(rootNote, liveVel, action.chordType);
            chordType = action.chordType;
        }

        if (isSelectedSequenceRecording()) {
            recorderNoteOn(rootNote, liveVel);
        }

        setTrellisButtonColor(pad, 0xFFFFFF);
        String chordStr = String(NOTE_NAMES[pad % 12]) + " " + String(CHORD_TYPE_NAMES[chordType & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), chordStr);
    } else {
        uint8_t modIdx = pad - 12;
        _livePlayManager.enableChordionKey(modIdx);
        setTrellisButtonColor(pad, 0xFFFFFF);
        String preview = String(CHORD_TYPE_NAMES[_livePlayManager.getChordType() & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), "[ " + preview + " ]");
    }
}

void LivePlayOrchestrator::handleLiveTrellisReleased(uint8_t pad) {
    uint8_t seqType = getSelectedSequenceType();

    if (seqType == DRUM_PART) {
        uint8_t note = BASE_NOTE + pad;
        _sequencer.playLiveNoteOff(note, 0);
        if (isSelectedSequenceRecording())
            recorderNoteOff(note);
        setTrellisButtonColor(pad, getSelectedSequenceColor());
        return;
    }

    if (pad < 12) {
        uint8_t rootNote = BASE_NOTE + pad;

        if (seqType == POLY_SYNTH) {
            ChordAction action = _livePlayManager.endChord(rootNote, pad);
            for (uint8_t i = 0; i < action.stopCount; i++)
                _sequencer.playLiveNoteOff(action.notesToStop[i], action.chordType);
        }

        if (isSelectedSequenceRecording()) {
            recorderNoteOff(rootNote);
        }

        // Repaint held/released pads for all tonal types. MONO/BASS latch the
        // note (no note-off here), but the pad must still return from white.
        syncLiveTrellis();
    }

    if (pad >= 12 && pad <= 15) {
        _livePlayManager.disableChordionKey(pad - 12);
        setTrellisButtonColor(pad, 0x101010);
        String preview = String(CHORD_TYPE_NAMES[_livePlayManager.getChordType() & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), "[ " + preview + " ]");
    }
}

void LivePlayOrchestrator::handleLiveSequencerTick() {
    _livePlayManager.advanceTick();
    _sequencer.handleLiveTick();
    if ((_livePlayManager.getTickCount() % 4) == 0)
        syncLiveTrellis();
}

void LivePlayOrchestrator::handleLiveFineTick() {
    _livePlayManager.advanceFineTick();
    uint8_t seqType = getSelectedSequenceType();

    if (seqType == DRUM_PART && _livePlayManager.shouldTriggerDrumRoll()) {
        _sequencer.playLiveNoteOn(_livePlayManager.getDrumRollNote(), getLiveVelocity(), 0);
    }

    _sequencer.handleLiveFineTick();
}

void LivePlayOrchestrator::syncLiveTrellis() {
    uint8_t seqType = getSelectedSequenceType();
    uint32_t seqColor = getSelectedSequenceColor();
    uint16_t heldPads = _livePlayManager.getHeldPadsMask();
    static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};
    uint32_t dimmed = (((seqColor >> 16 & 0xFF) * 35 / 255) << 16)
                    | (((seqColor >>  8 & 0xFF) * 35 / 255) <<  8)
                    |  ((seqColor       & 0xFF) * 35 / 255);
    for (uint8_t i = 0; i < 16; i++) {
        bool pressed = (heldPads >> i) & 1;
        if (pressed) {
            setTrellisButtonColor(i, 0xFFFFFF);
        } else if (seqType == DRUM_PART) {
            setTrellisButtonColor(i, seqColor);
        } else if (i < 12) {
            setTrellisButtonColor(i, isBlackKey[i] ? dimmed : seqColor);
        } else {
            setTrellisButtonColor(i, 0x101010);
        }
    }
}

void LivePlayOrchestrator::paintLiveTrellis() {
    _livePlayManager.releaseAllChords();
    uint8_t seqType = getSelectedSequenceType();
    uint32_t seqColor = getSelectedSequenceColor();

    static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};
    uint32_t dimmed = (((seqColor >> 16 & 0xFF) * 35 / 255) << 16)
                    | (((seqColor >>  8 & 0xFF) * 35 / 255) <<  8)
                    |  ((seqColor       & 0xFF) * 35 / 255);

    for (uint8_t i = 0; i < 16; i++) {
        if (seqType == DRUM_PART) {
            setTrellisButtonColor(i, seqColor);
        } else if (i < 12) {
            setTrellisButtonColor(i, isBlackKey[i] ? dimmed : seqColor);
        } else {
            setTrellisButtonColor(i, 0x101010);
        }
    }
    showTrellis();
}

void LivePlayOrchestrator::handleLiveDrumRollThreeAxis(ControlCommand command) {
    int val = command.value;
    switch (command.commandType) {
        case CHANGE_LEFT:
            if (val <= 2) {
                _livePlayManager.setDrumRollActive(false);
            } else {
                _livePlayManager.setDrumRollActive(true);
                uint8_t zone = (uint8_t)constrain((val * 16) / 127, 0, 15);
                _livePlayManager.setDrumRollNote(BASE_NOTE + zone);
            }
            break;
        case CHANGE_CENTER:
            if (val >= 125) {
                _livePlayManager.setDrumRollActive(false);
            } else {
                _livePlayManager.setDrumRollActive(true);
                _livePlayManager.setRollDivision(::map(constrain(val, 0, 120), 0, 120, 1, 16));
            }
            break;
        case CHANGE_RIGHT:
            _concreteSequencer.handleLiveThreeAxis(command);
            break;
    }
}

bool LivePlayOrchestrator::isSelectedSequenceWaiting(){
    return _recordingManager.isWaiting();
}

SequenceDisplayState LivePlayOrchestrator::getSequenceDisplayState(){
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

void LivePlayOrchestrator::toggleSelectedSequenceRecording(){
    _sequencer.toggleRecording();
    if (_sequencer.isRecording()) {
        uint16_t seqSize = _sequencer.getSequenceLength();
        uint8_t midiChannel = _sequencer.getMidiChannel();
        uint16_t currentPos = _sequencer.getCurrentPosition();
        _recordingManager.startRecording(seqSize, midiChannel, currentPos);
    } else {
        _recordingManager.stopRecording();
        recorderDumpToSequence();
    }
}

void LivePlayOrchestrator::recorderNoteOn(uint8_t note, uint8_t velocity) {
    if (_recordingManager.isRecording())
        _recordingManager.recordNoteOn(note, velocity);
}

void LivePlayOrchestrator::recorderNoteOff(uint8_t note) {
    if (_recordingManager.isRecording())
        _recordingManager.recordNoteOff(note);
}

void LivePlayOrchestrator::recorderAdvanceTick() {
    if (!_recordingManager.isRecording() && !_recordingManager.isWaiting()) return;

    _recordingManager.advanceTick();

    if (_recordingManager.isRecording() && _recordingManager.isEndOfSequence()) {
        _recordingManager.stopRecording();

        auto notes = _recordingManager.dumpRecordedSequence();
        if (!notes.empty()) {
            RTPScene* scene = _concreteSequencer.getScene(_concreteSequencer.getSelectScene());
            if (scene) {
                RTPEventNoteSequence* seq = scene->getSequence(_concreteSequencer.getSelectedSequence());
                if (seq) {
                    uint16_t seqSize = _recordingManager.getSequenceLength();
                    seq->clearSequence();
                    seq->resizeSequence(seqSize);
                    for (auto& note : notes) {
                        uint16_t pos = note.getEventRead();
                        if (pos < seqSize) {
                            seq->editNoteInSequence(pos, note.getEventNote(), note.getEventVelocity(),
                                                    note.getLength(), note.isLiteralPitch());
                            seq->editNoteInSequence(pos, true);
                        }
                    }
                }
            }
        }

        _sequencer.toggleRecording();
    }
}

void LivePlayOrchestrator::recorderDumpToSequence() {
    auto notes = _recordingManager.dumpRecordedSequence();
    if (notes.empty()) return;
    RTPScene* scene = _concreteSequencer.getScene(_concreteSequencer.getSelectScene());
    if (!scene) return;
    RTPEventNoteSequence* seq = scene->getSequence(_concreteSequencer.getSelectedSequence());
    if (!seq) return;
    uint16_t seqSize = _recordingManager.getSequenceLength();
    seq->clearSequence();
    seq->resizeSequence(seqSize);
    for (auto& note : notes) {
        uint16_t pos = note.getEventRead();
        if (pos < seqSize) {
            seq->editNoteInSequence(pos, note.getEventNote(), note.getEventVelocity(),
                                    note.getLength(), note.isLiteralPitch());
            seq->editNoteInSequence(pos, true);
        }
    }
}
