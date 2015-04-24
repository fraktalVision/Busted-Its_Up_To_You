#include "busted.all.h"
#include "player/player.h"
#include "tile/tile.h"
#include "Game_Manager/Game_Manager.h"

//Manages gameplay, players, UI, etc. -- wrapper for whole experience
Game_Manager *Game_Man;
GraphicsEngine *Graph_En = NULL;
extern Texture_Manager* pTexture_man;

//Situation database
sqlDB* sitDB;


//Initialize the board array...
tile* tileArray[TILENUM];

//Create all the tiles -- TODO: get the hard values for safety reason, no need to generate them soon
void drawBoard(void)
{
	//vec3 pos = {0, 0, 0};
	vec3 pos = vec3(0, 0, 0);
	//Center the board
	pos.x += -((0.25f*(float)TILENUM)*10.0f)/2.0f; // half of one side
	pos.z += -((0.25f*(float)TILENUM)*10.0f)/2.0f; // half of one side

	//For every requested tile
	for(int i = 0; i < TILENUM; ++i)
	{
		tile* myTile = new tile;	//new tile
		myTile->setPina((*pModel_man->getModels())["TILE"]);		//associate with the tile mesh
		myTile->setNum(i);			//Set it's id number (in the array)
		myTile->setName("tile"+i);	//set it's name

		//If this is not the first tile, reposition it
		if(i != 0)
		{
			if(i <= (0.25f*(float)TILENUM))
				pos.x += 5.5f*1.8f;
			else if(i <= (0.5f*(float)TILENUM))
				pos.z += 5.5f*1.8f;
			else if(i <= (0.75f*(float)TILENUM))
				pos.x -= 5.5f*1.8f;
			else if(i <= TILENUM)
				pos.z -= 5.5f*1.8f;
		}
		myTile->setPos(pos);
		


		if ( i == 8 || i == 24 )
			myTile->setAction(TA_ROLL_AGAIN);
		else if ( i == 0 || i == 16 )
			myTile->setAction(TA_LOSE_TURN);


		//default if it doesn't meet the following conditionals...
		vec3 color = myTile->getColor().a;
		std::string type = "action";
		std::string texture = "TID_TILE_BLUE";


	
		

		//Based on the tile's position in the array, set it's color and type attributes
		if(i == 0 || i == (0.25*TILENUM) || i == (0.5*TILENUM) || i == (0.75*TILENUM))
		{
			color.x = 0.5; color.y = 0.5; color.z = 0.5;
			type = "funFriends";
			texture = "TID_TILE_BROWN";
		}
		else if(i == 1 || i == (0.25*TILENUM)+1 || i == (0.5*TILENUM)+1 || i == (0.75*TILENUM)+1)
		{
			color.x = 1; color.y = 0; color.z = 0;
			type = "school";
			texture = "TID_TILE_TEAL";
		}
		else if(i == 2 || i == (0.25*TILENUM)+2 || i == (0.5*TILENUM)+2 || i == (0.75*TILENUM)+2)
		{
			color.x = 0; color.y = 1; color.z = 0;		
			type = "health";	
			texture = "TID_TILE_PINK";
		}
		else if(i == 3 || i == (0.25*TILENUM)+3 || i == (0.5*TILENUM)+3 || i == (0.75*TILENUM)+3)
		{
			color.x = 0; color.y = 0; color.z = 1;	
			type = "burn";	
			texture = "TID_TILE_ORANGE";
		}
		else if(i == 4 || i == (0.25*TILENUM)+4 || i == (0.5*TILENUM)+4 || i == (0.75*TILENUM)+4)
		{
			color.x = 1; color.y = 1; color.z = 0;		
			type = "familyHome";
			texture = "TID_TILE_BLUE";
		}
		else if(i == 5 || i == (0.25*TILENUM)+5 || i == (0.5*TILENUM)+5 || i == (0.75*TILENUM)+5)
		{
			color.x = 0; color.y = 1; color.z = 1;			
			type = "work";	
			texture = "TID_TILE_YELLOW";
		}
		else if(i == 6 || i == (0.25*TILENUM)+6 || i == (0.5*TILENUM)+6 || i == (0.75*TILENUM)+6)
		{
			color.x = 1; color.y = 0; color.z = 1;			
			type = "money";	
			texture = "TID_TILE_GREEN";
		}
		else if(i == 7 || i == (0.25*TILENUM)+7 || i == (0.5*TILENUM)+7 || i == (0.75*TILENUM)+7)
		{
			color.x = 1; color.y = 1; color.z = 1;	
			type = "action";
			texture = "TID_TILE_PURPLE";
		}

		//Set the values derived in the above connditionals
		myTile->setColor(color);
		myTile->setType(type);
		myTile->setTexture(pTexture_man->GetTexture(texture));
		

		tileArray[i] = myTile; //put myself into the global tile array
	}// for tiles
}

//Pseudo-random number generator
int random(void)
{
	int low=1, high=100; 
	return random(low, high);
}
int random(int low, int high)
{
	int rand_int;
	int range=(high-low)+1;
	rand_int = low+int(range*rand()/(RAND_MAX + 1.0));

	return rand_int;
}
//Dice roll
int rollDice(void)
{
	int value = random(2, 12);

	return rollDice(value);
}
//For explicit values
int rollDice(int value)
{
	//Current player
	player* plyr = Game_Man->GetCurrentPlayer();

	if(!(plyr->bAtDest))
		return 0;

	if(DATA("debug")) { std::cout << "Dice Roll: " << value << std::endl; }
	//Set the player's "to" position
	plyr->incTile(value);
	return value;
}
