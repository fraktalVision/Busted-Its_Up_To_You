#include "Sprite.h"

extern camera *Camera; //main camera for the game;

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
Sprite::Sprite()
{
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 0.0f;
	dim[0] = 0.0f;
	dim[1] = 0.0f;
	txt_coord0[0] = 0.0f;
	txt_coord0[1] = 0.0f;
	txt_coord1[0] = 0.0f;
	txt_coord1[1] = 0.0f;
	texture_id = 0;

	offset[0] = 0.0f;
	offset[1] = 0.0f;
	rot_angle = 0.0f;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
Sprite::Sprite( unsigned int tid, float x, float y, float z, float width, float height,
			   float t0x, float t0y, float t1x, float t1y ): texture_id(tid)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
	dim[0] = width;
	dim[1] = height;
	txt_coord0[0] = t0x;
	txt_coord0[1] = t0y;
	txt_coord1[0] = t1x;
	txt_coord1[1] = t1y;

	offset[0] = 0.0f;
	offset[1] = 0.0f;

	rot_angle = 0.0f;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
Sprite::Sprite( Sprite & s )
{
	(*this) = s;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
void Sprite::operator=(Sprite& s)
{
	pos.x = s.pos.x;
	pos.y = s.pos.y;
	pos.z = s.pos.z;
	dim[0] = s.dim[0];
	dim[1] = s.dim[1] ;
	texture_id = s.texture_id;

	txt_coord0[0] = s.txt_coord0[0];
	txt_coord0[1] = s.txt_coord0[1];
	txt_coord1[0] = s.txt_coord1[0];
	txt_coord1[1] = s.txt_coord1[1];

	offset[0] = s.offset[0];
	offset[1] = s.offset[1];

	rot_angle = s.rot_angle;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
void Sprite::SetPos(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
void Sprite::SetWidth(float w)
{
	dim[0] = w;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
void Sprite::SetHeight(float h)
{
	dim[1] = h;
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
float Sprite::GetWidth()
{
	return dim[0];
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
float Sprite::GetHeight()
{
	return dim[1];
}

/***********************************
Sprite Constructor

Author: Jamie Gault
***********************************/
void Sprite::Render()
{
	glPushMatrix();
			glTranslatef ( pos.x, pos.y, pos.z);
			glRotatef( -Camera->rot.y , 0.0f, 1.0f, 0.0f);
			glRotatef(-Camera->rot.x, 1.0f, 0.0f, 0.0f);

			//if the sprite is rotated
			if( rot_angle != 0.0f )
				glRotatef( rot_angle , 0.0f, 0.0f, 1.0f);

			if( offset[0] || offset[1] )
				glTranslatef ( offset[0] , offset[1], 0.0f);

			glScalef( dim[0]/2.0f, dim[1]/2.0f, 0.0f );

			glBindTexture(GL_TEXTURE_2D, texture_id);			// Bind text texture

			glBegin(GL_QUADS);						// Begin Drawing Quads

				glTexCoord2f(txt_coord1[0],txt_coord0[1]);
				glVertex3f(  1.0f ,  1.0f , 0.0f);

				glTexCoord2f(txt_coord0[0],txt_coord0[1]);
				glVertex3f(  -1.0f,   1.0f , 0.0f);

				glTexCoord2f(txt_coord0[0],txt_coord1[1]);
				glVertex3f(  -1.0f  , -1.0f , 0.0f);

				glTexCoord2f(txt_coord1[0],txt_coord1[1]);
				glVertex3f(  1.0f , - 1.0f , 0.0f);

			glEnd();			// Done Drawing Quads

	glPopMatrix();
}