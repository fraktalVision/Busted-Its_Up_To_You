/***********
Zachary Whaley
gamemogul@gmail.com
--Ogg Stream--
* Basic streaming object for OGG Vorbis sound sources
***********/

#ifndef OGG_STREAM_H
#define OGG_STREAM_H

#include <string>
#include <iostream>
#include <sstream>

#include <al/al.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#define BUFFER_SIZE 16384

enum SND_TYPE {SFX, MUSIC};	//Is the sound an effect or a song? (two differen volumes)
enum OGG_STATE { OGG_CLOSED, OGG_PLAY, OGG_PAUSED, OGG_STOPPED }; //What's the current state of the sound?

//!Class for encapsulating the ogg sound format and playing effects
class ogg_stream
{
private:
	std::string		filename;
	FILE*           oggFile;		//!< file handle
	OggVorbis_File  oggStream;		//!< stream handle
	vorbis_info*    vorbisInfo;		//!< some formatting data
	vorbis_comment* vorbisComment;	//!< user comments
	ALuint			source;			//!< audio source
	ALenum			format;			//!< internal format

	int				NUM_BUFFERS;	//!<Number of buffers this sound should be swapping in and out of

	OGG_STATE		oggState;		//!<Current state of the stream

private:
	void open();					//!< obtain a handle to the file
	void release();					//!< release the file handle
	bool loadPcm(ALuint buffer);	//!< load PCM data into buffer
	void resetOggStream();			//!< reset ogg stream

public:
	ALuint			*buffers;		//!< front and back buffers
	bool			loop;
	SND_TYPE		type;

	ALfloat			gain;			//!<Current fade amount
	ALfloat			maxgain;		//!<maximum amount of gain
	ALfloat			fade;			//!<fade incrememnt amount

	OGG_STATE getOggState(void)	{
		return oggState;
	}
	ALenum getState(void) {
		ALenum state = 0;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		check();
		return state;
	}

	//Init
	void display();					//!< display some info on the Ogg

	//Play
	void update(float volume = 0.5f);					//!< update the stream if necessary

	//Modify
	void pause(void);				//!< Set the ogg to it's pause state
	void play(void);				//!< Set the ogg to it's play state
	void endLoop_cb(bool anyway = false); //!<A callback at the end (override to do this anyway, even if it does loop)

	//Exit
	void stop(void);				//!< Set to stop state (will perform a release, etc.)

	ogg_stream(std::string file, SND_TYPE type = SFX, bool loop = false, int NUM_BUF = 2);
	~ogg_stream(void);

protected:
	void check();						//!< checks OpenAL error state
	std::string errorString(int code);	//!< stringify an error code
};

#endif