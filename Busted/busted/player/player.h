/***********
Zachary Whaley
gamemogul@gmail.com
--Players--
* Pretty self-explanitory
***********/

#ifndef PLAYER_H
#define PLAYER_H

#include "../busted.all.h"
#include "../Mentor.h"		//Each player gets a mentor
#include "../tile/tile.h"	//Players have their destination tile, and a temp tile (the immediate next one)
#include "../../engine/core/stat_manager/stat_manager.h"

//!class for each player
class player: public obj
{
private:
	int		score;		//!<Player score
	int		pID;		//!<Player ID
	tile*	myTile;		//!<Final destination tile
	tile*	tmpTile;	//!<Immediate tile player will go to
	Mentor* myMentor;	//!<Player's unique Mentor
	GENDER	gender;		//!<gender of the player
	float	chance;		//!<Chance of good/bad result for option
	int     outcomeID;  //!<stores the outcome ID to be used with selecting mentor comment

	std::string	soundSet;	//What sounds to play on my turn

	Stat	stat;

	std::string name;	//!<name of the particular user.

	Spline	movepath;	//!<path about which the tokens move
	float	path_t;		//!<time along the path based on the t along the spline

	Spline	scale_anim; //!<interpolates the scale values over the path time

	void setScore(int value);

public:

	MORALITY	lastMoral;	//!<Whether or not the last choice was good
	bool	lastOutcome;//!<Wheter or not the last outcome was good

	int		privacy;	//!<number of privacy calls the user has left

	virtual void render(void);

	void setID( int num )			{ this->pID = num; }	//!<Set my player ID
	int  getID( void )				{ return this->pID; }	//!<Get my player ID

	void setMentor( Mentor* men )	{ myMentor = men; }		//!<Set my Mentor
	Mentor* getMentor( )			{ return myMentor; }

	void setGender( GENDER gen )	{ gender = gen; }		//!<Set my Mentor
	GENDER getGender( )				{ return gender; }

	void setSndSet( std::string set )	{ soundSet = set; }		//!<Set my sound set
	std::string getSndSet(void)			{ return soundSet; }	//!<Reutnr sound set

	void setName( std::string Name )	{	name = Name; }		//!<Set name
	std::string getName( )					{ return name; }	//!<Get name

	void setOutID( int id )         { outcomeID = id; }         //!<Set outcome ID
	int getOutID( )                 { return outcomeID; }        //!<Get outcome ID

	void setChance( int chan )		{ if(chan > 1) { chan = 1; } else if(chan < 0) { chan = 0; } chance = chan; out();} //!<modify the possibility of a good/bad outcome manually
	int	 getChance( void )			{ return chance; out(); }//!<For use in determining a good/bad outcome
	void incChance( void )			{ this->chance += 0.1*(1-chance); out(); }//!<Generally increment
	void decChance( void )			{ this->chance -= 0.1*(1-chance); out(); }//!<Generally decrement
	void out(void)					{ if(DATA("debug")) { std::cout << "Chance: " << chance << std::endl; } }//!<Tell the user *in debug mode* what chance is

	void incScore(int value);								//!<Increment the score
	int  getScore(void);										//!<Retrieve the score

	bool bAtDest;											//!<Am I at my destination?
	void atDest(void);										//!<Called when reaching destination tile

	TILE_ACTION tile_action;								//!<marks an event like landing on roll again or lose a turn

	//Player's tile...
	void setTile(tile* myTile);								//!<Set my destination tile by pointer
	void setTile(int tile);									//!<Set my destination tile by ID
	void incTile(int tile);									//!<Increment the destination around the board
	tile* getTile(void);									//!<What's my tile?
	tile* getTempTile(void);

	void update(float t);									//!<Should I move, should I this, should that, etc.?

	void Reset();											//!<Clear every thing

	player();
	virtual ~player();
};

#endif