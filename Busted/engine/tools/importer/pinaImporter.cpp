#include "pinaImporter.h"

pina::pina(std::string filename, bool dlist):	_element(NULL),
														_object(NULL),
														mesh(NULL),
														numInputs(0),
														text_id(0),
														dlist_on(dlist),
														cull_face(true)
{
	this->filename = filename;
	this->daeObj = new DAE();

	if(DATA("debug")) std::cout<< filename.c_str();
	this->load();
}
pina::~pina(void){};

void pina::load(void)
{
	daeInt error = 0;

	error = daeObj->load(filename.c_str());
	errorReport(error, "File failed to load.");

	error = daeObj->getDatabase()->getElement(&_element, 0, 0, COLLADA_ELEMENT_GEOMETRY); //first and only geometry in the file
	errorReport(error, "Element could not be retrieved.");

	createFrom(_element);
	fromCOLLADA();
}

void pina::errorReport(daeInt error, std::string reason)
{
	assert(error == DAE_OK);
}

domSource* pina::findSource(domMesh *mesh, std::string id)
{
	domSource_Array sourceArray = mesh->getSource_array();

	for( unsigned int i = 0; i < sourceArray.getCount(); ++i)
	{
		//if(DATA("debug")) { std::cout << sourceArray[i]->getId() << " " << id << std::endl; }

		if ( std::string(sourceArray[i]->getId()) == id )
			return sourceArray[i];
	}

	if(DATA("debug")) { std::cout << "No Source" << std::endl; }
	return NULL;
}

void pina::updateInputs(domTriangles *triangles, domVertices *vertices)
{
		//All the inputs that define the object in <mesh>...
		domInputLocalOffset_Array inputArray = triangles->getInput_array();
		numInputs = int(inputArray.getCount());
		if(DATA("debug"))if(DATA("debug")) { std::cout << BREAKLINE << " #~-" << std::endl; }

		for(int i = 0; i < numInputs; ++i)
		{
			//Type of input...
			std::string semantic = inputArray[i]->getSemantic();
			if(DATA("debug"))if(DATA("debug")) { std::cout << "Semantic: " << semantic << std::endl; }

			//Get the source arrays for each type of attribute...
			//Vertex positions
				if		(semantic == "POSITION")
				{
					position.source = findSource( mesh, inputArray[i]->getSource().getID() );
					position.offset = inputArray[i]->getOffset();
					position.count = position.source->getFloat_array()->getCount();
				}

			//Normal orientations
				else if	(semantic == "NORMAL")
				{
					normal.source = findSource( mesh, inputArray[i]->getSource().getID() );
					normal.offset = inputArray[i]->getOffset();
					normal.count = normal.source->getFloat_array()->getCount();
				}

			//Texture coordinates
				else if	(semantic == "TEXCOORD")
				{
					texcoord.source = findSource( mesh, inputArray[i]->getSource().getID() );
					texcoord.offset = inputArray[i]->getOffset();
					texcoord.count = texcoord.source->getFloat_array()->getCount();
					texcoord.stride = 2;
				}

			//If the one of the inputs is VERTEX, we must check *it* for the others
			else if	(semantic == "VERTEX")
			{
				//Set the position and normal offset to that of the VERTEX input
				position.offset = inputArray[i]->getOffset();

				//All the inputs that define the object in <vertex>...
				domInputLocal_Array inputs = vertices->getInput_array();

				for( unsigned int j = 0; j < inputs.getCount(); ++j)
				{
					//Type of input...
					std::string semantic = inputs[j]->getSemantic();

					//Get the source arrays for each type of attribute, again...
					//Vertex positions
						if		(semantic == "POSITION")
						{
							position.source = findSource( mesh, inputs[j]->getSource().getID() );
							position.count = position.source->getFloat_array()->getCount();
						}

					//Normal orientations
						else if	(semantic == "NORMAL")
						{
							normal.source = findSource( mesh, inputs[j]->getSource().getID() );
							normal.count = normal.source->getFloat_array()->getCount();
						}
				}//for <vertex> inputs
			}
		}//for <mesh> inputs

		if(position.source)
			assert(position.source->getFloat_array()->getCount() % position.stride == 0);
		if(normal.source)
			assert(normal.source->getFloat_array()->getCount() % normal.stride == 0);
		if(texcoord.source)
			assert(texcoord.source->getFloat_array()->getCount() % texcoord.stride == 0);
}

void pina::createFrom(daeElementRef element)
{
	//// Create my class to hold geometry information and initialize it as empty
	//if(_object)
	//{
	//	delete _object;
	//	_object = NULL;
	//}
	//if(_element)
	//{
	//	delete _element;
	//	_element = NULL;
	//}

	_object = new meshObj();
	_element = element;
}

