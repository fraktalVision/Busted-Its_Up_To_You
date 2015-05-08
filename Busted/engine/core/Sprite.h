#pragma once

#include "all.h"
#include "structures.h"

//!Class that is used for handling billboard sprites that are rendered in the world.
class Sprite
{
private:
	float	dim[2]; //!<width and height
	float	offset[2]; //!<vector to offset the picture based on the center point

public:
	vec3	pos;

	float	txt_coord0[2]; //!<texture space coordinate upper corner
	float	txt_coord1[2];//!<texture space coordinat lower corner;
	unsigned int texture_id; //!<id for the texture being used
	float	rot_angle; //!<angle of rotation

	Sprite();
	Sprite( unsigned int tid, float x, float y, float z, float width, float height,
			float t0x=0.0f, float t0y=0.0f, float t1x=1.0f, float t1y=1.0f );
	Sprite( Sprite & s );
	void operator=(Sprite& s);

	void SetPos(float x, float y, float z);

	void SetWidth(float w);
	void SetHeight(float h);
	float GetWidth();
	float GetHeight();

	void SetOffset(float x, float y){ offset[0] = x; offset[1] = y; }

	void Render();
};