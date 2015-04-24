/***********
Zachary Whaley
gamemogul@gmail.com
--Sound Manager--
* Organizes the audio streams and manages their use
***********/

#ifndef SND_MAN_H
#define SND_MAN_H

#include <iostream>
#include <vector>
#include <string>

#include <al/al.h>
#include <al/alc.h>

#include "../ogg/stream/ogg_stream.h"

#include "../../core/stat_manager/stat_manager.h"
#include "sndFile_man.h"

enum SM_STATE { SM_PLAY, SM_PAUSED, SM_RESUME, SM_WAIT };
class sound_manager
{
private:
	ALCdevice					*device;			//!<What sound device am I using?
	ALCcontext					*context;			//!<sound context handle

	sndFile_man					*srcStrings;		//!<name/path key-value pairs for streams
	std::map<std::string, int>	audio_map;			//!<Access the sounds by their names instead of numbers
	std::vector<ogg_stream*>	oggs;				//!<Actual sound streams in memory

	SM_STATE					state;				//!<Current state of the sound engine
	int							numSets;			//!<Number of total sfx

private:
	void check(void);								//!<Is the openAL state ok?
	ogg_stream * getStream(const std::string & snd);

	void loadSrc(void);//!<Load the name/path key-value pairs for sound streams

	//Add sounds to the manager
	bool addOgg(std::string, SND_TYPE type = SFX, bool loop = false);//!<Add an ogg by filename [what type is it? does it loop?]
	void addOgg(ogg_stream*);//!<Add an ogg if it already exists, by it's address

public:
	//Overall volume
	float sndVolume;//!<volume for sfx
	float musVolume;//!<volume for music

	//Get a handle to the itterators to use data
	std::vector<ogg_stream*>*	getOggs(void);//!<Master vector which points to all ogg_stream
	std::map<std::string, int>*	getAudio(void);//!<Map which stores (via sndFile_man) the location in the ogg vector by name

	int getNumSets(void);
	std::string getSndSet(int set);//!<Get a sndSet by placement

	//Begin streaming the sounds
	void play(std::string);//!<Play a specific ogg
	void play(void);//!<Set the sound_manager state to 'sm_play'

	bool isPlaying(std::string);

	//Pause the stream
	void pause(std::string);//!<Pause a specific ogg
	void pause(void);//!<Set the sound_manager state to 'sm_paused'

	//Fade the source
	void fadeIn(std::string);//!<set a source to fade in with a subsequent call to play
	void fadeOut(std::string);//!<set a source to fade out with a subsequent call to play

	//Begin streaming the sounds
	void stop(std::string);//!<Stop everything -- clear all streams

	//Play the streams
	void render(void);//!<Cause the sound to be

	//Dump everything and clean out
	void close(void);

	sound_manager(void);
	sound_manager(std::string);
	virtual ~sound_manager(void);
};

#endif