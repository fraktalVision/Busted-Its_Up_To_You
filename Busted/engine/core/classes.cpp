#include "all.h"
#include "classes.h"
#include "../tools/importer/pinaImporter.h"
#include "../tools/datadrive/datadrive.h"

extern class DataDrive* DATA_MAN;	//pointer to the data manager (manages INI files)

//---------------------------
//Base-class of every "thing"
void thing::setVisible(bool myBool)		{ this->bVisible = myBool; }
bool thing::isVisible(void)				{ return this->bVisible; }

void thing::setSelected(bool myBool)	{ this->bSelected = myBool; }
bool thing::isSelected(void)			{ return this->bSelected; }

thing::thing()
{
	this->setName("Bob");
	this->setVisible(true);
	this->setSelected(false);
}
thing::~thing()
{
}

//---------------------------
//Locator

//Position
	//Set
	void locator::setPos(vec4 pos)
	{
		this->pos = pos;
	}
	void locator::setPos(vec3 pos)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->pos.z = pos.z;
	}
	void locator::setPos(vec2 pos)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
	}

	void locator::setPos(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	//Retrieve
	vec4 locator::getPos(void)		{ return this->pos; }
	float locator::getPosX(void)	{ return this->pos.x; }
	float locator::getPosY(void)	{ return this->pos.y; }
	float locator::getPosZ(void)	{ return this->pos.z; }

//Rotation
	//Set
	void locator::setRot(vec3 rot)
	{
		//Safety check
		if (rot.x > 359) rot.x = 0;
		if (rot.y > 359) rot.y = 0;
		if (rot.z > 359) rot.z = 0;

		this->rot = rot;
	}
	//Retrieve
	vec3 locator::getRot(void)		{ return this->rot; }

//Scale
	//Set
	void locator::setScale(vec3 scale)
	{
		this->scale = scale;
	}
	//Retrieve
	vec3 locator::getScale(void)	{ return this->scale; }

//Cancel transformations
	void locator::nullPosInc(void)
	{
		vec4 pos = vec4(0.0, 0.0, 0.0, 0.0);
		this->posInc = pos;
	}
	void locator::nullRotInc(void)
	{
		vec3 rot = vec3(0.0, 0.0, 0.0);
		this->rotInc = rot;
	}
	void locator::nullScaleInc(void)
	{
		vec3 scale = vec3(0.0, 0.0, 0.0);
		this->scaleInc = scale;
	}
	void locator::nullTransforms(void)
	{
		this->nullPosInc();
		this->nullRotInc();
		this->nullScaleInc();
	}

locator::locator(void)
{
	//Default instantiation
	vec4 position = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 rotation = vec3(0.0, 0.0, 0.0);
	vec3 scale = vec3(1.0, 1.0, 1.0);

	this->setPos(position);
	this->setRot(rotation);
	this->setScale(scale);
}

locator::~locator()
{
}

mesh_type::mesh_type(void) :	vertex(NULL),
								texcoord(NULL),
								vertex_list(NULL),
								normal_list(NULL),
								texcoord_list(NULL),
								num_vert(0),
								num_norm(0),
								num_text(0),
								num_poly(0)
{}

mesh_type::mesh_type(int v, int t, int n, int p)
{
	this->mesh_type::mesh_type();
	num_vert = v;
	num_text = t;
	num_norm = n;
	num_poly = p;

	try
	{
	vertex			= new vertex_type[num_vert];
	texcoord		= new vertex_type[num_text];
	normal			= new vertex_type[num_norm];

	vertex_list		= new list_type[num_poly];
	texcoord_list	= new list_type[num_poly];
	normal_list		= new list_type[num_poly];
	}
	catch(std::bad_alloc & x)
	{
		if(DATA("debug")) { std::cout << "bad_alloc: " << x.what() << std::endl; }
	}
}

mesh_type::~mesh_type(void)
{
	delete [] vertex;
	delete [] texcoord;
	delete [] vertex_list;
	delete [] normal_list;
	delete [] texcoord_list;

	vertex = NULL;
	texcoord = NULL;
	vertex_list = NULL;
	normal_list = NULL;
	texcoord_list = NULL;
}

//---------------------------
//Object
//Draw the object

//Set, retrieve, and modify the drawable mesh
	void obj::setPina(pina* mesh)			{ this->mesh = mesh; }
	void obj::setPina(std::string filename)	{
		if(this->mesh)
		{
			mesh->setFile(filename);
			mesh->load();
		}
		else
		{
			mesh = new pina(filename);
		}
	}
	pina* obj::getPina(void)				{ return this->mesh; }

void obj::render(void)
{
	meshObj* myMesh = NULL;
	if(this->mesh != NULL)
		myMesh = this->getPina()->getMesh();	//The object's mesh to-be-drawn

	if(this->isVisible() && myMesh != NULL)				//If the mesh is visible
	{
		//disable back face culling
		if( !this->getPina()->cull_face )
			glDisable( GL_CULL_FACE );

		glPushMatrix();
			//Global offset of the mesh away from the camera
			glTranslatef(0.0,0.0,MODEL_OFFSET);

			//Manipulate object based on it's stored transformation values...
				glTranslatef(this->getPos().x,this->getPos().y,this->getPos().z);

				glScalef(this->getScale().x,this->getScale().y,this->getScale().z);

				glRotatef(this->getRot().x,1.0,0.0,0.0);
				glRotatef(this->getRot().y,0.0,1.0,0.0);
				glRotatef(this->getRot().z,0.0,0.0,1.0);

			//Most objects will have been compiled as display lists, but for those who aren't, a poly-by-poly render is provided
			//if no display list
			if( myMesh->displaylist == 0)
			{
				// Bind texture
				if(this->texture)
				{
					glBindTexture(GL_TEXTURE_2D, getTexture());
					glColor3f	( 1.0f,1.0f,1.0f);
				}
				else if( this->getPina()->GetTextureID() )
				{
					glBindTexture(GL_TEXTURE_2D, this->getPina()->GetTextureID());
					glColor3f	( 1.0f,1.0f,1.0f);
				}

				//Loop through each sub-mesh and draw it
				for(std::vector<mesh_type*>::iterator mi = myMesh->mesh.begin(); mi < myMesh->mesh.end(); ++mi)
				{
					mesh_type* subMesh = (*mi);	//An individual mesh

					glBegin(GL_TRIANGLES);

						//Loop through each polygon and draw it, adjust the normals, and set it's color
						for(int j=0; j < subMesh->num_poly; ++j)
						{
							//Vert 1
							if(texture||this->getPina()->GetTextureID())
								glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].a ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].a ].y );
							else
								glColor3f	( this->getColor().a.x,this->getColor().a.y,this->getColor().a.z);

							glNormal3f	( subMesh->normal	[ subMesh->normal_list[j].a ].x, subMesh->normal	[ subMesh->normal_list	[j].a ].y, subMesh->normal[ subMesh->normal_list[j].a ].z );
							glVertex3f	( subMesh->vertex	[ subMesh->vertex_list[j].a ].x, subMesh->vertex	[ subMesh->vertex_list	[j].a ].y, subMesh->vertex[ subMesh->vertex_list[j].a ].z );

							//Vert 2

							if(texture||this->getPina()->GetTextureID())
								glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].b ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].b ].y );
							else
								glColor3f	( this->getColor().b.x,this->getColor().b.y,this->getColor().b.z);
							glNormal3f	( subMesh->normal	[ subMesh->normal_list	[j].b ].x, subMesh->normal	[ subMesh->normal_list	[j].b ].y, subMesh->normal[ subMesh->normal_list[j].b ].z);
							glVertex3f	( subMesh->vertex	[ subMesh->vertex_list	[j].b ].x, subMesh->vertex	[ subMesh->vertex_list	[j].b ].y, subMesh->vertex[ subMesh->vertex_list[j].b ].z);

							//Vert 3

							if(texture||this->getPina()->GetTextureID())
								glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].c ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].c ].y );
							else
								glColor3f	( this->getColor().c.x,this->getColor().c.y,this->getColor().c.z);

							glNormal3f	( subMesh->normal	[ subMesh->normal_list	[j].c ].x, subMesh->normal	[ subMesh->normal_list	[j].c ].y, subMesh->normal[ subMesh->normal_list[j].c ].z);
							glVertex3f	( subMesh->vertex	[ subMesh->vertex_list	[j].c ].x, subMesh->vertex	[ subMesh->vertex_list	[j].c ].y, subMesh->vertex[ subMesh->vertex_list[j].c ].z);
						} // for polygon

					glEnd();
				}// for submesh
				//Stop texture
				glBindTexture( GL_TEXTURE_2D, 0 );
			}// if no list
			else
			{
				//bind the correct texture
				if(this->texture)
				{
					glBindTexture(GL_TEXTURE_2D, getTexture());
					glColor3f	( 1.0f,1.0f,1.0f);
				}
				else if( this->getPina()->GetTextureID() )
				{
					glBindTexture(GL_TEXTURE_2D, this->getPina()->GetTextureID());
					glColor3f	( 1.0f,1.0f,1.0f);
				}

				//Draw the object by it's handler
				glCallList(myMesh->displaylist);

				glBindTexture( GL_TEXTURE_2D, 0 );
			}

		glPopMatrix();

		if( ! this->getPina()->cull_face )
			glEnable( GL_CULL_FACE );
	} // if is visible
}

