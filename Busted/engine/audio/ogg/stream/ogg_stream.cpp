/***********
Zachary Whaley
gamemogul@gmail.com
--Ogg Stream--
* Basic streaming object for OGG Vorbis sound sources
***********/

#include "ogg_stream.h"

#include "../../../tools/datadrive/datadrive.h"

extern class DataDrive* DATA_MAN;
#define DATA DATA_MAN->GetData

//---------------------------
//Constructors/Destructors
//---------------------------
//Initialize
ogg_stream::ogg_stream(std::string filename, SND_TYPE t, bool l, int NUM_BUF) 
	:	oggFile(NULL),
		gain(1.0f),
		fade(0.1f),
		maxgain(2.5f),
		vorbisInfo(NULL),
		vorbisComment(NULL),
		oggState(OGG_CLOSED)
{
	this->filename = filename;
	this->type = t;
	loop = l;
	NUM_BUFFERS = NUM_BUF;
	buffers = new ALuint[NUM_BUF];
	memset(buffers, 0, sizeof(ALuint) * NUM_BUF);

	open();
}

//---------------------------
//Close out
ogg_stream::~ogg_stream(void) 
{
	release();
}

//---------------------------
//Open the stream, start playing the source
void ogg_stream::open(void)
{		
	check();
	alGenBuffers(NUM_BUFFERS, buffers);						//Create buffers to play the sounds from
	check();
	alGenSources(1, &source);								//Create one source per sound
	check();

	//Test, load, and prepare the OGG file
	if(!(oggFile = fopen(filename.c_str(), "rb")))			//Make sure you can open the file before trying to stream it
	{
		this->release();
		throw std::runtime_error("Could not open Ogg file.");
	}
	else
	{
		if(DATA("debug")) { std::cout << "OGG File: " << filename << std::endl; }
	}

	int result = ov_open(oggFile, &oggStream, NULL, 0);		//Open an ogg file to a location in memory
	if(result < 0) {
		fclose(oggFile);
		throw std::runtime_error("Could not open Ogg stream. ERROR: " + errorString(result));
	}
	vorbisInfo = ov_info(&oggStream, -1);					//Grab file header info
	vorbisComment = ov_comment(&oggStream, -1);				//User comments (more headers)

	//Mono or stereo?
	if(vorbisInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
	check();
	alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
	check();
	alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
	check();
	alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
	check();

	oggState = OGG_STOPPED;
}

void ogg_stream::release()
{
	if(oggState == OGG_CLOSED) {
		return;
	}

	// stop playback
	pause();

	// delete sources
	alDeleteSources(1, &source);
	check();

	//Buffers are released *after* sources
	for(int i = 0; i < NUM_BUFFERS; ++i) {
		if(alIsBuffer(buffers[i]) == AL_TRUE) {
			alDeleteBuffers(1, &buffers[i]);
			check();
		}
	}

	// release orbis handle
	ov_clear(&oggStream);

	// close the file
	fclose(oggFile);

	oggState = OGG_CLOSED;
}


//---------------------------
//Stream audio bits from the ogg file, into buffers in soundcard memory
bool ogg_stream::loadPcm(ALuint buffer)
{
	char data[BUFFER_SIZE];
	int  size = 0;
	int  section;
	int  result;

	// load no more than BUFFER_SIZE bytes into ALuint buffer
	while(size < BUFFER_SIZE) {
		//read data from the file, load it into memory, section by section
		result = ov_read(&oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);
		//If everything is fine, append the size for a running total
		if (result > 0) {
			size += result;
		} else if (result == 0) {
			// no more data in OGG - break
			break;
		} else {
			throw std::runtime_error("OGG error: " + errorString(result));
		}
	}

	if (size == 0) {
		// we've reached the end of ogg stream; reset it
		resetOggStream();
		return false;
	}

	// put loaded data into ALuint buffer
	alBufferData(buffer, format, data, size, vorbisInfo->rate);
	check();
	return true;
}

//---------------------------
//Play
//---------------------------
//Update the stream
void ogg_stream::update(float volume)
{
	if (oggState == OGG_PLAY) {
		int processed = 0;

		gain += fade;
		if(gain < 0.0f) {
			fade = 0.0f;
			gain = 0.0f;
		} else if(gain > maxgain) {
			fade = 0.0f;
			gain = maxgain;
		}

		if(gain == 0) {
			stop();
			return;
		}

		alSourcef(source, AL_GAIN, gain * volume);
		check();

		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		check();

		//Cycle through the buffers and swap them
		for ( ; processed > 0; --processed) {
			ALuint buffer;

			//1. swap
			alSourceUnqueueBuffers(source, 1, &buffer);
			check();
				
			//2. Copy buffer data to the soundcard
			if (!loadPcm(buffer)) {
				if (!loop) {
					stop();
					break;
				} else {
					loadPcm(buffer);
				}				
			}

			//3. write to the new buffer
			alSourceQueueBuffers(source, 1, &buffer);
			check();
		}

		// check if we're in the right AL state - it can be reset for some reason (in 
		// window mode, move the window with mouse and sound will be reset)
		if (getState() != AL_PLAYING) {
			int queuedBuffers;
			alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);
			check();
			if (queuedBuffers) {
				alSourcePlay(source);
				check();
			}
		}
	}
}

