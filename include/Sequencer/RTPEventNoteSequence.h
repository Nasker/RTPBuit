#pragma once

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
	int _baseNote;
	bool _isEnabled;
	bool _isRecording;
	size_t _currentPosition;
	int _selectedParameter;
	int _pages;
	int _selectedPage;
public:
	RTPEventNoteSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
	void clearSequence();
	void addEventNote(RTPEventNotePlus eventNote);
	virtual void fordwardSequence();
	void backwardSequence();
	void resetSequence();
	int getCurrentSequencePosition();
	bool isCurrentSequenceEnabled();
	bool isRecording();
	void toggleRecording();
	void enableSequence(bool state);
	virtual void setTypeSpecificColor() = 0;
	virtual void playCurrentEventNote() = 0;
	void setMidiChannel(int midiChannel);
	int getMidiChannel();
	void setColor(int color);
	int getColor();
	void setType(int type);
	int getType();
	size_t getSequenceSize();
	void editNoteInSequence(size_t position, bool eventState);
	bool getNoteStateInSequence(size_t position);
	int getNoteVelocityInSequence(size_t position);
	void editNoteInSequence(size_t position, int note, int velocity);
	virtual void editNoteInCurrentPosition(ControlCommand command) = 0;
	void resizeSequence(size_t newSize);
	void selectParameter(int parameterIndex);
	void increaseParameterValue();
	void decreaseParameterValue();
	virtual void increasePage();
	virtual void decreasePage();
	int getParameterValue();
	String getParameterName();
	list<RTPEventNotePlus> getEventNoteSequence();
	String dumpSequenceToJson();
	int page();
	int pageOffset();
protected:
	void pointIterator(int position);
};