//Set and retrieve the color of the object
	void obj::setColor(vec3x3 color)		{ this->color = color; };
	void obj::setColor(vec3 color)			{ this->color.a = color; this->color.b = color; this->color.c = color; };
	vec3x3 obj::getColor(void)				{ return this->color; };

	obj::obj(bool addtolist) :	mesh(NULL),
					texture(0),
					soundID(-1),
					lit(true)
{
	//Default color
	vec3x3 color =	{
						vec3(1.0,0.5,0.5),
						vec3(0.5,1.0,0.5),
						vec3(0.5,0.5,1.0)
					};
	this->setColor(color);

	//If I'm moving, I won't anymore
	this->nullTransforms();

	if( addtolist )
	{
		//!!!!!!!!!!!!!!!!!!!!
		//MOST IMPORTANT STEP:
		//Put myself onto the draw-stack
		objStack.push_back(this);
	}

	//Notify my existence on the command line
	if(DATA("debug")) { std::cout << "Created object" << std::endl; }
	if(DATA("debug")) { std::cout << "Stack size: " << objStack.size() << " " << this->posInc.x << std::endl; }
}
obj::obj(std::string filename)
{
	//The this-> is absolutely necessary, obj::obj() simply won't work without it
	this->obj::obj();
	this->setPina(filename);
}
obj::~obj(){}

