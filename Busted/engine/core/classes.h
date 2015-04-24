/***********
Zachary Whaley
gamemogul@gmail.com
--Thing--
* Base class for nearly everything

--Locator--
* Base location class

--Obj--
* Base 3D class

--Camera--
* Matrix to modify the render-matrix
***********/

#ifndef _CLASSES_H_
#define _CLASSES_H_

#include "all.h"
#include "../tools/importer/pinaImporter.h"

//---------------------------
//!Base-class of every "thing"
class thing
{
private:
	std::string name;	//!<What am I called by the user?
	int id;				//!<What am I called by the application?
	bool bVisible;		//!<See it or not?
	bool bSelected;		//!<Selected?

	//Anything that needs to be in every object goes here

public:
	void		setName(std::string name)	{ this->name = name; };
	std::string	getName(void)				{ return this->name; };
	void		setID(int id)				{ this->id = id; };
	int			getID(void)					{ return this->id; };

	void setVisible(bool myBool);//!<Set whether or not to render
	bool isVisible(void);//!<Do I render it?
	
	void setSelected(bool myBool);//!<
	bool isSelected(void);//!<

	thing();
	virtual ~thing();
};

//---------------------------
//!Basic 3D object with no graphical parts...
//* Used for orientation of things
class locator : public thing
{
private:
public:
	vec4 pos;	//!<Translation...
	vec3 rot;	//!<Rotation...
	vec3 scale;	//!<Scale...

	//Increment values...
	//These values control the frame-by-frame movement of the object
	vec4 posInc;//!<How much the position is incremented by each frame (deprecated -- use other means)
	vec3 rotInc;//!<How much the rotation is incremented by each frame (deprecated -- use other means)
	vec3 scaleInc;//!<How much the scale is incremented by each frame (deprecated -- use other means)

	//Position
		void setPos(vec4 pos);//!<Set position
		void setPos(vec3 pos);//!<
		void setPos(vec2 pos);//!<
		void setPos(float x, float y, float z);//!<

		vec4 getPos(void);//!<Get the position
		float getPosX(void);//!<
		float getPosY(void);	//!<
		float getPosZ(void);//!<

	//Rotation
		void setRot(vec3 rot);//!<Set rotation
		vec3 getRot(void);//!<Get rotation

	//Scale
		void setScale(vec3 scale);//!<Set scale
		vec3 getScale(void);//!<Get scale

	//Stop transformations
		void nullPosInc(void);//!<Null position transforms
		void nullRotInc(void);//!<Clear out rotation transforms
		void nullScaleInc(void);//!<Clear out scalation transforms
		void nullTransforms(void);//!<Clear out ALL transforms

	locator();
	virtual ~locator();
};

class pina;



//---------------------------
//!Basic 3D object
class obj: public locator
{
private:

	pina	*mesh;//!<3D mesh loaded from a collada file
	GLuint	texture;//!<texture id
	vec3x3	color;	//!<object color (each vertex)...

	int			soundID; 
	bool			lit;

public:

	virtual void	render(void);		//!<What to do when drawing

	void			setPina(pina* mesh);	//!<Change the mesh of the object
	void			setPina(std::string);	//!<Change the mesh of the object
	pina*			getPina(void);			//!<Retrieve mesh for modification/information

	void			setTexture(GLuint texID)	{ this->texture = texID; };
	GLuint			getTexture(void)			{ return this->texture; };

	void			setLighting(bool l)	{ lit = l; };
	bool			isLightOn(void)			{ return lit; };

	void			setSound(int s)	{ soundID = s; };
	int				getSound(void)	{ return soundID; };

	void			setColor(vec3x3 color);	//!<Set the color of the mesh
	void			setColor(vec3 color);
	vec3x3			getColor(void);




	obj(bool addtolist = true);
	obj(std::string);
	virtual ~obj();
};


void RemoveObject( obj* ob );

//---------------------------
//!Basic camera object
class camera: public locator
{
private:
	vec2 origMouse;					//!<Where the mouse was last
	float distance;					//!< distance from focal point

public:
	void setCamDistance( float x ) { distance = x; }
	float getCamDistance( ) { return distance; }

	void setOrigMouse(vec2 mouse)	{ this->origMouse = mouse; };
	vec2 getOrigMouse(void)			{ return this->origMouse; };

	void render();					//!<What to do when drawing

	camera();
	virtual ~camera();

	void init();
};



class Spline
{
private:
	//used for calculating the spline points
	float A[3];
	float B[3];
	float C[3];
	float D[3];
	float points[4][3]; //points on the spline

public:
	Spline();
	Spline(vec3 p_one, vec3 p_two, vec3 p_three, vec3 p_four );
	void operator=( Spline& sp );

	vec3 GetPoint(float t); //returns a point on the spline
};




#endif