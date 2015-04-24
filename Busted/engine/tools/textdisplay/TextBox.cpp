#include "TextBox.h"
#include "TextManager.h"
#include "../textureload/textureImporter.h"
#include "../datadrive/datadrive.h"

const float firstletteroffset =  0.080f;//0.05f

/***********************************
TextBox Default Constructor
	
brief: creates a text box object and loads data
Author: Jamie Gault
***********************************/
TextBox::TextBox()
{
	m_gapspace = 0.0f;

	m_tm = NULL;
	m_pos[0] = 0.0f;
	m_pos[1] = 0.0f;
	m_box_dim[0] = 0.0f;
	m_box_dim[1] = 0.0f;
	m_letter_dim[0] = 0.0f;
	m_letter_dim[1] = 0.0f;
	m_maxwidth = 40.0f;

	m_cursor = false;
	m_visible = true;
	m_background = pTexture_man->GetTexture("TID_DEF_TXT");

	SetColour(1.0f, 1.0f, 1.0f, 1.0f);

	m_bgoffset[0][0] = 0.0f;
	m_bgoffset[0][1] = 0.0f;
	m_bgoffset[1][0] = 0.0f;
	m_bgoffset[1][1] = 0.0f;
}



/***********************************
TextBox Constructor
	
brief: creates a text box object based on 
Author: Jamie Gault
***********************************/
TextBox::TextBox(Text_Manager* t, std::string & s, float x, float y, float maxwidth, float gap , float letWd , float letHt)
{
	m_cursor = false;
	m_visible = true; 

	m_background = pTexture_man->GetTexture("TID_DEF_TXT");
	m_gapspace = gap;

	m_tm = t;

	//SetString(s);
	SetColour(1.0f, 1.0f, 1.0f, 1.0f);

	m_pos[0] = x;
	m_pos[1] = y;
	m_letter_dim[0] = letWd;
	m_letter_dim[1] = letHt;
	m_maxwidth = maxwidth;

	m_bgoffset[0][0] = -10.0f;
	m_bgoffset[0][1] = -10.0f;
	m_bgoffset[1][0] = 20.0f;
	m_bgoffset[1][1] = 10.0f;

	SetString(s, true);
}


/***********************************
TextBox Constructor
	
brief: creates a text box object and loads data
Author: Jamie Gault
***********************************/
TextBox::TextBox( Text_Manager* t, float x, float y, float boxwidth, float boxheight, float gap, float letWd, float letHt): m_gapspace(gap)
{
	m_tm = t;
	m_pos[0] = x;
	m_pos[1] = y;
	m_box_dim[0] = boxwidth;
	m_box_dim[1] = boxheight;
	m_letter_dim[0] = letWd;
	m_letter_dim[1] = letHt;
	m_maxwidth = boxwidth;

	m_cursor = false;
	m_visible = true;
	m_background = pTexture_man->GetTexture("TID_DEF_TXT");
	SetColour(1.0f, 1.0f, 1.0f, 1.0f);

	m_bgoffset[0][0] = -10.0f;
	m_bgoffset[0][1] = -10.0f;
	m_bgoffset[1][0] = 20.0f;
	m_bgoffset[1][1] = 10.0f;
}


/***********************************
TextBox Copy Constructor
	
brief: creates a text box object and loads data
Author: Jamie Gault
***********************************/
TextBox::TextBox(TextBox& textbox)
{
	m_tm = textbox.GetFontPtr();
	m_pos[0] = textbox.GetPosX();
	m_pos[1] = textbox.GetPosY();
	m_box_dim[0] = textbox.GetBoxWidth();
	m_box_dim[1] = textbox.GetBoxHeight();
	m_letter_dim[0] = textbox.GetLetterWidth();
	m_letter_dim[1] = textbox.GetLetterHeight();
	m_maxwidth = textbox.m_maxwidth;
	m_gapspace = textbox.GetGap();

	SetString( textbox.GetString() );

	m_cursor = textbox.m_cursor;
	m_visible = true;
	m_background = textbox.GetBackGround();

	SetColour(textbox.m_colour[0], textbox.m_colour[1], textbox.m_colour[2], textbox.m_colour[3]);

	m_bgoffset[0][0] = -10.0f;
	m_bgoffset[0][1] = -10.0f;
	m_bgoffset[1][0] = 20.0f;
	m_bgoffset[1][1] = 10.0f;

}



