#include "../../core/all.h"

#include "textureImporter.h"

Texture_Manager* pTexture_man;

float SCREEN_X_RATIO = 1.0f;
float SCREEN_Y_RATIO = 1.0f;

#define TXT_ASSET_FILE_NAME	"../Busted/assets/texturelist.xtx"

#define MAX_CHAR_PER_LINE	4096

void checkGlError() {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		std::ostringstream oss;
		oss << "OpenGL error: " << e;
		throw std::runtime_error(oss.str());
	}
}

/**************************************************************************
  *  \brief Loads a texture specificed in the current fine name.
  *	 \author Jamie Gault

  *  \param filename The name of the image file.
  *  \param glindex possible texture ID to use to replace old texture data.
  *  \return returns the id of the texture loaded into mem.
**************************************************************************/
GLuint LoadTexture( std::string filename, GLuint glindex)
{
	//sdl texture data to be loaded
	SDL_Surface *TextureImage;

	GLuint txid = 0; //fail safe is zero

	TextureImage = IMG_Load(filename.c_str());
	char* TextureError = IMG_GetError();

	//if the file was able to be loaded
	if (TextureImage = IMG_Load( filename.c_str() )  )
	{
		//if the memory for the texture doesn't exist
		if (glindex == 0)
		{
			//allocate a texture
			glGenTextures (1, &txid);
		}
		else
		{
			//assign the old name for a texture
			txid = glindex;
		}

		if( txid == 0 )
			return 0;

		// bind to the given texture ID
		glBindTexture( GL_TEXTURE_2D, txid );

		/* Generate The Texture */
		/* ALL TEXTRUES MUST BE 32-bit PNG in RGBA format, with alpha channel!!! */

	    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w,
                  TextureImage->h, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, TextureImage->pixels );

		/* Linear Filtering */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		 /* Free up any memory we may have used */
		SDL_FreeSurface( TextureImage );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	else
	{
		std::cout << TextureError;
		throw std::runtime_error("Failed to load texture: " + filename);
	}

	return txid;
}

/***********************************
ViewOrtho

brief: change to orthoginal matrix
Author: Jamie Gault
***********************************/
void ViewOrtho()
{
	int dim[4];

	glGetIntegerv(GL_VIEWPORT, (GLint*)dim);

	glMatrixMode(GL_PROJECTION);								// Select Projection
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	glOrtho( 0, DATA("SCREEN_WIDTH"), DATA("SCREEN_HEIGHT"), 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);									// Select Modelview Matrix
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
}

/***********************************
ViewPerspective

return: setup for view perspective
Author: Jamie Gault
***********************************/
void ViewPerspective()
{
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview
	glPopMatrix();							// Pop The Matrix
}

/***********************************
RenderFullScreenImage

brief: Displays a full screen image
Author: Jamie Gault
***********************************/
void RenderFullScreenImage( int image, float alpha )
{
	float width = DATA("SCREEN_WIDTH");
	float height = DATA("SCREEN_HEIGHT");

	//if the image is a texture
	if( glIsTexture( image ) )
	{
		glBindTexture(GL_TEXTURE_2D, image);

		//render a square
		glBegin(GL_QUADS);
			glColor4f(1.0f, 1.0f, 1.0f, alpha);
			glTexCoord2f(0,0);
			glVertex2f(0, 0);

			glTexCoord2f(0,1);
			glVertex2f( 0, height);

			glTexCoord2f(1,1);
			glVertex2f(width, height);

			glTexCoord2f(1,0);
			glVertex2f(width, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);			// Bind To The Blur Texture
	}
}

