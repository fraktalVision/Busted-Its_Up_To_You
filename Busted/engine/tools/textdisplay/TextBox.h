#pragma once

#include "../textureload/textureImporter.h"

extern const float firstletteroffset;
extern Texture_Manager* pTexture_man;
extern float GetLetterSpace( char c );

class Text_Manager;

///////////////////////////////////////////////////////////////////////////////
/*!
TextBox class
	-class for displaying text within a certain box range
	-Contains a pointer to the font that it's supposed to Use

*/
///////////////////////////////////////////////////////////////////////////////
class TextBox
{
public:
	TextBox(); //default

	TextBox(Text_Manager* t, std::string & s, float x, float y, float maxwidth,
				float gap = 10.0f, float letWd = 30.0f, float letHt = 30.0f);
	//window will adjust itself to the dimensions needed for the text

	TextBox( Text_Manager* t, float x, float y, float width, float height,
				float gap = 10.0f, float letWd = 30.0f, float letHt = 30.0f );

	TextBox(TextBox& textbox);

	TextBox& operator=( TextBox& textbox );

	void SetString( std::string s, bool recalculateSize = false ); //!<adds a string to the given texture box
	void ClearString();//!< Remove a string from it
	std::string GetString(); //!< Get the string that it has
	void AppendString( std::string s );//!< add a string to the end of the current string in the text box

	float GetPosX();
	float GetPosY();
	float GetBoxWidth();
	float GetBoxHeight();
	float GetLetterWidth();
	float GetLetterHeight();
	float GetGap();//!< Get the space between letters
	bool hasCursor();
	float GetCursorAlpha();//!< Returns the alpha intensity of the cursor mimicing the blinking
	GLuint GetBackGround();	//!< Get the texture ID of the background
	Text_Manager* GetFontPtr();

	void SetVisible( bool v ) { m_visible = v; }
	void SetPosX(float x){ m_pos[0] = x; }
	void SetPosY(float y){ m_pos[1] = y; }
	void SetBoxWidth(float w){ m_box_dim[0] = w; }
	void SetBoxHeight(float h){ m_box_dim[1] = h; }
	void SetLetterWidth(float w){ m_letter_dim[0] = w; }
	void SetLetterHeight(float h){ m_letter_dim[1] = h; }

	void EnableCursor(bool enable) { m_cursor = enable; }

	void SetBackground(GLuint bg); //!< Assign a new texture to use as the background

	void Render();

	float GetColourComp(unsigned int i);

	void SetColour(float r, float g, float b, float a);

	float GetBGOffset( int lr, int xy ){ return m_bgoffset[lr][xy] ; }
	void  SetBGOffset( float value, int lr, int xy ){ if( (lr == 0 || lr == 1)&&(xy == 0 || xy == 1) )m_bgoffset[lr][xy] = value ; }

	void Update();

private:

	std::string m_displaystring;
	float m_pos[2]; //x and y screen positions
	float m_letter_dim[2]; //letter width and height
	float m_box_dim[2]; //box width and height
	float m_bgoffset[2][2]; //values to push the background and border out by
	float m_gapspace; //space between letters
	float m_maxwidth; //maximum width
	bool m_cursor; //show blinking cursor
	bool m_visible;
	Text_Manager* m_tm;
	GLuint m_background; //background texture for the scene

	float m_colour[4]; //colour of the font
};
