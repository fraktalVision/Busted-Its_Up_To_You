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

#include <iterator>
#include "stat_manager.h"
#include "../../tools/datadrive/datadrive.h"

extern class DataDrive* DATA_MAN;
#define DATA DATA_MAN->GetData

//---------------------------
//Constructors/Destructors
	//You *could* make an empty one, but....
	Stat::Stat(void){ this->mode = std::ios::trunc; }
	//Do not construct without a filename!!!
	Stat::Stat(std::string filename, std::ios::open_mode Mode)
	{
		//File to open and write to
		this->filename = filename;
		mode = Mode;
	}
	Stat::~Stat(void)
	{
		//If the file is open, write all data, and bail out
		if(this->file.is_open())
			close();
	}

//---------------------------
//Open the file for writing
void Stat::open(void)
{
	//Dump the values in the map and file-list
	this->vars.clear();
	this->files.clear();
	this->parseFile.clear();

	//Open and close the file to do a test...
		std::ifstream inp;
		inp.open(filename.c_str(), std::ifstream::in);
		inp.close();

	//If opening the file failed, then it doesn't exist
	if(inp.fail())
	{
		if(DATA("debug")) { std::cout << "No previous file." << std::endl; }

		inp.clear(std::ios::failbit);					//Reset the failbit for next time

		file.open(filename.c_str(), mode);		//Open it in whatever mode specified in construction
	}
	//Else, it does exist, and I must absorb the values
	else
	{
		if(DATA("debug")) { std::cout << "Previous file." << std::endl; }

		//Re-open
		inp.open(filename.c_str(), std::ifstream::in);
		//Iterate the file into a vector
		std::vector<std::string> wholeFile( (std::istream_iterator<std::string>( inp )), std::istream_iterator<std::string>() );
		this->parseFile = wholeFile;

		//Absorb all the values into a map
		if(parseFile.size() % 2 == 0) //Only if there are key-value pairs
		{
			//Iterate the vector into the map
			for(std::vector<std::string>::iterator fi = parseFile.begin(); fi != parseFile.end(); ++fi )
			{
				//If we're importing another file
				if((*fi) == "FILE")
				{
						++fi;
					//Safety for now... values from sucked in files get placed in old file if std::ios::trunc
					if(mode == std::ios::in)
					{
						//Do this all over again for this new file...
						if(DATA("debug")) { std::cout << "Linked file: " << (*fi) << std::endl; }

						files.push_back(*fi);	//To write back out, later...
						copy(*fi);				//Absorb new file's values
					}
					else
						if(DATA("debug")) { std::cout << "WARNING: File '" << (*fi) << "' was not linked, the openmode for this file is innapropriate." << std::endl; }
				}
				else
				{
					//This doesn't work, fi++ is evaluated before the de-reference
					//this->vars[(*fi++)] = (*fi);
					std::string key = (*fi);
					++fi;
					this->vars[key] = (*fi);
				}
			}
		}

		//close the input-file
		inp.close();

		//open the file for writing, which will overwrite any previous data
		file.open(filename.c_str(), mode);
	}
}

//---------------------------
//Return a value from the map
std::string Stat::read(std::string var)
{
	//If the parsed map has a size, return the indice's value, otherwise return a blank
	return (this->vars.size() > 0)?this->vars[var]:"";
}

//---------------------------
//Copy one map into this one from a file
void Stat::copy(std::string File)
{
	Stat stat(File);		//Temp stat_manager
	stat.open();			//Open and copy

	//Copy values from the new map into my local one
	this->copy(stat.getVars());

	stat.close();
}

//---------------------------
//Copy one map into this one
void Stat::copy(std::map<std::string, std::string> *Map)
{
	//Iterate through target map and copy values...
	for(std::map<std::string, std::string>::iterator si = Map->begin(); si != Map->end(); ++si)
		this->vars[(*si).first] = (*si).second;
}

//---------------------------
//Set a key's value in the map
void Stat::write(std::string key, std::string val)
{
	//If the file is open (meaning it has been parsed into the map), set the key-value pair in the map
	if(this->file.is_open())
		vars[key] = val;
}

//---------------------------
//Write to the file, close the file (if it's writable)
void Stat::close(void)
{
	if(mode != std::ios::in)
	{
		//Iterate over the file-list, writing to the file-stream
		for(std::vector<std::string>::iterator fi = this->files.begin(); fi != files.end(); ++fi )
		{
			file << "FILE " << (*fi) << std::endl;
		}

		//Iterate over the map, writing to the file-stream
		for(std::map<std::string,std::string>::iterator mi = vars.begin(); mi != vars.end(); ++mi )
		{
			file << (*mi).first << " " << (*mi).second << std::endl;
		}
	}

	//Dump the values in the map and file-list
	this->vars.clear();
	this->files.clear();
	this->parseFile.clear();

	//Close the file
	file.close();
}