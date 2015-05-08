/***********
Zachary Whaley
gamemogul@gmail.com
--SQLite Database Object--
* An object to load up a SQLite database file, and access it's contents
***********/

#include "../../../busted/busted.all.h"
#include "SQLiteDB.h"

//---------------------------
//Constructors/Destructors
	sqlDB::sqlDB()
	{
		this->filename = "";
		this->zErrMsg = 0;
		this->rc = SQLITE_OK;
	}
	sqlDB::sqlDB(std::string filename)
	{
		this->sqlDB::sqlDB();
		sqlDB::open(filename);
	}
	sqlDB::~sqlDB(void){};

//---------------------------
//Open the db for reading and executing
bool sqlDB::open(std::string filename)
{
	this->filename = filename;
	this->zErrMsg = 0;

	this->rc = sqlite3_open(this->filename.c_str(), &this->db);
	if( this->rc )
	{
		fprintf(stderr, "Can't open database: %s\nError Code: %d", sqlite3_errmsg(db), sqlite3_errcode(db));
		sqlite3_close(db);
		return 1;
	}

	if( this->rc == SQLITE_OK )
		return 0;
	else
		return 1;
}

//---------------------------
//Execute a query on the database
bool sqlDB::exec(std::string query)
{
	//Clear previously queried data
	this->rows.clear();

	//Execute query.c_str() on this->db, using &this->callback as what to do afterward, return errors in zErrMsg
	this->rc = sqlite3_exec(this->db, query.c_str(), &this->callback, 0, &this->zErrMsg);

	if( this->rc == SQLITE_OK ) //if it did succeed
	{
		//this->queries.push_back(query);
		return 0;
	}
	else //if it did not succeed
	{
		fprintf(stderr, "SQL error: %s\n", this->zErrMsg);
		sqlite3_free(this->zErrMsg);
		return 1;
	}
}

//---------------------------
//What to do after a query is executed
int sqlDB::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	std::map<std::string,std::string> *myMap = new std::map<std::string,std::string>;
	for(int i=0; i<argc; i++)
	{
		(*myMap)[azColName[i]] = (argv[i] ? argv[i] : "NULL");
		//if(DATA("debug")) { std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl; }
	}
	//this->rows.push_back(myMap);
	//HAAAAAAACK!!! Fix later PLEASE!
	//sqlite3_callback has to be static because of its footprint, but statics don't have 'this' pointers
	//Calling the singleton explicitly
	sitDB->rows.push_back(myMap);
	return 0;
}

//---------------------------
//Close the database, finilize everything (not much, really)
bool sqlDB::close(void)
{
	this->rc = sqlite3_close(this->db);

	if( this->rc == SQLITE_OK )
		return 0;
	else
	{
		fprintf(stderr, "SQL error: %s\n", this->zErrMsg);
		sqlite3_free(this->zErrMsg);

		return 1;
	}
}