/***********************************
TextBox operator= 
	
brief: assignment operator
Author: Jamie Gault
***********************************/
TextBox& TextBox::operator=( TextBox& textbox )
{
	m_tm = textbox.GetFontPtr();
	m_pos[0] = textbox.GetPosX();
	m_pos[1] = textbox.GetPosY();
	m_box_dim[0] = textbox.GetBoxWidth();
	m_box_dim[1] = textbox.GetBoxHeight();
	m_letter_dim[0] = textbox.GetLetterWidth();
	m_letter_dim[1] = textbox.GetLetterHeight();
	m_gapspace = textbox.GetGap();
	m_maxwidth = textbox.m_maxwidth;

	SetString( textbox.GetString() );

	m_cursor = textbox.m_cursor;
	m_visible = true;
	m_background = textbox.GetBackGround();

	SetColour(textbox.m_colour[0], textbox.m_colour[1], textbox.m_colour[2], textbox.m_colour[3]);


	m_bgoffset[0][0] = textbox.m_bgoffset[0][0];
	m_bgoffset[0][1] = textbox.m_bgoffset[0][1];
	m_bgoffset[1][0] = textbox.m_bgoffset[1][0];
	m_bgoffset[1][1] = textbox.m_bgoffset[1][1];

	return *this;
}


/***********************************
TextBox SetString
	
brief: assign the new string to the old string
Author: Jamie Gault
***********************************/
void TextBox::SetString( std::string s, bool recalculateSize)
{
	m_displaystring = s;
	if (recalculateSize) {
		Update();
	}
}

/***********************************
TextBox SetString
	
brief: clears the string of data
Author: Jamie Gault
***********************************/
void TextBox::ClearString()
{
	m_displaystring.clear();
}

/***********************************
TextBox GetString
	
brief: returns the string being stored in the text box
Author: Jamie Gault
***********************************/
std::string TextBox::GetString()
{
	return m_displaystring;
}



/***********************************
TextBox AppendString
	
brief: adds s to the end of the string in the box
Author: Jamie Gault
***********************************/
void TextBox::AppendString( std::string s )
{
	m_displaystring.append( s );
}


/***********************************
TextBox SetBackground
	
brief: assign a background texture
Author: Jamie Gault
***********************************/
void TextBox::SetBackground(GLuint bg)
{
	if( glIsTexture( bg ) || bg == 0 )
		m_background = bg;
}



/***********************************
TextBox GetPosX
	
Author: Jamie Gault
***********************************/
float TextBox::GetPosX()
{ return m_pos[0]; }


/***********************************
TextBox GetPosY

Author: Jamie Gault
***********************************/
float TextBox::GetPosY()
{ return m_pos[1]; }


/***********************************
TextBox GetBoxWidth

Author: Jamie Gault
***********************************/
float TextBox::GetBoxWidth()
{ return m_box_dim[0]; }


/***********************************
TextBox GetBoxHeight

Author: Jamie Gault
***********************************/
float TextBox::GetBoxHeight()
{ return m_box_dim[1]; }


/***********************************
TextBox GetLetterWidth
	
Author: Jamie Gault
***********************************/
float TextBox::GetLetterWidth()
{ return m_letter_dim[0]; }


/***********************************
TextBox GetLetterHeight
	
Author: Jamie Gault
***********************************/
float TextBox::GetLetterHeight()
{ return m_letter_dim[1]; }


/***********************************
TextBox GetGap
	
Author: Jamie Gault
***********************************/
float TextBox::GetGap()
{ return m_gapspace;} 

bool TextBox::hasCursor()
{
	return m_cursor;
}

