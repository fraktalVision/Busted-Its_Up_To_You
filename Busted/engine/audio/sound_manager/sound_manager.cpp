/***********
Zachary Whaley
gamemogul@gmail.com
--Sound Manager--
* Organizes the audio streams and manages their use
***********/

#include <al/al.h>
#include <al/alc.h>
#include <assert.h>

#include "sound_manager.h"
#include "../ogg/stream/ogg_stream.h"

#include "../../tools/datadrive/datadrive.h"

extern class DataDrive* DATA_MAN;
#define DATA DATA_MAN->GetData

//---------------------------
//Singleton (externed elsewhere):
sound_manager *pMusic_man;

//---------------------------
//Constructors/Destructors
//---------------------------
//Instantiate a sound manager with no default sounds -- you must add streams to play anything
sound_manager::sound_manager(void) 
	:	srcStrings(NULL),
		device(NULL),
		context(NULL),
		numSets(0),
		sndVolume(0.5f),
		musVolume(0.5f),
		state(SM_PLAY)
{
	//Pick a device
	this->device = alcOpenDevice((ALchar*)"DirectSound");	// select the "preferred device"
	assert(device != NULL);									//It better exist
	if (device)
	{
		context = alcCreateContext(device,NULL);			//Make a context for all the sounds to play in
		alcMakeContextCurrent(context);						//Make it the current context
	}

	check();												//Check for errors
}

//---------------------------
//Instantiate a sound manager based on a text file with paths to sounds
//NOTE:(relative to the execuatble -- not the text file)
sound_manager::sound_manager(std::string file)
{
	this->sound_manager::sound_manager();				//Do everything for a no default-sound sound manager
	srcStrings = new sndFile_man(file);					//Make the sound path file read-only -- the file won't truncate on crash
	loadSrc();											//Load up the streams from the source file
}

//---------------------------
//Kill all
sound_manager::~sound_manager(void)
{
	this->close();
}

//---------------------------
//Get a handle of the vector of ogg-streams
std::vector<ogg_stream*>* sound_manager::getOggs(void)
{
	return &this->oggs;
}


//---------------------------
//Get a handle to the audio map which has 'keys' of the sound names, and 'values' of their position in the ogg vector
std::map<std::string, int>* sound_manager::getAudio(void)
{
	return &this->audio_map;
}

//---------------------------
//Load up the sources from the provided text file
void sound_manager::loadSrc(void)
{
	//If there is a stat manager for the source strings
	if(srcStrings)
	{
		srcStrings->open();		//Open the file with the paths to sounds
		//if(DATA("debug")) { std::cout << "BOBBY: " << srcStrings->read("BOBBY") << std::endl; } //test for mult files
		for(std::map<std::string, std::string>::iterator si = srcStrings->getVars()->begin(); si != srcStrings->getVars()->end(); ++si)
		{
			//The first value is the sound name, the second value is its path
			if(this->addOgg( (*si).second ))			//Load the ogg
				audio_map[(*si).first] = int(this->oggs.size())-1;	//Associate it's vector position with the name

			//Display the lists of sounds and their position ids
			//if(DATA("debug")) { std::cout << (*si).first << " = " << this->audio_map[(*si).first] << std::endl; }
		}

		//Hack for now -- Sound Sets: SNDSETS -- follow design patterns, silly!
		for(std::map<std::string, sndSet>::iterator si = ((sndFile_man*)(srcStrings))->getSndSets()->begin(); si != ((sndFile_man*)(srcStrings))->getSndSets()->end(); ++si)
		{
			//The first value is the sound name, the second value is its path
			if(this->addOgg( std::string((*si).second.happy), MUSIC, true ))				//Load the ogg
				audio_map[(*si).first + "-happy"] = int(this->oggs.size())-1;	//Associate it's vector position with the name

			if(this->addOgg( std::string((*si).second.sad), MUSIC, true ))					//Load the ogg
				audio_map[(*si).first + "-sad"] = int(this->oggs.size())-1;		//Associate it's vector position with the name

			if(this->addOgg( std::string((*si).second.puzzled), MUSIC, true ))				//Load the ogg
				audio_map[(*si).first + "-puzzled"] = int(this->oggs.size())-1;	//Associate it's vector position with the name
		}	
		srcStrings->close();	//Close the source file		

		numSets = ((sndFile_man*)(srcStrings))->getNumSets();
	}
}


//---------------------------
//Add sounds to the manager
//---------------------------
//Add an ogg the sound manager by path
bool sound_manager::addOgg(std::string filename, SND_TYPE type, bool loop)
{/*

 if(filename == "./assets/music/SNDSETS/set2/Fallin_Loop_Reggae_short.ogg")
 int bob = 5;*/

	//Open and close the file to do a test...
	if (std::ifstream(filename.c_str(), std::ifstream::in)) {
		ogg_stream *ogg = new ogg_stream(filename, type, loop);
		this->addOgg(ogg);
		return true;
	}
	else
		return false;
}