/***********************************
RenderCoordScreenImage

brief: Displays an image to the dimensions specified
Author: Jamie Gault
***********************************/
void RenderCoordScreenImage( int image, float x0, float y0, float x1, float y1, float alpha/*, float r, float g, float b */)
{
	glBindTexture(GL_TEXTURE_2D, image);			// Bind To The Blur Texture

	glBegin(GL_QUADS);						// Begin Drawing Quads

		glColor4f(1.0f, 1.0f, 1.0f, alpha);

		glTexCoord2f(0,0);
		glVertex2f(x0, y0);

		glTexCoord2f(0,1);
		glVertex2f( x0, y1);				// Second Vertex

		glTexCoord2f(1,1);
		glVertex2f(x1,y1);				// Third Vertex

		glTexCoord2f(1,0);
		glVertex2f(x1,y0 );				// Fourth Vertex
	glEnd();			// Done Drawing Quads

	glBindTexture(GL_TEXTURE_2D, 0);			// Bind To The Blur Texture
}

/***********************************
RenderCoordScreenImage

brief: Displays an image to the dimensions specified
Author: Jamie Gault
***********************************/
void RenderCoordScreenImage( int image, float x0, float y0, float x1, float y1, float tx0, float ty0, float tx1, float ty1,float alpha )
{
	if( glIsTexture( image ) )
	{
		glBindTexture(GL_TEXTURE_2D, image);			// Bind Texture

		glBegin(GL_QUADS);						// Begin Drawing Quads
			glColor4f(1.0f, 1.0f, 1.0f, alpha);
			glTexCoord2f(tx0,ty0);
			glVertex2f(x0, y0);

			glTexCoord2f(tx0,ty1);
			glVertex2f( x0, y1);				// Second Vertex

			glTexCoord2f(tx1,ty1);
			glVertex2f(x1,y1);				// Third Vertex

			glTexCoord2f(tx1,ty0);
			glVertex2f(x1,y0 );				// Fourth Vertex
		glEnd();			// Done Drawing Quads

		glBindTexture(GL_TEXTURE_2D, 0);			// Bind To The Blur Texture
	}
}

/***********************************
RenderScreenGel

brief: Displays a gell over the image
Author: Jamie Gault
***********************************/
void RenderScreenGel( float r, float g, float b, float alpha )
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_QUADS);						// Begin Drawing Quads
		glColor4f(r, g, b, alpha);
		glVertex2f(0.0f, 0.0f);
		glVertex2f( 0.0f, 768.0f+2.0f );				// Second Vertex
		glVertex2f(1024.0f+2.0f, 768.0f+2.0f );				// Third Vertex
		glVertex2f(1024.0f+2.0f, 0.0f );				// Fourth Vertex
	glEnd();			// Done Drawing Quads
}

/***********************************
Texture_Manager Constructor

Author: Jamie Gault
***********************************/
std::string TextureFilePaths::GetPathAtLevel( int i )
{
	if(i < m_max_level)
		return m_filepaths[i];
	else if( m_max_level != 0)
		return m_filepaths[m_max_level-1];

	return std::string("NOT LOADED");
}

/***********************************
Texture_Manager Constructor

Author: Jamie Gault
***********************************/
Texture_Manager::Texture_Manager(): m_level(1), m_loaded( false )
{
	//if the file paths are loaded, load the textures
	if( LoadFileMap() )
	{
		LoadTextures();
		m_loaded = true;
	}
}

/***********************************
Texture_Manager GetTexture

Author: Jamie Gault
***********************************/
GLuint Texture_Manager::GetTexture( std::string id )
{
	//if all textures are loaded
	if( m_loaded )
	{
		TextureData& td = m_texture_map[id];
		//if the texture itself is loaded
		if( td.loaded )
			return td.id;
	}

	return 0;
}