float TextBox::GetCursorAlpha()
{
	static const int blink_tick_count = 500;
	int now = GetTickCount();
	return now % (blink_tick_count << 1) > blink_tick_count ? 1.0f : 0.f;
}

/***********************************
TextBox GetBackGround
	
Author: Jamie Gault
***********************************/
GLuint TextBox::GetBackGround()
{ return m_background; }


/***********************************
TextBox GetFontPtr
	
Author: Jamie Gault
***********************************/
Text_Manager* TextBox::GetFontPtr() 
{ return m_tm; }


/***********************************
TextBox AppendString
	
brief: draws text block to window
Author: Jamie Gault
***********************************/
void TextBox::Render()
{

	if( m_visible )
		m_tm->Render( *this );
}


/***********************************
TextBox GetColourComp
	
brief: draws text block to window
Author: Jamie Gault
***********************************/
float TextBox::GetColourComp(unsigned int i)
{ 
	if( i < 4 ) 
		return m_colour[ i ]; 
	return 0.0f;
}

/***********************************
TextBox SetColour
	
brief: draws text block to window
Author: Jamie Gault
***********************************/
void TextBox::SetColour(float r, float g, float b, float a = 1.0f)
{
	m_colour[0] = r; m_colour[1] = g; m_colour[2] = b; m_colour[3] = a; 
}

/***********************************
TextBox Update
	
brief: recalculate box size
Author: Vigen Issahhanjan
***********************************/
void TextBox::Update()
{
	//break the words up into a vector of strings
	std::istringstream stream(GetString());
	std::vector<std::string> word_vec ( (std::istream_iterator<std::string>(stream)), std::istream_iterator<std::string>());

	std::vector<std::string>::iterator word_it = word_vec.begin(); 

	m_box_dim[0] = 0.0f;
	m_box_dim[1] = 0.0f;
	//while not at the end of the string
	while( word_it != word_vec.end())
	{
		float widthiter = 0.0f;

		//go to the next row
		m_box_dim[1] += DATA("LETTER_HT_SCALE") * GetLetterHeight();

		
		bool wentin = false;

		float wordlength = 0.0f;

		
		float prevletlen;
		wordlength += GetLetterSpace(*(*word_it).begin()) - firstletteroffset;

		//iterate through each letter, spacing based on letter width
		for(std::string::iterator let_it = (*word_it).begin(); 
						let_it != (*word_it).end() ; ++let_it )
		{
			prevletlen = GetLetterSpace(*let_it); //space between previous letter
			wordlength += DATA("LETTER_WID_SCALE")*GetLetterWidth()*(1.0f - (prevletlen + GetLetterSpace(*let_it)));
		}
		wordlength += DATA("LETTER_WID_SCALE")*GetLetterWidth()*(1.0f);

		//while each word has not been processed
		while( word_it != word_vec.end() &&
			 widthiter + wordlength < m_maxwidth)
		{
	
			wentin = true;

			widthiter += wordlength;
			widthiter += GetGap(); // add for a space

			wordlength = GetLetterSpace(*(*word_it).begin()) - firstletteroffset;

			++word_it;
			if( word_it != word_vec.end() )
			{
				//iterate through each letter, spacing based on letter width
				for(std::string::iterator let_it = (*word_it).begin(); 
								let_it != (*word_it).end() ; ++let_it )
				{
					prevletlen = GetLetterSpace(*let_it); //space between previous letter
					wordlength += DATA("LETTER_WID_SCALE")*GetLetterWidth()*(1.0f - (prevletlen+GetLetterSpace(*let_it)));
				}

				//wordlength += DATA("LETTER_WID_SCALE")*GetLetterWidth()*(1.0f);
			}
		}

		if( m_box_dim[0] < widthiter)
		{
			if( widthiter > m_maxwidth )
				m_box_dim[0] = m_maxwidth;
			else
				m_box_dim[0] = widthiter + 0;
		}

		//stop infinite loop
		if( !wentin )
		{
			m_box_dim[0] = m_maxwidth;
			break;
		}
	}
}