//---------------------------
//Modify
//---------------------------
//Constructors/Destructors
void ogg_stream::pause(void)
{
	if (oggState != OGG_PLAY) {
		return;
	}
	alSourceStop(source);

	int queued = 0;
	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
	check(); 
		
	while(queued--) {
		ALuint buffer;
		alSourceUnqueueBuffers(source, 1, &buffer);
		check();
	}

	oggState = OGG_PAUSED;
}

void ogg_stream::play(void)		
{ 
 	if (oggState == OGG_PLAY) {
		return;
	}

	//Cycle through all the buffers, and update their contents with new data
	for(int i = 0; i < NUM_BUFFERS; ++i) { 
		loadPcm(buffers[i]);
	}

	//Stream new data into the buffer
	alSourceQueueBuffers(source, NUM_BUFFERS, buffers);
	check();

	//Play the newly loaded source
	alSourcePlay(source);
	check();

	oggState = OGG_PLAY;
}

void ogg_stream::stop(void)		
{ 
	pause();
	resetOggStream();
	oggState = OGG_STOPPED; 
}

//---------------------------
//Constructors/Destructors
void ogg_stream::resetOggStream()
{
	//Rewind
	if (!ov_seekable(&oggStream)) {
		throw std::runtime_error("Ogg not seekable");		//You can seek your position in the file
	}

	//Reset the stream position to the beginning of the ogg file
	//int result = ov_pcm_seek (&oggStream, oggStream.offset);
	int result = ov_time_seek(&oggStream, 0.0f);
	if (result < 0) {
		throw std::runtime_error("Ogg error: " + errorString(result));
	}
}

//---------------------------
//Check for errors
void ogg_stream::check()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		std::stringstream ss;
		ss << "OpenAL error was raised: " << error  << " in " << this->filename << std::endl; 
		throw std::runtime_error(ss.str());
	}
}

//---------------------------
//Report errors
std::string ogg_stream::errorString(int code)
{
	switch(code) {
	case OV_EREAD:
		return std::string("Read from media.");
	case OV_ENOTVORBIS:
		return std::string("Not Vorbis data.");
	case OV_EVERSION:
		return std::string("Vorbis version mismatch.");
	case OV_EBADHEADER:
		return std::string("Invalid Vorbis header.");
	case OV_EFAULT:
		return std::string("Internal logic fault (bug or heap/stack corruption.");
	default:
		return std::string("Unknown Ogg error.");
	}
}

//---------------------------
//Show file info
void ogg_stream::display()
{
	if(DATA("debug")) { 
		std::cout
			<< "//-----OGG FILE-----"								<< "\n"
			<< "version         "	<< vorbisInfo->version			<< "\n"
			<< "channels        "	<< vorbisInfo->channels			<< "\n"
			<< "rate (hz)       "	<< vorbisInfo->rate				<< "\n"
			<< "bitrate upper   "	<< vorbisInfo->bitrate_upper	<< "\n"
			<< "bitrate nominal "	<< vorbisInfo->bitrate_nominal	<< "\n"
			<< "bitrate lower   "	<< vorbisInfo->bitrate_lower	<< "\n"
			<< "bitrate window  "	<< vorbisInfo->bitrate_window	<< "\n"
			<< "\n"
			<< "vendor "			<< vorbisComment->vendor		<< "\n";

		for(int i = 0; i < vorbisComment->comments; i++)
			std::cout << "   " << vorbisComment->user_comments[i] << std::endl; 

		std::cout << std::endl; 
	}
}

