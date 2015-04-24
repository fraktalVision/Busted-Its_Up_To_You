#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include <gl/gl.h>
#include <iostream>
#include <dae.h>
#include <dae/daeDom.h>
#include <dom/domGeometry.h>
#include <dae/daeIntegrationObject.h>

#define MAX_POLYS 3000
#define MAX_VERTS MAX_POLYS * 3

//Forward declarations
	class thing;
	class locator;
	class obj;
	class camera;

//Void function pointer
typedef void (*voidFuncPtr)(void);

//Vectors for various dimensions of data
	//2D
	class vec2{
		public:
			GLfloat x, y;
			//vec2():x(0),y(0){}
	};
	//3D
	class vec3{
		public:
			GLfloat x, y, z;
			vec3():x(0),y(0),z(0){}
			vec3(float a, float b, float c)
			{
				this->x = a;
				this->y = b;
				this->z = c;
			}
			~vec3(){}
	};
	//4D
	class vec4{
		public:
			GLfloat x, y, z, w;
			vec4():x(0),y(0),z(0),w(0){}
			vec4(float a, float b, float c, float d)
			{
				this->x = a;
				this->y = b;
				this->z = c;
				this->w = d;
			}
	};
	//9D
	class vec3x3{
		public:
			vec3 a, b, c;
	};

//Vertex and polygon structs, separate from vectors in case something else is needed
	//Vertex/Normal
	class vertex_type : public vec3
	{
	public:
		vertex_type(){}
		~vertex_type(){}
	};

	//Vertex/Normal lists
	class list_type{
		public:
			GLuint a, b, c;
			list_type():a(0),b(0),c(0){}
			list_type(float a, float b, float c)
			{
				this->a = a;
				this->b = b;
				this->c = c;
			}
			~list_type(){}
	};

//3D mesh-type
class mesh_type
{
public:
	vertex_type	*vertex;		//vertex array
	vertex_type	*normal;		//normal array
	vertex_type	*texcoord;		//texture array
	list_type	*vertex_list;	//vertex draw-order
	list_type	*normal_list;	//normal draw-order
	list_type	*texcoord_list;	//texture draw-order
	int num_vert, num_norm, num_text, num_poly;

	mesh_type(void);
	mesh_type(int,int,int,int);
	virtual ~mesh_type(void);
};

//Actual 3D object
class meshObj
{
public:
	std::vector<mesh_type*>		mesh;			//vector of pointers to each sub-mesh
	GLuint						displaylist;	//Display list handler

	meshObj() : mesh(NULL), displaylist(0){}
	~meshObj(){}
};

class input
{
public:
	domSource		*source;
	unsigned short	offset;
	unsigned int	count;
	unsigned short	stride;

	input():	source(NULL), offset(0), count(0), stride(3){}
	~input(){}
};

#endif