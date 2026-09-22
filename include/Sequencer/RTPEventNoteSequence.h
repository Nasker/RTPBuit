#pragma once

#include <cstdint>
#include "Interfaces/IMidiOutput.hpp"
#include "Midi/MidiPort.hpp"
#include "RTPEventNotePlus.h"
#include "RTPParameter.hpp"
#include "NotesPlayer.hpp"
#include "MusicManager.hpp"
#include "Arduino.h"
#include <vector>
#include <queue>
#include "Structs.h"
#include "ControlCommand.h"
#include "constants.h"
#include "ArduinoJson.h"

using namespace std;

class MidiRouter;

#define SEQ_BLOCK_SIZE 16
#define SCENE_BLOCK_SIZE 16

enum SequenceParametersIndex{
	TYPE,
	MIDI_CHANNEL,
	COLOR,
	LENGTH,
	INPUT_PORT,
	PORT,
	CLOCK_DIVIDER
};

class RTPEventNoteSequence{
protected:
	IMidiOutput* _midiOutput = nullptr;
	vector<RTPEventNotePlus> EventNoteSequence;
	vector<RTPParameter> sequenceParameters;
	NotesPlayer& _notesPlayer;
	MusicManager& _musicManager;
	vector<RTPEventNotePlus>::iterator it;
	uint8_t _baseNote;
	bool _isEnabled;
	bool _isRecording;
	size_t _currentPosition;
	uint8_t _selectedParameter;
        // Staged settings edit: rotary adjusts this pending value (shown in the
        // settings display); it only lands on the real parameter on commit.
        // -1 = no pending edit.
        int _pendingParamValue = -1;
	uint8_t _selectedPage;
	uint8_t _pulseCounter;
	String _name;
	char _usbHostLabel[48];
	char _inputUsbHostLabel[48];
	static class UsbHostManager* _usbHostManager;
	static const uint8_t CLOCK_DIVIDER_PULSES[11];
public:
	RTPEventNoteSequence(uint8_t midiChannel, uint16_t NEvents, uint8_t type, uint8_t baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
	void clearSequence();
	void addEventNote(RTPEventNotePlus eventNote);
	virtual void fordwardSequence();
	void backwardSequence();
	void resetSequence();
	uint16_t getCurrentSequencePosition();
	bool isCurrentSequenceEnabled() const;
	// True while the playhead sits on a step that holds a note. Used by the
	// scene grid to flash the pad of a sequence that is sounding right now.
	// Stateless: the playhead rests on the fired step for its whole duration.
	bool isPlayheadOnActiveStep();
	bool isRecording();
	void toggleRecording();
	void enableSequence(bool state);
	virtual void setTypeSpecificColor() = 0;
	virtual void playCurrentEventNote() = 0;
	virtual void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {}
	virtual void playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {}
	virtual void handleLiveThreeAxis(ControlCommand command) {}
	virtual void handleLiveSequencerTick() {}
	virtual void handleLiveHalfTick() {}  // For 32nd note resolution
	virtual uint8_t getLiveVelocity() const { return 90; }
	void setMidiOutput(IMidiOutput* midiOutput) { _midiOutput = midiOutput; }
	void setMidiChannel(uint8_t midiChannel);
	uint8_t getMidiChannel();
	uint8_t getMidiChannel() const;  // Const version for JSON serialization
	void setColor(uint32_t color);
	uint32_t getColor();
	void setType(uint8_t type);
	uint8_t getType();
	uint8_t getType() const;  // Const version for JSON serialization
	size_t getSequenceSize();
	void editNoteInSequence(size_t position, bool eventState);
	bool getNoteStateInSequence(size_t position);
	uint8_t getNoteVelocityInSequence(size_t position);
	void editNoteInSequence(size_t position, uint8_t note, uint8_t velocity);
	void editNoteInSequence(size_t position, uint8_t note, uint8_t velocity, uint8_t length, bool literalPitch);
	// Absolute-position write (no page offset) used by the recorder dump.
	void writeRecordedNote(size_t absPosition, uint8_t note, uint8_t velocity,
	                       uint8_t length, bool literalPitch, uint8_t microOffset);
	virtual void editNoteInCurrentPosition(ControlCommand command) = 0;
	void resizeSequence(size_t newSize);
	void selectParameter(uint8_t parameterIndex);
	void increaseParameterValue();
	void decreaseParameterValue();
	// Commit/discard the staged parameter edit (see _pendingParamValue).
	void commitParameterEdit();
	void discardParameterEdit();
	bool hasPendingParameterEdit() const { return _pendingParamValue >= 0; }
	virtual void increasePage();
	virtual void decreasePage();
	uint8_t getParameterValue();
	String getParameterName();
	uint8_t getSelectedParameterIndex() { return _selectedParameter; }
	uint8_t getPort();
	uint8_t getPort() const;
	void setPort(uint8_t port);
	MidiPort getPortAsMidiPort();
	uint8_t getUsbHostDeviceIndex();
	uint8_t getInput();
	uint8_t getInput() const;
	void setInput(uint8_t input);
	uint8_t getLength() const;
	void setLength(uint8_t length);
	uint8_t getClockDivider() const;
	uint8_t getClockDivider();
	void setClockDivider(uint8_t index);
	uint8_t getClockDividerPulses() const;
	bool isStepPulse() const;
	uint8_t getPulseCounter() const { return _pulseCounter; }
	// True on the exact 24-PPQN pulse the current step's note should fire:
	// its stored microOffset (0 = on the grid). Notes whose offset exceeds
	// the current divider still fire on the last pulse of the step.
	bool isNotePulse() const;
	String getName() const { return _name; }
	void setName(const String& name) { _name = name; }
	// isSelected matters only for INPUT_PORT == 0 ("follow selection"); an
	// explicitly chosen source (1-8) listens regardless of selection.
	bool acceptsInput(uint8_t srcPort, uint8_t srcDevice, bool isSelected);
	const char* getUsbHostLabel() const { return _usbHostLabel; }
	void setUsbHostLabel(const char* label);
	const char* getInputUsbHostLabel() const { return _inputUsbHostLabel; }
	void setInputUsbHostLabel(const char* label);
	// Live-input thru: plays the incoming event out this lane's own port,
	// literally (no harmony/chord remap). Not gated on lane enable.
	void thruNoteOn(uint8_t note, uint8_t velocity)  { routeLiveNoteOn(note, velocity, getMidiChannel()); }
	void thruNoteOff(uint8_t note)                   { routeLiveNoteOff(note, getMidiChannel()); }
	void thruCC(uint8_t controller, uint8_t value)   { routeLiveCC(controller, value, getMidiChannel()); }
	static void setUsbHostManager(UsbHostManager* m) { _usbHostManager = m; }
	static void setRouter(MidiRouter* router);
	vector<RTPEventNotePlus>& getEventNoteSequence();
	const vector<RTPEventNotePlus>& getEventNoteSequence() const;  // Const version for JSON serialization
	String dumpSequenceToJson();
	uint8_t page();
	uint16_t pageOffset();
protected:
	// Snapshot the label of the device currently at the selected USB Host slot
	// (PORT 5-8); clears the label for any other port or when the slot is empty.
	void _syncUsbHostLabelToPort();
	// Same for the input binding (INPUT_PORT 5-8), kept separate so a lane can
	// listen to one host device while outputting to another.
	void _syncUsbHostLabelToInput();
        // Tile existing content into newly exposed pages when LENGTH grows:
        // new position pos copies pos % oldSize, so +1 page copies bar 1 and
        // doubling 2->4 gives bar3=bar1, bar4=bar2.
        void _tilePatternOnGrow(uint8_t oldPages, uint8_t newPages);
	void pointIterator(uint16_t position);
	void routeLiveNoteOn(uint8_t note, uint8_t velocity, uint8_t channel);
	void routeLiveNoteOff(uint8_t note, uint8_t channel);
	void routeLiveCC(uint8_t controller, uint8_t value, uint8_t channel);
private:
	static MidiRouter* _router;
};
