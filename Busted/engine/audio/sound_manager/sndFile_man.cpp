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

#include "sndFile_man.h"

//---------------------------
//Constructors/Destructors
sndFile_man::sndFile_man(std::string filename) : numSets(0)
	{
		//File to open
		this->filename = filename;
		mode = std::ios::in;
	}

	sndFile_man::~sndFile_man(void){}

	/*sndSet sndSet::operator =(sndSet param)
	{
		sndSet mySet;
		mySet.happy = param.happy;
		mySet.sad = param.sad;
		mySet.puzzled = param.puzzled;
		return mySet;
	}*/

//---------------------------
//Get a handle on the sound-sets for the players
std::map<std::string, sndSet>* sndFile_man::getSndSets(void)
{
	return &this->sndSets;
}

//---------------------------
//Get a handle on the sound-sets for the players
std::map<int, std::string>* sndFile_man::getPlaceMap(void)
{
	return &this->place_map;
}

//---------------------------
//Open the file for writing
void sndFile_man::open(void)
{
	this->Stat::open();

	//Dump the values in the map and file-list
	this->place_map.clear();
	this->sndSets.clear();

	//Iterate the vector into the map
	for(std::vector<std::string>::iterator fi = parseFile.begin(); fi != parseFile.end(); ++fi )
	{
		//If we're importing another file
		if((*fi) == "FILE")
		{
			//Do this all over again for this new file...
			copy(*++fi);				//Absorb new file's values
		}
		else if((*fi) == "SNDSET")
		{
			sndFile_man stat(*++fi);
			stat.open();
				sndSet set;

				set.happy = stat.read("happy");
				set.sad = stat.read("sad");
				set.puzzled = stat.read("puzzled");

				this->sndSets[stat.read("name")] = set;
				this->place_map[int(sndSets.size())-1] = stat.read("name");
			stat.close();
		}
	}

	numSets = int(sndSets.size());
}

////---------------------------
////Copy one map into this one from a file
void sndFile_man::copy(std::string File)
{
	sndFile_man *stat = new sndFile_man(File);	//Temp sndFile_man
	stat->open();			//Open and copy
	
	//Copy values from the new map into my local one
	this->copy(stat->getVars());

	this->copy(stat->getSndSets());

	stat->close();
	delete stat;
}

//---------------------------
//Copy one map into this one
void sndFile_man::copy(std::map<std::string, std::string> *Map)
{
	//Iterate through target map and copy values...
	for(std::map<std::string, std::string>::iterator si = Map->begin(); si != Map->end(); ++si)
	{
		if((*si).first != "SNDSET")
			this->vars[(*si).first] = (*si).second;
	}
}

//---------------------------
//Copy one map into this one
void sndFile_man::copy(std::map<std::string, sndSet> *Map)
{
	//Iterate through target map and copy values...
	for(std::map<std::string, sndSet>::iterator si = Map->begin(); si != Map->end(); ++si)
	{
		this->sndSets[(*si).first] = (*si).second;
		this->place_map[int(sndSets.size())-1] = (*si).first;
	}
}

void sndFile_man::close(void)
{
	this->Stat::close();

	//Dump the values in the sndSet map
	this->sndSets.clear();
	//this->place_map.clear();
}

int sndFile_man::getNumSets(void)
{
	return this->numSets;
}