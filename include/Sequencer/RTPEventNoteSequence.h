#pragma once

#include <cstdint>
#include "RTPEventNotePlus.h"
#include "RTPParameter.hpp"
#include "NotesPlayer.hpp"
#include "MusicManager.hpp"
#include "Arduino.h"
#include <vector>
#include <list>
#include "Structs.h"
#include "ControlCommand.h"
#include "constants.h"
#include "ArduinoJson.h"

using namespace std;

#define SEQ_BLOCK_SIZE 16
#define SCENE_BLOCK_SIZE 16

enum SequenceParametersIndex{
	TYPE,
	MIDI_CHANNEL,
	COLOR,
	LENGTH
};

class RTPEventNoteSequence{
protected:
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
	virtual void editNoteInCurrentPosition(ControlCommand command) = 0;
	void resizeSequence(size_t newSize);
	void selectParameter(uint8_t parameterIndex);
	void increaseParameterValue();
	void decreaseParameterValue();
	virtual void increasePage();
	virtual void decreasePage();
	uint8_t getParameterValue();
	String getParameterName();
	list<RTPEventNotePlus> getEventNoteSequence();
	const list<RTPEventNotePlus>& getEventNoteSequence() const;  // Const version for JSON serialization
	String dumpSequenceToJson();
	uint8_t page();
	uint16_t pageOffset();
protected:
	void pointIterator(uint16_t position);
};
