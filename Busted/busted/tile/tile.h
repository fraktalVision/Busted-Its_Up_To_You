/***********
Zachary Whaley
gamemogul@gmail.com
--Board Tile--
* Tile players land on going around the board
* Each as a unique id # and has 1 of 8 category types to access the DB
	* action
	* burn
	* familyHome
	* funFriends
	* health
	* lifeHappens
	* school
	* work
***********/

#ifndef TILE_H
#define TILE_H

#include "../busted.all.h"

enum TILE_ACTION{ TA_NONE, TA_ROLL_AGAIN, TA_LOSE_TURN };//!<The actions to perform when players land on this tile

//!Class for the tiles player move across
class tile: public obj
{
private:
	std::string type;				//!<Situation type
	int tileNum;
	TILE_ACTION tile_action;		//!<What action to perform when players land on this tile

	vec3 bases[6];					//!<Points to go to for players landing on this tile

public:
	void setType(std::string type);	//!<Set which category this tile represents
	std::string getType(void);		//!<Return which category

	void setAction( TILE_ACTION a );//!<Set what the tile does when a player lands on it
	TILE_ACTION getAction();		//!<Return what the tile does when a player lands on it

	void setNum(int num);			//!<Set this tile's number on the board
	int getNum(void);				//!<Set this tile's number on the board

	vec3* getBases(void);//!<Bases that players go to on a tile

	tile();
	tile(int num);//!<Build the tile with a specific number on the board
	virtual ~tile();
};

#endif