/***********
Zachary Whaley
gamemogul@gmail.com
--Statistics Manager--
* Used to record statistics for determining % of occurrences and the like
* Specify a file on construction
	* If file does not exist it will be created
	* If it does, the values therein will absorbed each time it is accessed
	* Even if the program crashes, values will be stored from the last save point (last 'close' command)
***********/

#ifndef _SOUND_FILE_MAN_
#define _SOUND_FILE_MAN_

#include "../../core/stat_manager/stat_manager.h"

/*!	\brief Class to hold the names of mentor sounds
*	Note, the only one actually being used is 'happy' -- long story short, another way was found.
*/
class sndSet
{
public:
	std::string happy;
	std::string sad;
	std::string puzzled;

	//sndSet operator =(sndSet);
};

/*!	\brief Class to point to all the sounds
* This uses Stat_Manager as a base to catalog all sounds
*/
class sndFile_man : public Stat
{
private:
	std::map<std::string, sndSet>	sndSets;  //!<A map of all the sndSets
	std::map<int, std::string>		place_map; //!<a map of the names of soundsets by their order number
	int								numSets;	//!<The number of soundsets (for looping)
public:

	std::map<std::string, sndSet>* getSndSets(void);	//!< Retrieve the map
	std::map<int, std::string>* getPlaceMap(void);	//!< Retrieve the map

	virtual void open(void);

	virtual void copy(std::string);							//!< Copy the Stats from a file
	virtual void copy(std::map<std::string, std::string>*);//!< Copy the Stats from a normal Stat_Manager Map
	virtual void copy(std::map<std::string, sndSet>*);//!< Copy the Stats from a sndFile_man map

	virtual void close(void);

	virtual int getNumSets(void);//!<For looping

	sndFile_man(std::string);
	virtual ~sndFile_man(void);
};

#endif