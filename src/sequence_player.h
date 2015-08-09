#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_SEQUENCE_PLAYER_H
#define MODERNMIDI_SEQUENCE_PLAYER_H

#include "modernmidi.h"
#include "concurrent_queue.h"
#include "midi_message.h"
#include "midi_file_reader.h"

#include <functional>
#include <thread>
#include <chrono>
#include <atomic>

#include "midi_output.h"

namespace mm 
{

class MidiSequencePlayer 
{
	MidiOutput & output;
public:

	MidiSequencePlayer(MidiOutput & output);
	~MidiSequencePlayer();
		
	void loadSequence(const std::vector<MidiTrack> & tracks);
	void start();
	void stop();

	void setLooping(bool newState)
	{
		loop = newState;
	}

	float length() const; // length of the song in seconds
		
	double ticksToSeconds(int ticks)
	{
		double beats = (double) ticks / ticksPerBeat;
		double seconds = beats / (beatsPerMinute / 60.0);
		return seconds;
	}
		
	int secondsToTicks(float seconds)
	{
		double ticksPerSecond = (beatsPerMinute * ticksPerBeat) / 60.0;
		double ticks = ticksPerSecond * seconds;
		return (int) ticks;
	}

	std::function<void ()> startedEvent;
	std::function<void ()> stoppedEvent;

	std::function<void(const MidiPlayerEvent ev)> eventCallback;

	ConcurrentQueue<MidiPlayerEvent> eventQueue;

	std::vector<MidiPlayerEvent> eventList; // indexed by track

private:

	void run();

	void addTimestampedEvent(int track, double now, std::shared_ptr<TrackEvent> ev);

	float beatsPerMinute;
	double ticksPerBeat;
	double msPerTick;
		
	float playTimeSeconds = 0;
	float startTime = 0;
	int eventCursor = 0;

	std::thread sequencerThread;
	std::atomic<bool> shouldSequence;
	bool loop = false;
};

} // mm

#endif