void pina::fromCOLLADA(void)
{
	//Dump the existing models (if any)
	_object->mesh.clear();

	//Get things...
		// Get a properly typed pointer to the COLLADA domGeometry element
		mesh = ((domGeometry*)(domElement*)_element)->getMesh();
		domVertices *vertices = mesh->getVertices();

	//Set things...
		//For each <triangles>
		for( unsigned int i = 0; i < mesh->getTriangles_array().getCount(); ++i)
		{
			//Update the member pointers
			domTriangles *triangles = mesh->getTriangles_array()[i];
			this->updateInputs(triangles, vertices);
			domP *P = triangles->getP();

			//It *must* have vertices
			assert(position.source);

			//Prepare to create the mesh and allocate the number of polys, etc.
			int num_vert = position.count/position.stride;
			int num_text = 0;
			int num_norm = 0;
			int num_poly = triangles->getCount();

			if(normal.source)
				num_norm = normal.count/normal.stride;

			if(texcoord.source)
				num_text = texcoord.count/texcoord.stride;

			// This code translates from a COLLADA element to a user provided structure
			mesh_type *sub_mesh = NULL;
			try
			{
				sub_mesh = new mesh_type(num_vert, num_text, num_norm, num_poly);
			}
			catch(std::bad_alloc & x)
			{
				if(DATA("debug")) { std::cout << "bad_alloc: " << x.what() << std::endl; }
			}

			//Vertices
				// Loop over all the triangles
				for (int i = 0; i < sub_mesh->num_vert; ++i)
				{
					// Copy all the indices from the triangles into my structure
					sub_mesh->vertex[i].x = position.source->getFloat_array()->getValue()[position.stride*i];
					sub_mesh->vertex[i].y = position.source->getFloat_array()->getValue()[position.stride*i+1];
					sub_mesh->vertex[i].z = position.source->getFloat_array()->getValue()[position.stride*i+2];
				}
				// Tri list
				for(int i = 0; i < sub_mesh->num_poly; ++i)
				{
					sub_mesh->vertex_list[i].a	= P->getValue()[numInputs * (i    ) + numInputs*i*2 + position.offset];
					sub_mesh->vertex_list[i].b	= P->getValue()[numInputs * (i + 1) + numInputs*i*2 + position.offset];
					sub_mesh->vertex_list[i].c	= P->getValue()[numInputs * (i + 2) + numInputs*i*2 + position.offset];

					//if(DATA("debug")) { std::cout << "Vert: " << sub_mesh->vertex_list[i].a << " " << sub_mesh->vertex_list[i].b << " " << sub_mesh->vertex_list[i].c << std::endl; }
				}

			//Normals
				if(normal.source)
				{
					// Loop over all the triangles
					for (int i = 0; i < sub_mesh->num_norm; ++i)
					{
						// Copy all the indices from the triangles into my structure
						sub_mesh->normal[i].x	= normal.source->getFloat_array()->getValue()[normal.stride*i];
						sub_mesh->normal[i].y	= normal.source->getFloat_array()->getValue()[normal.stride*i+1];
						sub_mesh->normal[i].z	= normal.source->getFloat_array()->getValue()[normal.stride*i+2];
					}
					//Normal list
					for(int i = 0; i < sub_mesh->num_poly; ++i)
					{
						sub_mesh->normal_list[i].a	= P->getValue()[numInputs * (i    ) + numInputs*i*2 + normal.offset];
						sub_mesh->normal_list[i].b	= P->getValue()[numInputs * (i + 1) + numInputs*i*2 + normal.offset];
						sub_mesh->normal_list[i].c	= P->getValue()[numInputs * (i + 2) + numInputs*i*2 + normal.offset];
					}
				}
				else
				{
					sub_mesh->num_norm = sub_mesh->num_vert;
					// Loop over all the triangles
					for (int i = 0; i < sub_mesh->num_norm; ++i)
					{
						// Copy all the indices from the triangles into my structure
						sub_mesh->normal[i].x	= normal.stride*i;
						sub_mesh->normal[i].y	= normal.stride*i+1;
						sub_mesh->normal[i].z	= normal.stride*i+2;
					}
				}

			//Textcoords
				if(texcoord.source)
				{
					if(texcoord.stride == 3)
					{
						// Loop over all the triangles
						for(int i = 0; i < sub_mesh->num_text; ++i)
						{
							// Copy all the indices from the triangles into my structure
							sub_mesh->texcoord[i].x	= texcoord.source->getFloat_array()->getValue()[texcoord.stride*i];
							sub_mesh->texcoord[i].y	= texcoord.source->getFloat_array()->getValue()[texcoord.stride*i+1];
							sub_mesh->texcoord[i].z	= texcoord.source->getFloat_array()->getValue()[texcoord.stride*i+2];
						}
						//Textcoord list
						for(int i = 0; i < sub_mesh->num_poly; ++i)
						{
							sub_mesh->texcoord_list[i].a	= P->getValue()[numInputs * (i    ) + numInputs*i*2 + texcoord.offset];
							sub_mesh->texcoord_list[i].b	= P->getValue()[numInputs * (i + 1) + numInputs*i*2 + texcoord.offset];
							sub_mesh->texcoord_list[i].c	= P->getValue()[numInputs * (i + 2) + numInputs*i*2 + texcoord.offset];
						}
					}
					else
					{
						// Loop over all the triangles
						for(int i = 0; i < sub_mesh->num_text; ++i)
						{
							// Copy all the indices from the triangles into my structure
							sub_mesh->texcoord[i].x	= texcoord.source->getFloat_array()->getValue()[texcoord.stride*i];
							sub_mesh->texcoord[i].y	= texcoord.source->getFloat_array()->getValue()[texcoord.stride*i+1];
						}
						//Textcoord list
						for(int i = 0; i < sub_mesh->num_poly; ++i)
						{
							sub_mesh->texcoord_list[i].a	= P->getValue()[numInputs * (i    ) + numInputs*i*2 + texcoord.offset];
							sub_mesh->texcoord_list[i].b	= P->getValue()[numInputs * (i + 1) + numInputs*i*2 + texcoord.offset];
							sub_mesh->texcoord_list[i].c	= P->getValue()[numInputs * (i + 2) + numInputs*i*2 + texcoord.offset];
						}
					}
				}

				if(DATA("debug"))
					if(DATA("debug")) { std::cout
							/*<< "-~# Obj: "	<< triangles->getID()	<< " #~-\n"*/
							<< " Vert count:\t"	<< sub_mesh->num_vert	<< "\n"
							<< " Poly count:\t"	<< sub_mesh->num_poly	<< "\n"
							<< " Norm count:\t"	<< sub_mesh->num_norm	<< "\n"
							<< " Text count:\t"	<< sub_mesh->num_text	<< "\n"
							<< " P size:\t"		<< P->getValue().getCount() << "\n"

							<< std::endl; }
							//<< " P/inputs: " << P->getValue().getCount()/numInputs/3 //the 3 is for X, Y, Z (or A, B, C or S, T, R... whatever)

			//Put the mesh into the object
			_object->mesh.push_back(sub_mesh);
		}//for <triangles>

		//For generating a display list
	if(dlist_on)
	{
		_object->displaylist = glGenLists(1);

		if( _object->displaylist == 0 )
			return ;

		glNewList(_object->displaylist,GL_COMPILE);
		// Bind texture

			//Loop through each sub-mesh and draw it
			for(std::vector<mesh_type*>::iterator mi = _object->mesh.begin(); mi < _object->mesh.end(); ++mi)
			{
				mesh_type* subMesh = (*mi);	//An individual mesh

				glBegin(GL_TRIANGLES);
					//Loop through each polygon and draw it, adjust the normals, and set it's color
					for(int j=0; j < subMesh->num_poly; ++j)
					{
						//Vert 1
						glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].a ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].a ].y );
						glNormal3f	( subMesh->normal	[ subMesh->normal_list[j].a ].x, subMesh->normal	[ subMesh->normal_list	[j].a ].y, subMesh->normal[ subMesh->normal_list[j].a ].z );
						glVertex3f	( subMesh->vertex	[ subMesh->vertex_list[j].a ].x, subMesh->vertex	[ subMesh->vertex_list	[j].a ].y, subMesh->vertex[ subMesh->vertex_list[j].a ].z );

						//Vert 2
						glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].b ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].b ].y );
						glNormal3f	( subMesh->normal	[ subMesh->normal_list	[j].b ].x, subMesh->normal	[ subMesh->normal_list	[j].b ].y, subMesh->normal[ subMesh->normal_list[j].b ].z);
						glVertex3f	( subMesh->vertex	[ subMesh->vertex_list	[j].b ].x, subMesh->vertex	[ subMesh->vertex_list	[j].b ].y, subMesh->vertex[ subMesh->vertex_list[j].b ].z);

						//Vert 3
						glTexCoord2f( subMesh->texcoord	[ subMesh->texcoord_list[j].c ].x, 1.0f-subMesh->texcoord[ subMesh->texcoord_list[j].c ].y );
						glNormal3f	( subMesh->normal	[ subMesh->normal_list	[j].c ].x, subMesh->normal	[ subMesh->normal_list	[j].c ].y, subMesh->normal[ subMesh->normal_list[j].c ].z);
						glVertex3f	( subMesh->vertex	[ subMesh->vertex_list	[j].c ].x, subMesh->vertex	[ subMesh->vertex_list	[j].c ].y, subMesh->vertex[ subMesh->vertex_list[j].c ].z);
					} // for polygon

				glEnd();
			}// for submesh
		glEndList();
	}
	else
		_object->displaylist = 0;

	daeObj->clear();

	daeObj = NULL;
	_element = NULL;
	mesh = NULL;
}

void pina::toCOLLADA(void)
{
}

void pina::save(std::string outfile)
{
	toCOLLADA();
	daeObj->saveAs(outfile.c_str(), filename.c_str());
}