/***********************************
Texture_Manager LoadFileMap

Author: Jamie Gault
***********************************/
bool Texture_Manager::LoadFileMap()
{
	std::ifstream myfile(TXT_ASSET_FILE_NAME);

	//if the file exists
	if(myfile.is_open())
	{
		//run through the entire file
		while (!myfile.eof())
		{
			char charPeekChar = static_cast<char>(myfile.peek());
			if(charPeekChar == '/')
			{
				//skip to the next line
				myfile.ignore(MAX_CHAR_PER_LINE, '\n');
			}
			else // line has stuff to read in
			{
				std::string strline;
				std::getline(myfile, strline);
				size_t firstQuote = 0;
				firstQuote = strline.find('"', firstQuote);

				// If empty line, the position returned is string::npos
				if(firstQuote  == std::string::npos)
				{
					continue; // go to next line
				}
				else // process line
				{
					// Get Sound Handle
					size_t secondQuote = strline.find('"', firstQuote + 1);
					std::string strHandle = strline.substr(firstQuote + 1, secondQuote - firstQuote - 1);

					// Get File Location
					firstQuote = strline.find('"', secondQuote + 1);
					secondQuote = strline.find('"', firstQuote + 1);
					std::string strFileLoc = strline.substr(firstQuote + 1, secondQuote - firstQuote - 1);

					TextureFilePaths* path = new TextureFilePaths();
					path->AddPath( strFileLoc );

					//find additional paths for textures
					do
					{
						firstQuote = strline.find('"', secondQuote + 1);

						if( firstQuote != -1 )
						{
							secondQuote = strline.find('"', firstQuote + 1);
							strFileLoc = strline.substr(firstQuote + 1, secondQuote - firstQuote - 1);
							path->AddPath( strFileLoc );
						}
					}while( firstQuote != -1);

					//assign the file path to the map with the string for the id
					m_file_map[strHandle] = path; //add the file path to the map
				}
			}
		}
		myfile.close();

		return true;
	}

	return false;
}

/***********************************
Texture_Manager LoadTextures

Author: Jamie Gault
***********************************/
void Texture_Manager::LoadTextures()
{
	for( std::map<std::string, TextureFilePaths*>::iterator mi = m_file_map.begin();
		mi != m_file_map.end(); ++mi )
	{
		AddTexture( (*mi).second->GetPathAtLevel(m_level), (*mi).first );
	}
}

/***********************************
Texture_Manager LoadTextures

Author: Jamie Gault
***********************************/
void Texture_Manager::ReLoadTextures()
{
	for( std::map<std::string, TextureFilePaths*>::iterator mi = m_file_map.begin();
		mi != m_file_map.end(); ++mi )
	{
		if( m_texture_map[(*mi).first].loaded)
			AddTexture( (*mi).second->GetPathAtLevel(m_level), (*mi).first );
	}
}

/***********************************
Texture_Manager Constructor

Author: Jamie Gault
***********************************/
void Texture_Manager::AddTexture(std::string filepath, std::string id)
{
	int glid = 0;

	//make sure the texture isn't already loaded
	if( m_texture_map.find( id ) != m_texture_map.end() )
	{
		if( m_texture_map[id].loaded)
		{
			glid = m_texture_map[id].id;
		}
	}

	TextureData data( LoadTexture( filepath, glid) );
	m_texture_map[id] = data;
}

/***********************************
Texture_Manager UnloadTexture

Author: Jamie Gault
***********************************/
void Texture_Manager::UnloadTexture( std::string id )
{
	if( glIsTexture(m_texture_map[id].id))
	{
		glDeleteTextures( 1, &(m_texture_map[id].id) );
		m_texture_map[id].loaded = false;
	}
}

/***********************************
Texture_Manager LoadTexture

Author: Jamie Gault
***********************************/
void Texture_Manager::SetupTexture( std::string id )
{
	if( m_texture_map.count( id) != 0 )
	{
		//if the texture isn't loaded
		if( ! m_texture_map[id].loaded )
		{
			AddTexture( m_file_map[id]->GetPathAtLevel(m_level), id );
		}
	}
}

/***********************************
Texture_Manager IsLoaded

Author: Jamie Gault
***********************************/
bool Texture_Manager::IsLoaded( std::string id )
{
	if( m_texture_map.count( id) != 0 )
	{
		//if the texture isn't loaded
		if( m_texture_map[id].loaded )
		{
			return true;
		}
	}
	return false;
}