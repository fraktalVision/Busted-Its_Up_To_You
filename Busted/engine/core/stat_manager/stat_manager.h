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

#ifndef STAT_MANAGER_H
#define STAT_MANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

//!<Class to take input data
class Stat
{
protected:
	std::string							filename;	//!<File to open, read, and write to
	std::ios::open_mode					mode;		//!<How to open it (can make it read-only with std::ios::in)
	std::ofstream						file;		//!<Output file handle
	std::vector<std::string>			parseFile;
	std::map<std::string, std::string>	vars;		//!<Key-value pair map
	std::vector<std::string>			files;		//!<Any files sourced by the doc

public:
	virtual void open(void);								//!<Open file for reading and writing

	virtual std::map<std::string, std::string>* getVars(void)	{ return &this->vars; }

	virtual std::string read(std::string var);				//!<Get a value from the parsed keys
	virtual void copy(std::map<std::string, std::string>*);	//!<Copy one map into this one
	virtual void copy(std::string);							//!<Copy one map into this one from a file
	virtual void write(std::string key, std::string val);	//!<Set a value from the parsed keys

	virtual void close(void);								//!<Write to file, and close file

	//Construct it default with truncate enabled
	Stat(void);
	Stat(std::string filename, std::ios::open_mode mode = std::ios::trunc);//!<Truncate will clear out the file after it opens -- be careful!  if you only want to read, use std::ios::in
	virtual ~Stat(void);
};

#endif