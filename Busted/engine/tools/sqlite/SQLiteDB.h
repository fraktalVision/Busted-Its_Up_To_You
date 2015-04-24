/***********
Zachary Whaley
gamemogul@gmail.com
--SQLite Database Object--
* An object to load up a SQLite database file, and access it's contents
***********/

#ifndef _SQLITEDB_H_
#define _SQLITEDB_H_

#include <sqlite3.h>

class sqlDB
{
private:
	std::string		filename;			//Name of DB file
	sqlite3			*db;				//Database pointer
	int				rc;					//Error status
	char			*zErrMsg;			//Error message

	//std::vector<std::string> queries;	//List of previous queries

public:
	std::vector<std::map<std::string, std::string>*> rows; //List of rows that callbacks can modify...

	//Open the db for reading and executing
	bool open(std::string filename);

	//Execute a query on the database
	bool exec(std::string query);

private:
	//What to do after a query is executed
	static int		callback(void *NotUsed, int argc, char **argv, char **azColName);

public:
	//Close the database, finilize everything (not much, really)
	bool close(void);

	sqlDB();
	sqlDB(std::string filename);
	virtual ~sqlDB();
};
#endif