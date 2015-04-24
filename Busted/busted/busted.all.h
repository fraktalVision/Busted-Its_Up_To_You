/***********
Zachary Whaley
gamemogul@gmail.com
--BUSTED.ALL.H--
* Throw as many globally needed things in here for the game as possible
* There is also an 'all.h' which houses the globals necessary for the engine,
  this has the things necessary for the game (portability)
***********/

#include "../engine/core/all.h"
#include "../engine/core/classes.h"
//#include "../engine/tools/xml/tinyxml.h"
#include "../engine/tools/sqlite/SQLiteDB.h"
#include "../engine/tools/textdisplay/TextManager.h"
#include "../engine/tools/textureload/textureImporter.h"
#include "../engine/tools/datadrive/datadrive.h"
#include "../engine/core/glut/Input.h"
#include "../engine/core/Sprite.h"

#ifndef BUSTED_ALL_H_
#define BUSTED_ALL_H_

//Forward declataions
	class tile;
	class player;
	enum MORALITY { //!< enum for the lastMoral variable since we've added neutral options.
		NEGATIVE = -1, 
		NEUTRAL = 0,
		POSITIVE = 1
	}; 

//Situation database
extern sqlDB* sitDB;

//Globalize these functions
int random(void);				//random between 1, 100
int random(int low, int high);	//random between what you specify
int rollDice(void);				//random dice movement between 2, 12
int rollDice(int value);		//move the player an explicit amount

#define TILENUM 32				//Board size
void drawBoard(void);			//generate the tiles

//array for all the tiles on the board
extern tile* tileArray[TILENUM];


#endif