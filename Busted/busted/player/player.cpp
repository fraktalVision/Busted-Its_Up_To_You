/***********
Zachary Whaley
gamemogul@gmail.com
--Players--
* Pretty self-explanitory
***********/

#include "player.h"

//---------------------------
//Player

void player::update(float t)
{
	if(!this->bAtDest)
	{
		tile *MyTile = this->tmpTile;

		path_t += t/40.0f; //increment the t along the spline

		if( path_t > 1.0f /*this->posInc.x == 0 && this->posInc.z == 0*/ )
		{

			//spot setting
			/**/


			if(this->tmpTile->getNum() == this->getTile()->getNum())
			{
				vec3 position = vec3( (MyTile->getPosX() + MyTile->getBases()[this->getID()-1].x), (this->getPos().y ), (MyTile->getPosZ() + MyTile->getBases()[this->getID()-1].z) );
				this->setPos(position);

				this->atDest();
				path_t = 1.0f;
			}
			else
			{
				int tileNum = this->tmpTile->getNum()+1;

				while(tileNum > TILENUM-1)
					tileNum = tileNum - TILENUM;//yes, this is right

				this->tmpTile = tileArray[tileNum];
				if(DATA("debug")) { std::cout << tileNum << std::endl; }
				this->bAtDest = false;

				//figure out the tile to be moved to
				int nexttileNum = this->tmpTile->getNum()+1;

				while(nexttileNum > TILENUM-1)
					nexttileNum = nexttileNum - TILENUM;//yes, this is right

				tile *MyTile = tileArray[tileNum];

				//calc points of the spline
				vec3 v1 = vec3( this->pos.x, this->pos.y, this->pos.z);

				//point that is 5 units higher and a third the distance away from the the start
				vec3 v2 = vec3( (MyTile->getPosX() +MyTile->getBases()[this->getID()-1].x - this->pos.x)/3.0f + this->pos.x, 
							DATA("HOP_HT") + this->pos.y,
							(MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z - this->pos.z)/3.0f + this->pos.z);

				//point that is 5 units higher and two/thirds the distance away from the the start
				vec3 v3 = vec3( 2.0f*(MyTile->getPosX() +MyTile->getBases()[this->getID()-1].x - this->pos.x)/3.0f + this->pos.x, 
							DATA("HOP_HT") + this->pos.y,
							2.0f*(MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z - this->pos.z)/3.0f + this->pos.z);
				
				//point at the goal destination of the next tile
				vec3 v4 = vec3(	MyTile->getPosX() + MyTile->getBases()[this->getID()-1].x , 
							MyTile->getPosY() + MyTile->getBases()[this->getID()-1].y,
							MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z );

				//generate a spline based on the input points
				movepath = Spline( v1, v2, v3, v4);

				path_t -= 1.0f;


				//load in scaling animation
				v1 = vec3( DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"));
				v2 = vec3( DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_X"), 
							DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"), 
							DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Z"));
				v3 = vec3(DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_X"), 
							DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"), 
							DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Z"));
				
				scale_anim = Spline( v1, v2, v3, v1);
				
			}
		}


		//Get the stored rotation, apply the increment to it...
		vec3 rotation = vec3( (this->getRot().x + this->rotInc.x*t), (this->getRot().y + this->rotInc.y*t), (this->getRot().z + this->rotInc.z*t) );
		this->setRot(rotation);

		//Get the stored position, apply the increment to it...
		vec3 position = movepath.GetPoint( path_t );
		this->setPos(position);

		//get scale at the certain time
		if( path_t > 0.5f )
		{

			vec3 v1 = vec3( DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"));
			vec3 v2 = vec3( DATA("DEF_TOK_SCALE"), 
						DATA("DEF_TOK_SCALE"), 
						DATA("DEF_TOK_SCALE"));
			vec3 v3 = vec3(DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"), 
						DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_X"), 
						DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"));
			vec3 v4 = vec3(DATA("DEF_TOK_SCALE"), 
						DATA("DEF_TOK_SCALE"), 
						DATA("DEF_TOK_SCALE"));
			scale_anim = Spline( v1, v2, v3, v4);

			//scale path_t so that as it's >0.5, the value being given to the range is between 0 to 1
			vec3 scale = scale_anim.GetPoint( 2.0f*(path_t-0.5f) );
			this->setScale(scale);
		}
		else
		{
			vec3 scale = scale_anim.GetPoint(path_t*2.0f);
			this->setScale(scale);
		}

	}
}

void player::render(void)
{
	this->obj::render();
}

void player::atDest(void)
{
	this->bAtDest = true;
	tile_action = this->getTile()->getAction();
}




void player::setScore(int value)	{ this->score = value; }
void player::incScore(int value)	{ this->score += value; }
int player::getScore(void)			{ return this->score; }

void player::setTile(tile* myTile)	{ this->myTile = myTile; this->bAtDest = false; }
void player::setTile(int tile)		{ this->setTile(tileArray[tile]); }
void player::incTile(int tilenumber)
{
	//////////
	//Temp tile:
		int tileNum = this->getTile()->getNum() + 1;

		while(tileNum > TILENUM-1)
			tileNum = tileNum - TILENUM;//yes, this is right

		this->tmpTile = tileArray[tileNum];

	//////////
	//Dest tile:
		tileNum = this->getTile()->getNum();
		tileNum += tilenumber;

		while(tileNum > TILENUM-1)
			tileNum = tileNum - TILENUM;//yes, this is right

		this->setTile(tileArray[tileNum]);

		//Tile stats...
			stat.open();
			{
				std::stringstream tile, hits;
				tile << "Tile" << tileNum;
				hits << atoi( stat.read( tile.str() ).c_str() ) + 1;
				stat.write(tile.str(), hits.str());
			}
			stat.close();

			

		//calc points of the spline
		tile *MyTile = this->tmpTile;
		vec3 v1 = vec3( this->pos.x, this->pos.y, this->pos.z);
		vec3 v2 = vec3( (MyTile->getPosX() +MyTile->getBases()[this->getID()-1].x - this->pos.x)/3.0f + this->pos.x, 
					DATA("HOP_HT") + this->pos.y,
					(MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z - this->pos.z)/3.0f + this->pos.z);

		vec3 v3 = vec3( 2.0f*(MyTile->getPosX() +MyTile->getBases()[this->getID()-1].x - this->pos.x)/3.0f + this->pos.x, 
					DATA("HOP_HT") + this->pos.y,
					2.0f*(MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z - this->pos.z)/3.0f + this->pos.z);
		vec3 v4 = vec3(	MyTile->getPosX() + MyTile->getBases()[this->getID()-1].x , 
					MyTile->getPosY() + MyTile->getBases()[this->getID()-1].y,
					MyTile->getPosZ() +MyTile->getBases()[this->getID()-1].z );

		movepath = Spline( v1, v2, v3, v4);

		//load in scaling animation
		v1 = vec3( DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"));
		v2 = vec3( DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_X"), 
					DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"), 
					DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Z"));
		v3 = vec3(DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_X"), 
					DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Y"), 
					DATA("DEF_TOK_SCALE")*DATA("SCALE_MAX_Z"));
		
		scale_anim = Spline( v1, v2, v3, v1);

		path_t = 0.0f;
}
tile* player::getTile(void)			{ return this->myTile; }
tile* player::getTempTile(void)			{ return this->tmpTile; }


player::player() :	score(0),
					bAtDest(true),
					gender(GEN_MALE),
					myMentor(NULL),
					chance(0.5),
					tile_action( TA_NONE ),
					path_t(1.0f),
					lastMoral(POSITIVE),
					lastOutcome(true),
					stat("../Busted/assets/stats/tileStats.txt"),
					privacy(0)

					
{
	//Start at beginning...
	this->setTile(0);
	this->tmpTile = tileArray[0];

	vec4 pos = this->getTile()->getPos();
	pos.y += 2.5;
	this->setPos(pos);
	vec3 scale = vec3(DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"), DATA("DEF_TOK_SCALE"));
	this->setScale(scale);

	rot.y += 180.0f;	

	vec3 position = vec3( pos.x, pos.y, pos.z );
	movepath = Spline( position, position, position, position );
}

void player::Reset()
{
	score = 0;
	bAtDest = true;
	chance = 0.5;
	this->setTile(0);
	this->tmpTile = tileArray[0];

	vec4 pos; //= this->getTile()->getPos();
	pos.y = getTile()->pos.y + getTile()->getBases()[getID()-1].y;
	pos.x = getTile()->pos.x + getTile()->getBases()[getID()-1].x;
	pos.z = getTile()->pos.z + getTile()->getBases()[getID()-1].z;

	this->setPos(pos);

}

player::~player()
{
}


