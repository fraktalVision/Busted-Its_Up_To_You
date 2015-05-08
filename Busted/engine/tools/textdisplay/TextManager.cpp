#include "TextBox.h"
#include "TextManager.h"
#include "../textureload/textureImporter.h"
#include "../datadrive/datadrive.h"

/***********************************
Text_Manager Constructor

Author: Jamie Gault
***********************************/
Text_Manager::Text_Manager( std::string fonttexture )
{
	m_textureID = LoadTexture( const_cast<char*>(fonttexture.c_str()) );
	m_initialized = ( m_textureID == 0 ) ? false : true;
}

/***********************************
Text_Manager Deconstructor

Author: Jamie Gault
***********************************/
Text_Manager::~Text_Manager()
{
	if( m_initialized)
	{
		glDeleteTextures( 1, &m_textureID);
	}
}

static float r = 1.0f;
/***********************************
Text_Manager Render

brief: renders the string stored in the text box using the parameters of the text box
Author: Jamie Gault
***********************************/
void Text_Manager::Render( TextBox &textbox )
{
	r = 1.0f;
	if( m_initialized )
	{
		float x0 = textbox.GetPosX() + textbox.GetBGOffset(0,0);
		float x1 = textbox.GetPosX() + textbox.GetBoxWidth()+ textbox.GetBGOffset(1,0);
		float y0 = textbox.GetPosY() + textbox.GetBGOffset(0,1);
		float y1 = textbox.GetPosY() + textbox.GetBoxHeight()+ textbox.GetBGOffset(1,1);

		//if there is a background
		if( textbox.GetBackGround() )
		{
			glBindTexture(GL_TEXTURE_2D, textbox.GetBackGround());
			glBegin(GL_QUADS);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glTexCoord2f(0,0);
				glVertex2f(x0,y0);

				glTexCoord2f(0,1);
				glVertex2f(x0,y1);

				glTexCoord2f(1,1);
				glVertex2f(x1,y1);

				glTexCoord2f(1,0);
				glVertex2f(x1,y0 );
			glEnd();
		}

		// Bind text texture
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		glColor4f(textbox.GetColourComp(0), textbox.GetColourComp(1), textbox.GetColourComp(2), textbox.GetColourComp(3));

		//break the words up into a vector of strings
		std::istringstream stream(textbox.GetString());
		std::vector<std::string> word_vec ( (std::istream_iterator<std::string>(stream)), std::istream_iterator<std::string>());

		std::vector<std::string>::iterator word_it = word_vec.begin();

		glBegin(GL_QUADS);

		//while not at the end of the string
		float heightiter = 0.0f;
		float widthiter = 0.0f;
		while( heightiter <  textbox.GetBoxHeight() && word_it != word_vec.end())
		{
			float wordlength = 0.0f;
			float prevletlen;
			widthiter = GetLetterSpace(*(*word_it).begin()) - firstletteroffset;

			//iterate through each letter, spacing based on letter width
			for(std::string::iterator let_it = (*word_it).begin();
							let_it != (*word_it).end() ; ++let_it )
			{
				prevletlen = GetLetterSpace(*let_it); //space between previous letter
				wordlength += DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*(1.0f - (prevletlen+GetLetterSpace(*let_it)));
			}
			wordlength += DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*(1.0f);

			//while each word has not been processed
			while( word_it != word_vec.end() &&
				 widthiter + wordlength < textbox.GetBoxWidth())
			{
				widthiter += GetLetterSpace(*(*word_it).begin()) - firstletteroffset;
				//iterate through each letter, spacing based on letter width
				for(std::string::iterator let_it = (*word_it).begin();
								let_it != (*word_it).end() ; ++let_it)
				{
					prevletlen = GetLetterSpace(*let_it); //space between previous letter
					RenderLetter( textbox.GetPosX() + widthiter,
								textbox.GetPosY() + heightiter,
								textbox.GetLetterWidth(), textbox.GetLetterHeight(), *let_it);
					widthiter += DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*(1.0f - (prevletlen+GetLetterSpace(*let_it)));
				}
				//widthiter += DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*(1.0f);
				widthiter += textbox.GetGap(); // add for a space

				wordlength = 0.0f;

				++word_it;

				if( word_it != word_vec.end() )
				{
					wordlength += GetLetterSpace(*(*word_it).begin()) - firstletteroffset;

					//calculate the length of the next word
					for(std::string::iterator let_it = (*word_it).begin();
									let_it != (*word_it).end() ;  ++let_it)
					{
						prevletlen = GetLetterSpace(*let_it); //space between previous letter
						wordlength += DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*
										( 1.0f - (prevletlen + GetLetterSpace(*let_it)));
					}
					//wordlength +=DATA("LETTER_WID_SCALE")*textbox.GetLetterWidth()*(1.0f);
				}
			}

			//go to the next row
			heightiter += DATA("LETTER_HT_SCALE") * textbox.GetLetterHeight();
		}
		glEnd();

		//ViewPerspective();						// Switch To A Perspective View
		glBindTexture(GL_TEXTURE_2D, 0);			// Bind To The Blur Texture

		if (textbox.hasCursor()) {
			if (widthiter < 1) {
				widthiter = 4;
			}
			glBegin(GL_TRIANGLE_STRIP);
				glColor4f(0.0f, 0.0f, 0.0f, textbox.GetCursorAlpha());
				GLuint x = textbox.GetPosX() + widthiter;
				GLuint y = textbox.GetPosY();
				glVertex2f(x + 2, y);
				glVertex2f(x, y);
				glVertex2f(x + 2, y + textbox.GetLetterHeight());
				glVertex2f(x, y + textbox.GetLetterHeight());
			glEnd();
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

/***********************************
Text_Manager Render

brief: render a letter given
Author: Jamie Gault
***********************************/
void Text_Manager::RenderLetter( float x, float y, float width, float height, char c )
{
	float tx0 = (float(c%16)*32.0f)/512.0f;
	float ty0 = (float(c/16)*32.0f)/512.0f;
	float tx1 = tx0 + 32.0f/512.0f;
	float ty1 = ty0 + 32.0f/512.0f;
	float x1 = x + width;
	float y1 = y + width;

	//r = 1.0f - r;
	//glColor4f(r, 0.0f, 0.0f, 1);
	//glBegin(GL_LINE_LOOP);
	//	glVertex2f(x, y);
	//	glVertex2f(x, y1);				// Second Vertex
	//	glVertex2f(x1,y1);				// Third Vertex
	//	glVertex2f(x1,y );				// Fourth Vertex
	//glEnd();

	//std::cout << c << " \n";

	//glBegin(GL_QUADS);						// Begin Drawing Quads
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(tx0,ty0);
		glVertex2f(x, y);

		glTexCoord2f(tx0,ty1);
		glVertex2f( x, y1);				// Second Vertex

		glTexCoord2f(tx1,ty1);
		glVertex2f(x1,y1);				// Third Vertex

		glTexCoord2f(tx1,ty0);
		glVertex2f(x1,y );				// Fourth Vertex
	//glEnd();			// Done Drawing Quads
}

float GetLetterSpace( char c )
{
	switch(c)
	{
	case 'À':
	case 'Á':
	case 'Â':
	case 'Ã':
	case 'Ä':
	case 'Å':
	case 'È':
	case 'É':
	case 'Ê':
	case 'Ë':
	case 'Ç':
	case 'A':
	case 'B':
	case 'E':
	case 'H':
	case 'K':

	case 'L':
	case 'R':

	case 'V':
	case 'X':
	case 'Z':
		return DATA("averageCapital");//0.06f;

	case 'C':
	case 'D':
		return -0.07f;

	case 'Æ':
	case 'Ð':
	case 'Ò':
	case 'Ó':
	case 'Ô':
	case 'Õ':
	case 'Ö':

	case 'G':
	case 'M':
	case 'O':
	case 'Q':
	case 'W':
		return DATA("wideCapital");//0.00f;

	case 'P':
	case 'F':
	case 'T':
	case 'S':
	case 'Y':
	case 'n':
	case 'Ý':
	case 'ñ':
	case 'ð':
		return 0.00f;

	case 'I':
	case 'J':
	case 'r':
	case 'Ì':
	case 'Í':
	case 'Î':
	case 'Ï':
		return 0.10f;

	case 'f':
	case 't':
	case 'j':

	case 'l':
		return 0.17f;

	case 'i':
	case 'ì':
	case 'í':
	case 'î':
	case 'ï':
		return 0.21f;
	case '.':
	case ',':
	case '"':
		return 0.2f;

	case 'm':
	case 'æ':
		return DATA("reallywideletter");

	case 'w':
		return -0.15f;

	case 'à':
	case 'á':
	case 'â':
	case 'ã':
	case 'ä':
	case 'Ù':
	case 'Ú':
	case 'Û':
	case 'Ü':
	case 'Ñ':
	case 'ß':
	case 'ç':

	case 'b':
	case 'N':
	case 'U':
	case 'a':

	case 'p':

		return DATA("wideletter");//-0.80f;

	case 'o':
		return 0.07f;

	case 's':
	case 'è':
	case 'é':
	case 'ê':
	case 'ë':
	case 'e':
	case 'y':
	case 'ý':
	case 'ÿ':
	case 'ø':
		return 0.03f;
	case 'h':
	case 'u':
		return 0.00f;

	default:
		return 0.03f;
	}
}