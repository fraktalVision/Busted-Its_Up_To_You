/*
	\file textureImporter.h
	Created by: Jamie Gault
	Created on 4/25/07

*/

//#pragma once

#ifndef _TEXTURE_IMP_H_
#define _TEXTURE_IMP_H_

#include <windows.h>
#include <iostream>
#include <gl/gl.h>
#include <gl/glu.h>
#include <vector>
#include <string>
#include <map>

#define GL_ERROR_CHECK   {GLenum glErr = glGetError(); while (glErr != GL_NO_ERROR){ printf("glError  %s\n", gluErrorString(glErr));  glErr = glGetError();  }}






GLuint LoadTexture( std::string filename, GLuint glindex = 0);




//load the orthogonal matrix
void ViewOrtho();

//unload to the perspective matrix
void ViewPerspective();

//display an image that takes up the entire screen
void RenderFullScreenImage( int image, float alpha = 1.0f );

//display an image to the dimensions specified
void RenderCoordScreenImage( int image, 
							float x0, 
							float y0, 
							float x1, 
							float y1, 
							float alpha = 1.0f);

//be able to specify texture coordinates
void RenderCoordScreenImage( int image, 
							float x0, 
							float y0, 
							float x1, 
							float y1, 
							float tx0, 
							float ty0, 
							float tx1,
							float ty1,
							float alpha = 1.0f );

//render a gell over the screen
void RenderScreenGel( float r, float g, float b, float alpha ); 


/*
! A Texture node class used by the Texture_Manager class.

*Class for holding details about individual textures.
*/
class TextureData
{
public:
	
    /**
      *Default Constructor for the function  */
	TextureData(): loaded(false), id(0){}

	/*Constructor that assigns a texture ID  
    * \param i Texture ID assigned by the graphics card    */
	TextureData( GLuint i):loaded(true), id(i) {} 

	/**\brief Deep copies one texture data to another 
	*	\param t Texture to be copied*/
	TextureData& operator=( TextureData& t ) { loaded = t.loaded; id = t.id; return *this;}

	
    /**
      *Bool saying whether the texture is actually loaded in. */
	bool loaded;

	/**
      *Texture ID given by the graphics card.*/
	GLuint id;
};



/*! A Texture file path node class used by the Texture_Manager class.
*
* Class for handling texture file locations for different file textures.
*/
class TextureFilePaths
{
public:
	TextureFilePaths(): m_max_level(0), m_should_load(true){};

	void AddPath(std::string& s ){m_filepaths.push_back(s); ++m_max_level;} //adds another string to be registered
	std::string GetPathAtLevel( int i );//given the texture level, determine whether to load

	bool m_should_load; //member variable stating whether the texture should be loaded initially.

private:
	int m_max_level; //maximum level that the texture can reach
	std::vector<std::string> m_filepaths; //vector of all the file paths
};




/*
*	 class for managing global texture objects
*/
class Texture_Manager
{
public:
	Texture_Manager();
	
	GLuint GetTexture( std::string id );

	void UnloadTexture( std::string id );
	void SetupTexture( std::string id ); //this gives the call to search for a file name and load a file
	bool IsLoaded( std::string id );

	void SetLevel(int i){ if( m_level != i ){m_level = i; ReLoadTextures();} } //reload all textures based on the level
	int	 GetLevel() { return m_level; } //returns the value for the current texture level.

private:
	int m_level;

	bool LoadFileMap();
	void LoadTextures();
	void ReLoadTextures();
	void AddTexture(std::string filepath, std::string id);


	std::map<std::string, TextureData> m_texture_map;
	std::map<std::string, TextureFilePaths*> m_file_map;
	bool m_loaded;
};




#endif