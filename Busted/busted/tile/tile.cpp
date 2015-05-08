#include "tile.h"

//---------------------------
//Board tile
void tile::setType(std::string type)	{ this->type = type;	}
std::string tile::getType(void)			{ return this->type;	}

void tile::setAction( TILE_ACTION a )	{ tile_action = a;		}
TILE_ACTION tile::getAction()			{ return tile_action;	}

void tile::setNum(int num)				{ this->tileNum = num;	}
int tile::getNum(void)					{ return this->tileNum; }

tile::tile(): tile_action( TA_NONE )
{
	vec3 points[6] =	{
							vec3(2, DATA("TILE_HEIGHT") ,2),
							vec3(2, DATA("TILE_HEIGHT") ,-2),
							vec3(0, DATA("TILE_HEIGHT") ,-2),
							vec3(-2, DATA("TILE_HEIGHT") ,-2),
							vec3(-2, DATA("TILE_HEIGHT") ,2),
							vec3(0, DATA("TILE_HEIGHT") ,2)
						};
	for(int i = 0; i < 6; ++i)
		this->bases[i] = points[i];

	vec3 scale = vec3(2.0f, 2.0f, 2.0f);
	this->setScale(scale);

	this->setType("Action");

	this->setNum(0);
}
vec3* tile::getBases(void)	{ return this->bases; }
tile::tile(int num)
{
	tile::tile();

	this->setNum(num);
}
tile::~tile()
{
}