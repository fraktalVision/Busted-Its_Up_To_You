#pragma once

#include "../../core/all.h"
#include "TextBox.h"


///////////////////////////////////////////////////////////////////////////////
/*
Text_Manager class
	-class for handling texture data for fonts and rendering
	-This class holds ont the texture data for each font and a pointer needs to be passed to every texture box that uses it.
*/
///////////////////////////////////////////////////////////////////////////////
class Text_Manager
{
public:
	Text_Manager( std::string fonttexture );
	~Text_Manager();

	void Render( TextBox &textbox );

	bool IsActive(){ return m_initialized; }

private:
	//!Renders a specific character at a position x/y with dimensions width and height
	void RenderLetter( float x, float y, float width, float height, char c );

	GLuint m_textureID; //texture id that holds the font in opengl
	bool m_initialized; //wether or not the font has been initialized

};