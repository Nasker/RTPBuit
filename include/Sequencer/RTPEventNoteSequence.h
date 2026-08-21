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
#include <list>
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
	PORT
};

class RTPEventNoteSequence{
protected:
	IMidiOutput* _midiOutput = nullptr;
	list<RTPEventNotePlus> EventNoteSequence;
	vector<RTPParameter> sequenceParameters;
	NotesPlayer& _notesPlayer;
	MusicManager& _musicManager;
	list<RTPEventNotePlus>::iterator it;
	uint8_t _baseNote;
	bool _isEnabled;
	bool _isRecording;
	size_t _currentPosition;
	uint8_t _selectedParameter;
	uint8_t _pages;
	uint8_t _selectedPage;
public:
	RTPEventNoteSequence(uint8_t midiChannel, uint16_t NEvents, uint8_t type, uint8_t baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
	void clearSequence();
	void addEventNote(RTPEventNotePlus eventNote);
	virtual void fordwardSequence();
	void backwardSequence();
	void resetSequence();
	uint16_t getCurrentSequencePosition();
	bool isCurrentSequenceEnabled();
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
	virtual void editNoteInCurrentPosition(ControlCommand command) = 0;
	void resizeSequence(size_t newSize);
	void selectParameter(uint8_t parameterIndex);
	void increaseParameterValue();
	void decreaseParameterValue();
	virtual void increasePage();
	virtual void decreasePage();
	uint8_t getParameterValue();
	String getParameterName();
	uint8_t getPort();
	uint8_t getPort() const;
	void setPort(uint8_t port);
	MidiPort getPortAsMidiPort();
	uint8_t getUsbHostDeviceIndex();
	static void setRouter(MidiRouter* router);
	list<RTPEventNotePlus> getEventNoteSequence();
	const list<RTPEventNotePlus>& getEventNoteSequence() const;  // Const version for JSON serialization
	String dumpSequenceToJson();
	uint8_t page();
	uint16_t pageOffset();
protected:
	void pointIterator(uint16_t position);
	void routeLiveNoteOn(uint8_t note, uint8_t velocity, uint8_t channel);
	void routeLiveNoteOff(uint8_t note, uint8_t channel);
	void routeLiveCC(uint8_t controller, uint8_t value, uint8_t channel);
private:
	static MidiRouter* _router;
};