void RemoveObject( obj* ob )
{
	for( std::vector<obj*>::iterator i = objStack.begin(); i != objStack.end(); ++i )
	{
		if( (*i) == ob )
		{
			objStack.erase( i );
			return;
		}
	}
}

//---------------------------
//Camera
//Modify view transforms based on where the camera is located
void camera::render(void)
{
	glTranslatef( 0.0f, 0.0f,  -distance);

	glRotatef(this->getRot().x , 1.0, 0.0, 0.0);
	glRotatef(this->getRot().y , 0.0, 1.0, 0.0);
	glRotatef(this->getRot().z , 0.0, 0.0, 1.0);

	glTranslatef( -this->getPos().x, -this->getPos().y, -this->getPos().z );
};

camera::camera()
{
}

camera::~camera()
{
}

void camera::init()
{
	//Origin of the mouse (default: (0,0))
	//Used for camera manipulations

	distance = DATA("CAMERA_DISTANCE");//100.0f;
	vec2 mouse = {0, 0};
	this->setOrigMouse(mouse);

	//Default positioning
	vec3 rot = vec3(DATA("CAMERA_DEF_ANGLE"), 0, 0);
	vec4 pos = vec4(0, 0, /*15*/0, 0);
	this->setPos(pos);
	this->setRot(rot);
}

Spline::Spline()
{
	for(int i = 0; i < 3 ; ++i )
	{
		A[i] = 0.0f;
		B[i] = 0.0f;
		C[i] = 0.0f;
		D[i] = 0.0f;
	}

	//set all the points to zero
	for( int p = 0; p < 4; ++p )
	{
		for( int q = 0; q < 3; ++q )
		{
			points[p][q] = 0.0f;
		}
	}
}

Spline::Spline(vec3 p_one, vec3 p_two, vec3 p_three, vec3 p_four )
{
	//copy over points
	points[0][0] = p_one.x;
	points[0][1] = p_one.y;
	points[0][2] = p_one.z;

	points[1][0] = p_two.x;
	points[1][1] = p_two.y;
	points[1][2] = p_two.z;

	points[2][0] = p_three.x;
	points[2][1] = p_three.y;
	points[2][2] = p_three.z;

	points[3][0] = p_four.x;
	points[3][1] = p_four.y;
	points[3][2] = p_four.z;

	//determine spline constants
	for(int i = 0; i < 3 ; ++i )
	{
		A[i] = points[3][i] - 3.0f * points[2][i] + 3.0f * points[1][i] - points[0][i];
		B[i] = 3.0f * points[2][i] - 6.0f * points[1][i] + 3.0f * points[0][i];
		C[i] = 3.0f * points[1][i] - 3.0f * points[0][i];
		D[i] = points[0][i];
	}
}

void Spline::operator=( Spline& sp )
{
	//copy constants
	for(int i = 0; i < 3 ; ++i )
	{
		A[i] = sp.A[i];
		B[i] = sp.B[i];
		C[i] = sp.C[i];
		D[i] = sp.D[i];
	}

	//copy all points
	for( int p = 0; p < 4; ++p )
	{
		for( int q = 0; q < 3; ++q )
		{
			points[p][q] = sp.points[p][q];
		}
	}
}

vec3 Spline::GetPoint(float t)
{
	vec3 result;

	result.x = A[0] * t * t * t + B[0] * t * t + C[0] * t + D[0];
	result.y = A[1] * t * t * t + B[1] * t * t + C[1] * t + D[1];
	result.z = A[2] * t * t * t + B[2] * t * t + C[2] * t + D[2];

	return result;
}