//---------------------------
//Add an ogg to the sound manager by pointer to an existing stream
void sound_manager::addOgg(ogg_stream *ogg)
{
	//Add the ogg to the vector
	try
	{
		if (ogg)
			this->oggs.push_back(ogg);
		else
			throw std::runtime_error("Cannot add ogg to sound_manager, ogg is a NULL pointer");
	} catch (const std::exception & e) {
		if(DATA("debug")) { std::cout << e.what() << std::endl; }

		/*for(std::vector<ogg_stream*>::iterator oi = oggs.begin(); oi != oggs.end(); ++oi)
		if(DATA("debug")) { std::cout << "Addr of ogg src: " << (*oi)->source << std::endl; }*/
	}
}

ogg_stream * sound_manager::getStream(const std::string & snd) 
{
	if (snd.empty()) {
		return NULL;
	}

	//Make sure the sound actually exists
	//For out of bounds checking
	std::map<std::string,int>::iterator it = getAudio()->find(snd);
	if (it == getAudio()->end()) {
		if(DATA("debug")) { std::cout << "No such sound." << std::endl; }
		return NULL;
	}

	int index = it->second;
	if (index >= 0 && index < oggs.size()) {
		return oggs[index];
	} else {
		return NULL;
	}
}

//---------------------------
//Begin streaming the sounds
//---------------------------
//Play a sound by it's name
void sound_manager::play(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (!ogg) {
		return;
	}
	check();
	try {
		//Play
		ogg->play();
		//Show info
		ogg->display();
	} catch (const std::exception & e) {
		if(DATA("debug")) { std::cout << e.what() << std::endl; }
	}
}

//---------------------------
//Pause all sound production
void sound_manager::play()
{
	this->state = SM_RESUME;
}

//---------------------------
//
bool sound_manager::isPlaying(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (ogg) {
		return (ogg->getOggState() != OGG_CLOSED);
	}
	return false;
}

//---------------------------
//Pause the stream
//---------------------------
//Interrupt the stream of an ogg by it's name
void sound_manager::pause(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (ogg) {
		ogg->pause();
	}
}

//---------------------------
//Pause all sound production
void sound_manager::pause(void)
{
	this->state = SM_PAUSED;
}

//---------------------------
//Stop the stream
//---------------------------
//Stop the stream of an ogg by it's name
void sound_manager::stop(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (ogg) {
		ogg->stop();
	}
}

//---------------------------
//Fade the stream
//---------------------------
//
void sound_manager::fadeIn(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (ogg) {
		ogg->gain = 0.0f;
		ogg->fade = 0.1f;
	}
}

//---------------------------
//
void sound_manager::fadeOut(std::string snd)
{
	ogg_stream * ogg = getStream(snd);
	if (ogg) {
		ogg->fade = -0.1f;
	}
}

//---------------------------
//Generate the audio from the streams
void sound_manager::render(void)
{
	switch(state) {
	case SM_PLAY:
		for (size_t i = 0; i < oggs.size(); ++i) {
			ogg_stream * ogg = oggs[i];
			try {	  
				//float volume = ((*ogg)->type == SFX)?sndVolume:musVolume;
				float volume = 0.5f;
				if(ogg->type == SFX) {
					volume = this->sndVolume;
				} else {
					volume = this->musVolume;
				}
				ogg->update(volume);
			} catch (const std::exception & e) {
				if(DATA("debug")) { std::cout << e.what() << std::endl; }
			}
		}
		break;

	case SM_PAUSED:
		for (size_t i = 0; i < oggs.size(); ++i) {
			ogg_stream * ogg = oggs[i];
			try {
				ogg->pause();
			} catch (const std::exception & e) {
				if(DATA("debug")) { std::cout << e.what() << std::endl; }
			}
		}
		state = SM_PLAY;
		break;

	case SM_RESUME:
		for (size_t i = 0; i < oggs.size(); ++i) {
			ogg_stream * ogg = oggs[i];
			try {
				if (ogg->getOggState() == OGG_PAUSED) {
					ogg->play();
				}
			} catch (const std::exception & e) {
				if(DATA("debug")) { std::cout << e.what() << std::endl; }
			}
		}
		state = SM_PLAY;
		break;

	default:
		break;
	}
}

//---------------------------
//Clear everything out
void sound_manager::close(void)
{
	//Release all the streams
	for (size_t i = 0; i < oggs.size(); ++i) {
		delete oggs[i];
	}

	//Dump the vector
	this->oggs.clear();

	alcMakeContextCurrent(NULL);	//Make no context current
	alcDestroyContext(context);		//Unset old context
	alcCloseDevice(device);			//Shutdown the audio device
}

int sound_manager::getNumSets(void)
{
	return this->numSets;
}

std::string sound_manager::getSndSet(int set)
{
	//srcStrings->open();
	std::string mySet = (*srcStrings->getPlaceMap())[set];
	//srcStrings->close()

	return mySet;
}

//---------------------------
//Check to make sure there aren't any OpenAL errors
void sound_manager::check()
{
	ALCenum error = alcGetError(this->device);

	if(error != ALC_NO_ERROR)
	{
		std::stringstream ss;
		ss << "OpenAL error was raised: " << error  << std::endl; 
		throw std::runtime_error(ss.str());
	}
}
