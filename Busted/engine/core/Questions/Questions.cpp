/*
	Questions.cpp
	Created by: Jamie Gault
	Created on 4/25/07

*/

#include "Questions.h"
#include "../../tools/datadrive/datadrive.h"
#include "../../../busted/player/player.h"
#include "../glut/Input.h"

extern Texture_Manager* pTexture_man;
extern Text_Manager* pText_man;

/***********************************
Box_2D Constructor

Author: Jamie Gault
***********************************/
Box_2D::Box_2D(): m_boxtype(BT_LINES), angle(0.0f)
{
	m_textureset.m_idle = 0;
	m_textureset.m_mouse_over = 0;
	m_textureset.m_use = 0;
	m_pos[0] = 0.0f;
	m_pos[1] = 0.0f;
	m_dim[0] = 0.0f;
	m_dim[1] = 0.0f;
	use_maintxt = true;
}

/***********************************
Box_2D Constructor

Author: Jamie Gault
***********************************/
Box_2D::Box_2D( BOX_TYPE bt,float x, float y, float w, float h): m_boxtype(bt), angle(0.0f)
{
	m_textureset.m_idle = 0;
	m_textureset.m_mouse_over = 0;
	m_textureset.m_use = 0;
	m_pos[0] = x;
	m_pos[1] = y;
	m_dim[0] = w;
	m_dim[1] = h;
	use_maintxt = true;
}

/***********************************
Box_2D Constructor

Author: Jamie Gault
***********************************/
Box_2D::Box_2D( unsigned int texture,float x, float y, float w, float h): m_boxtype(BT_SQUARE), angle(0.0f)
{
	m_textureset.m_idle = texture;
	m_textureset.m_mouse_over = 0;
	m_textureset.m_use = texture;
	m_pos[0] = x;
	m_pos[1] = y;
	m_dim[0] = w;
	m_dim[1] = h;
	use_maintxt = true;

	UseBG();
}
Box_2D::Box_2D( unsigned int texture, float x, float y, float w, float h, unsigned int over_texture): m_boxtype(BT_SQUARE), angle(0.0f)
{
	m_textureset.m_idle = texture;
	m_textureset.m_mouse_over = over_texture;
	m_textureset.m_use = texture;
	m_pos[0] = x;
	m_pos[1] = y;
	m_dim[0] = w;
	m_dim[1] = h;
	use_maintxt = true;

	UseBG();
}

/***********************************
Box_2D Constructor

Author: Jamie Gault
***********************************/
Box_2D::Box_2D( Box_2D& box_2D)
{
	m_pos[0] = box_2D.m_pos[0];
	m_pos[1] = box_2D.m_pos[1];
	m_dim[0] = box_2D.m_dim[0];
	m_dim[1] = box_2D.m_dim[1];
	angle = box_2D.angle;
	m_boxtype = box_2D.GetBoxType();
	m_textureset.m_idle = box_2D.GetBackGround();
	m_textureset.m_mouse_over = box_2D.GetAltBackGround();
	use_maintxt = true;
}

/***********************************
Box_2D operator=

Author: Jamie Gault
***********************************/
Box_2D& Box_2D::operator=(Box_2D& box_2D)
{
	m_pos[0] = box_2D.m_pos[0];
	m_pos[1] = box_2D.m_pos[1];
	m_dim[0] = box_2D.m_dim[0];
	m_dim[1] = box_2D.m_dim[1];
	m_boxtype = box_2D.GetBoxType();
	m_textureset.m_idle = box_2D.GetBackGround();
	angle = box_2D.angle;
	use_maintxt = true;

	return (*this);
}

/***********************************
Box_2D IsMouseOver

Author: Jamie Gault
***********************************/
bool Box_2D::IsMouseOver()
{
		//box collision
	if( pInput->GetMousePosX() >= GetPosX() &&
		pInput->GetMousePosX() <= GetPosX() + GetWidth() )
	{
		if( pInput->GetMousePosY() >= GetPosY() &&
			pInput->GetMousePosY() <= GetPosY() + GetHeight() )
		{
			return true;
		}
	}

	return false;
}

/***********************************
Box_2D Render

Author: Jamie Gault
***********************************/
void Box_2D::Render()
{
	//ViewOrtho(); // Switch To An Ortho View

	if( m_boxtype == BT_LINES )
	{
			float x0, y0, x1, y1;

			x0 = m_pos[0];
			x1 = m_pos[0]+ GetWidth();

			y0 = m_pos[1];
			y1 = m_pos[1]+ GetHeight();

			glBegin(GL_LINES);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glVertex2f(x0,y0);
				glVertex2f(x0,y1);

				glVertex2f(x0,y1);
				glVertex2f(x1,y1);

				glVertex2f(x1,y1);
				glVertex2f(x1,y0);

				glVertex2f(x1,y0);
				glVertex2f(x0,y0);
			glEnd();

		//glPopMatrix();
	}
	else if( m_boxtype == BT_SQUARE )
	{
		if( use_maintxt )
			UseBG();	// Bind main texture
		else
			UseAltBG();	// Bind alternate texture

		//if it has a background
		if(  m_textureset.m_use )
			glBindTexture(GL_TEXTURE_2D, m_textureset.m_use);

		glPushMatrix();
			glTranslatef(m_pos[0], m_pos[1], 0.0f);

			//if the angle has been set
			if( angle != 0.0f )
			{
				glTranslatef(GetWidth()/2.0f, GetHeight()/2.0f, 0.0f);
				glRotatef(angle, 0.0f, 0.0f, 1.0f );
				glTranslatef(-GetWidth()/2.0f, -GetHeight()/2.0f, 0.0f);
			}

			glBegin(GL_QUADS);						// Begin Drawing Quads
				//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glTexCoord2f(0,0);
				glVertex2f(0.0f,0.0f);

				glTexCoord2f(0,1);
				glVertex2f(0.0f, GetHeight());				// Second Vertex

				glTexCoord2f(1,1);
				glVertex2f( GetWidth(), GetHeight());				// Third Vertex

				glTexCoord2f(1,0);
				glVertex2f( GetWidth(),0.0f );				// Fourth Vertex
			glEnd();			// Done Drawing Quads

		glPopMatrix();
	}
	else if ( m_boxtype == BT_SQUARE_MIRROR )
	{
				//if it has a background
		if( use_maintxt )
		{
			//if it has a background
			if(  m_textureset.m_use )
				glBindTexture(GL_TEXTURE_2D,  m_textureset.m_use);			// Bind main texture
		}
		else
		{
			//if it has a background
			if( m_textureset.m_mouse_over )
				glBindTexture(GL_TEXTURE_2D, m_textureset.m_mouse_over);			// Bind alternate texture
		}

		glBegin(GL_QUADS);						// Begin Drawing Quads
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(1,0);
			glVertex2f(m_pos[0],m_pos[1]);

			glTexCoord2f(1,1);
			glVertex2f(m_pos[0],m_pos[1] + GetHeight());				// Second Vertex

			glTexCoord2f(0,1);
			glVertex2f(m_pos[0] + GetWidth(),m_pos[1] + GetHeight());				// Third Vertex

			glTexCoord2f(0,0);
			glVertex2f(m_pos[0] + GetWidth(),m_pos[1] );			// Fourth Vertex
		glEnd();			// Done Drawing Quads
	}

	//ViewPerspective();						// Switch To A Perspective View
}

/***********************************
Answer Constructor

Author: Jamie Gault
***********************************/
Answer::Answer():m_textbox()
{
	//decide which back ground to use based on the dimensions of the bitmap
	//if( m_textbox.GetBoxHeight() > 128.0f || m_textbox.GetBoxWidth() > 256.0f )
		m_textureset.m_idle = pTexture_man->GetTexture("GEN_BUT_IDLE");
	/*else
		m_textureset.m_idle = pTexture_man->GetTexture("TID_TEXTBG");*/

	m_textureset.m_mouse_over = pTexture_man->GetTexture("GEN_BUT_MO");
	m_textureset.m_selected = 0;
	selected = false;
	m_textbox.SetBackground( m_textureset.m_idle );
}

/***********************************
Answer Constructor

Author: Jamie Gault
***********************************/
Answer::Answer( Text_Manager *tx_m, std::string &s, float x0, float y0, float width):
													m_textbox( tx_m, s, x0, y0, width, 10.0f*SCREEN_X_RATIO, 30.0f*SCREEN_X_RATIO, 30.0f*SCREEN_Y_RATIO ),
													m_press_delay( DATA("ANS_PRESS_DMAX") )
{
	//decide which back ground to use based on the dimensions of the bitmap
	//if( m_textbox.GetBoxHeight() > 128.0f || m_textbox.GetBoxWidth() > 256.0f )
		m_textureset.m_idle = pTexture_man->GetTexture("GEN_BUT_IDLE");
	/*else
		m_textureset.m_idle = pTexture_man->GetTexture("TID_TEXTBG");*/

	m_textureset.m_mouse_over = pTexture_man->GetTexture("GEN_BUT_MO");
	m_textureset.m_selected = pTexture_man->GetTexture("GEN_BUT_CLICK");
	selected = false;
	m_textbox.SetBackground( m_textureset.m_idle );
}

/***********************************
Answer Constructor

Author: Jamie Gault
***********************************/
Answer::Answer( Answer& a ): m_textbox( a.GetTextBox() ),m_press_delay( DATA("ANS_PRESS_DMAX") )
{
	m_textureset.m_idle = a.m_textureset.m_idle;
	m_textureset.m_mouse_over = a.m_textureset.m_mouse_over;
	m_textureset.m_selected = a.m_textureset.m_selected;
	selected = a.selected;
	m_textbox.SetBackground( m_textureset.m_idle );
}

/***********************************
Answer Constructor

Author: Jamie Gault
***********************************/
Answer::Answer( TextBox& t ):  m_textbox( t ), m_press_delay( DATA("ANS_PRESS_DMAX") )
{
	//decide which back ground to use based on the dimensions of the bitmap
	//if( m_textbox.GetBoxHeight() > 128.0f || m_textbox.GetBoxWidth() > 256.0f )
		m_textureset.m_idle = pTexture_man->GetTexture("GEN_BUT_IDLE");
	/*else
		m_textureset.m_idle = pTexture_man->GetTexture("TID_TEXTBG");*/

	m_textureset.m_mouse_over = pTexture_man->GetTexture("GEN_BUT_MO");
	m_textureset.m_selected = pTexture_man->GetTexture("GEN_BUT_CLICK");
	m_textbox.SetBackground( m_textureset.m_idle );
	selected = false;
}

/***********************************
Answer operator=

Author: Jamie Gault
***********************************/
Answer& Answer::operator=( Answer& a)
{
	m_textbox = a.GetTextBox();
	m_textureset.m_idle = a.m_textureset.m_idle;
	m_textureset.m_mouse_over = a.m_textureset.m_mouse_over;
	m_textureset.m_selected = a.m_textureset.m_selected;
	selected = a.selected;
	m_textbox.SetBackground( m_textureset.m_idle );
	return *this;
}

/***********************************
Answer IsMouseOver

Author: Jamie Gault
***********************************/
bool Answer::IsMouseOver()
{
	//box collision
	if( pInput->GetMousePosX() >= m_textbox.GetPosX() + m_textbox.GetBGOffset(0, 0) &&
		pInput->GetMousePosX() <= m_textbox.GetPosX() + m_textbox.GetBoxWidth() + m_textbox.GetBGOffset(1, 0))
	{
		if( pInput->GetMousePosY() >= m_textbox.GetPosY()+ m_textbox.GetBGOffset(0, 1) &&
			pInput->GetMousePosY() <= m_textbox.GetPosY() + m_textbox.GetBoxHeight() + m_textbox.GetBGOffset(1, 1) )
		{
			return true;
		}
	}

	return false;
}

/***********************************
Answer Update

Author: Jamie Gault
***********************************/
void Answer::Update(float t)
{
	m_press_delay += t;

	if( m_press_delay >= DATA("ANS_PRESS_DMAX") )
	{
		//determine the background to be used
		if( IsMouseOver() || selected )
		{
			if( pInput->PollMouseBut(MLEFT) )
			{
				m_press_delay = 0.0f;
				m_textbox.SetBackground( m_textureset.m_selected );
			}
			else
				m_textbox.SetBackground( m_textureset.m_mouse_over );
		}
		else
		{
			m_textbox.SetBackground( m_textureset.m_idle );
		}
	}
}

/***********************************
Answer Render

Author: Jamie Gault
***********************************/
void Answer::Render()
{
	m_textbox.Render(); //draw textbox
}

/***********************************
Answer Render

Author: Jamie Gault
***********************************/
void Answer::SetBGs(GLuint idle, GLuint mo, GLuint click )
{
	m_textureset.m_idle = idle;
	m_textureset.m_mouse_over = mo;
	m_textureset.m_selected  = click;
	m_textbox.SetBackground(m_textureset.m_idle);
}

/***********************************
Answer GetTextBox

Author: Jamie Gault
***********************************/
TextBox& Answer::GetTextBox()
{
	return m_textbox;
}

/***********************************
DropDownMenu Constructor

Author: Jamie Gault
***********************************/
DropDownMenu::DropDownMenu(Text_Manager *tx_m,int player_id, std::string player_name, float x, float y):
																		m_active(false),
																		m_score_given(0),
																		m_player_num(player_id)
{
	std::stringstream identifier;
	identifier << player_name << ": ";

	user_id_box = new Box_2D(pTexture_man->GetTexture( std::string("icon_") + player_name ), x, y, 75.0f, 75.0f);//new Answer(tx_m, identifier.str(), x, y, 500.0f);

	float ht = 40.0f;//user_id_box->GetTextBox().GetBoxHeight();

	for(int i = 0; i < 4; ++i )
	{
		Answer* ans = new Answer( tx_m, std::string("-1"),
								x + user_id_box->GetWidth(),
								y- i*ht + DATA("DD_MEN_OFFSETY"), 400.0f );

		ans->GetTextBox().SetBGOffset( 0.0f, 0, 0 );
		ans->GetTextBox().SetBGOffset( 0.0f, 1, 0 );
		ans->GetTextBox().SetBGOffset( -0.0f, 0, 1 );
		ans->GetTextBox().SetBGOffset( 0.0f, 1, 1 );

		char num[50];
		sprintf(num, "%i", i );
		ans->GetTextBox().SetString( std::string(num));
		ans->GetTextBox().SetVisible( false );

		m_score_menu.push_back(ans);
	}

	//frame = Box_2D((*m_score_menu.begin())->m_textureset.m_idle, /*user_id_box->GetPosX()*/x, y/*user_id_box->GetPosY()*/, 75.0f+(*m_score_menu.begin())->GetTextBox().GetBoxWidth(), 75.0f);
}

/***********************************
DropDownMenu Destructor

Author: Jamie Gault
***********************************/
DropDownMenu::~DropDownMenu()
{
	delete user_id_box;

	for(std::vector<Answer*>::iterator option = m_score_menu.begin(); option != m_score_menu.end(); ++option )
	{
		delete(*option);
	}
}

/***********************************
DropDownMenu CheckActive

Author: Jamie Gault
***********************************/
void DropDownMenu::Update(float t)
{
	for(std::vector<Answer*>::iterator option = m_score_menu.begin(); option != m_score_menu.end(); ++option )
	{
		(*option)->Update(t);
	}

	if( ! m_active )
	{
		//if the mouse is over the drop down frame
		if( frame.IsMouseOver())
		{
			SetActivity(true);
		}
	}
	else
	{
		float minx = user_id_box->GetPosX() ;

		float miny = user_id_box->GetPosY() -
				4.0f*((*m_score_menu.begin())->GetTextBox().GetBoxHeight() +
				(*m_score_menu.begin())->GetTextBox().GetBGOffset(1,1));

		float maxx = user_id_box->GetPosX() + user_id_box->GetWidth() + (*m_score_menu.begin())->GetTextBox().GetBGOffset(1,0) +
						(*m_score_menu.begin())->GetTextBox().GetBoxWidth() ;
		float maxy = user_id_box->GetPosY() + 2.0f*((*m_score_menu.begin())->GetTextBox().GetBoxHeight()+ (*m_score_menu.begin())->GetTextBox().GetBGOffset(1,1));

		//box check
		bool mouseover = (pInput->GetMousePosX() >= minx)&&(pInput->GetMousePosX() <= maxx)&&
						 (pInput->GetMousePosY() >= miny)&&(pInput->GetMousePosY() <= maxy);

		bool answerselected = false;

		//if the user selects an answer
		if( pInput->PollMouseBut(MLEFT) )
		{
			int i = 0;

			for( std::vector<Answer*>::iterator  ai = m_score_menu.begin();
				ai != m_score_menu.end() && !answerselected; ++ai, ++i )
			{
				if( (*ai)->IsMouseOver() )
				{
					answerselected = true;
					m_score_given = i;
				}
			}
		}

		//set the drop down box to idle
		if( ! mouseover || answerselected )
		{
			SetActivity(false);
		}
	}
}

/***********************************
DropDownMenu Render

Author: Jamie Gault
***********************************/
void DropDownMenu::Render()
{
	//render frame box

	frame = Box_2D((*m_score_menu.begin())->m_textureset.m_idle, user_id_box->GetPosX(), user_id_box->GetPosY(), 75.0f+(*m_score_menu.begin())->GetTextBox().GetBoxWidth(), 75.0f);

	frame.Render();

	user_id_box->Render();

	if(!m_active)
	{
		//generate a box with the player's score
		char score[10];
		sprintf(score, "%i",  m_score_given);

		TextBox Score( (*m_score_menu.begin())->GetTextBox().GetFontPtr(), std::string( "-5" ),
			(*m_score_menu.begin())->GetTextBox().GetPosX() + 2.0f*(*m_score_menu.begin())->GetTextBox().GetBGOffset(1,0),
			(*m_score_menu.begin())->GetTextBox().GetPosY(),500.0f, 10.0f*SCREEN_X_RATIO, 40.0f*SCREEN_X_RATIO, 40.0f*SCREEN_Y_RATIO);

		Score.SetBGOffset( 0.0f, 0, 0 );
		Score.SetBGOffset( 0.0f, 1, 0 );
		Score.SetBGOffset( -0.0f, 0, 1 );
		Score.SetBGOffset( 0.0f, 1, 1 );

		Score.SetString(std::string( score ));

		Score.SetBackground( 0/*(*m_score_menu.begin())->m_textureset.m_idle*/ );

		Score.Render();
	}
	else
	{
		//render all drop down options
		for(std::vector<Answer*>::iterator option = m_score_menu.begin();
										option != m_score_menu.end(); ++option )
		{
			(*option)->Render();
		}
	}
}

/***********************************
DropDownMenu IsActive

Author: Jamie Gault
***********************************/
bool DropDownMenu::IsActive()
{
	return m_active;
}

/***********************************
DropDownMenu SetScore

Author: Jamie Gault
***********************************/
void DropDownMenu::SetScore( int s )
{
	m_score_given = s;
}

/***********************************
DropDownMenu GetScore

Author: Jamie Gault
***********************************/
int DropDownMenu::GetScore()
{
	return m_score_given;
}

/***********************************
DropDownMenu SetActive

Author: Jamie Gault
***********************************/
void DropDownMenu::SetActivity(bool act)
{
	m_active = act;

	//iterate through and set all options to not visible
	for(std::vector<Answer*>::iterator option = m_score_menu.begin(); option != m_score_menu.end(); ++option )
	{
		(*option)->GetTextBox().SetVisible( act);
	}
}

/***********************************
DropDownMenu SetPos

Author: Jamie Gault
***********************************/
void DropDownMenu::SetPos(float x, float y)
{
	user_id_box->SetPosX(x);
	user_id_box->SetPosY(y);

	float ht = 40.0f;//user_id_box->GetTextBox().GetBoxHeight();
	int i = 0;
	for(std::vector<Answer*>::iterator option = m_score_menu.begin(); option != m_score_menu.end(); ++option, ++i)
	{
		(*option)->GetTextBox().SetPosX(x+ user_id_box->GetWidth());
		(*option)->GetTextBox().SetPosY(y-ht*i + DATA("DD_MEN_OFFSETY"));
	}
}

/***********************************
Question Constructor

Author: Jamie Gault
***********************************/
Question::Question( unsigned int question_id, QuestionType qt, std::string category, std::string question,
				   std::vector<std::string> answers, std::vector<int> answer_ids,
				   std::vector< player* > *player_vec, int player_id, int playercnt)
{
	Text_Manager* font = pText_man;
	m_result_score = 0;
	m_question_id = question_id;
	m_answered = false;
	m_background = 0;
	m_backblend = 0.0f;
	trans_anim = 0.0f;
	m_category = category;

	if (m_category == "school")
	{
		std::string test = "";
	}

	//initialize the tag for the identifier box
	m_identifier = Box_2D( pTexture_man->GetTexture(std::string("SitHd_")+category), DATA("Q_IDENT_POSX"),
							DATA("Q_IDENT_POSY"), DATA("Q_IDENT_WD"), DATA("Q_IDENT_HT"));

	m_questionbox = TextBox( font,
							question,
							DATA("question_pos_x")*SCREEN_X_RATIO,
							DATA("question_pos_y")*SCREEN_Y_RATIO,
							DATA("question_wid")*SCREEN_X_RATIO, 15.0f*SCREEN_X_RATIO,
							30.0f*SCREEN_X_RATIO, 30.0f*SCREEN_Y_RATIO);

	m_questionbox.SetBGOffset(DATA("que_brd_lf"), 0, 0 );
	m_questionbox.SetBGOffset(DATA("que_brd_rt"), 1, 0 );
	m_questionbox.SetBGOffset(DATA("que_brd_up"), 0, 1 );
	m_questionbox.SetBGOffset(DATA("que_brd_dn"), 1, 1 );

	if( m_questionbox.GetBoxHeight() > m_questionbox.GetLetterHeight()*2.0f)
	{
		if( m_questionbox.GetBoxHeight() > m_questionbox.GetLetterHeight()*3.0f)
			m_questionbox.SetBackground( pTexture_man->GetTexture("QUESTBG_L") );
		else
			m_questionbox.SetBackground(pTexture_man->GetTexture("ANSBG_128") );
	}
	else
		m_questionbox.SetBackground(pTexture_man->GetTexture("ANSBG_64") );

	m_answer_index = -1;
	m_qtype = qt;

	//copy answer ID blocks
	m_answer_IDs.assign( answer_ids.begin(),  answer_ids.end());

	//accumulator for the positions for where the answers should be positioned
	float height = m_questionbox.GetPosY() + m_questionbox.GetBoxHeight() + DATA("que_space_between");

	if( qt != OPEN_ENDED)
	{
		int i = 0;
		//generate all the answers in sequence
		for( std::vector<std::string>::iterator ans_it = answers.begin(); ans_it != answers.end() ; ++ans_it, ++i)
		{
			Answer* ans = new Answer( font, (*ans_it), m_questionbox.GetPosX() + 50.0f, height, m_questionbox.GetBoxWidth()- 50.0f);

			if( qt == MULTIPLE_CHOICE)
			{
				if( ans->GetTextBox().GetBoxHeight() > ans->GetTextBox().GetLetterHeight()*2.0f)
				{
					ans->m_textureset.m_idle = pTexture_man->GetTexture("ANSBG_128") ;
					ans->m_textureset.m_mouse_over = pTexture_man->GetTexture("ANSBG_128_MO");
				}
				else
				{
					ans->m_textureset.m_idle = pTexture_man->GetTexture("ANSBG_64") ;
					ans->m_textureset.m_mouse_over = pTexture_man->GetTexture("ANSBG_64_MO");
				}
			}

			m_answers.push_back( ans);

			std::string letter; //texture id for the button
			//get the right texture
			switch( i )
			{
			default:
			case 0:
				letter = std::string("A");
				break;
			case 1:
				letter = std::string("B");
				break;
			case 2:
				letter = std::string("C");
				break;
			case 3:
				letter = std::string("D");
				break;
			}

			Answer* button = new Answer( TextBox(pText_man, m_questionbox.GetPosX() - 20.0f, height - 20.0f, 50.0f, 65.0f, 10.0f, 45.0f, 45.0f));
			button->GetTextBox().SetString(letter);
			//button->GetTextBox().SetBGOffset(0.0f, 1, 1 );
			button->m_textureset.m_idle = pTexture_man->GetTexture( std::string("ANSBUT_IDLE") );
			button->m_textureset.m_mouse_over = pTexture_man->GetTexture(std::string("ANSBUT_MO"));
			button->m_textureset.m_selected = pTexture_man->GetTexture(std::string("ANSBUT_CLICK"));
			m_answer_buts.push_back( button );

			height += ans->GetTextBox().GetBoxHeight() + DATA("que_space_between");
		}
	}

	//if question is of the open ended type
	if(qt == OPEN_ENDED)
	{
		height += m_questionbox.GetBoxHeight() + 3.5f*DATA("que_space_between");

		float x = 0.0f;

		//load in drop down menus for everyone
		for( int i = 1; i <= playercnt; ++i )
		{
			//skip the player that is not currently active
			if( i == player_id )
				continue;

			DropDownMenu* ddmenu = new DropDownMenu(font, i, ((*player_vec)[i-1])->getName(), m_questionbox.GetPosX() + x - 1000.0f, 425.0f);

			x += ddmenu->GetWidth() + 80.0f*SCREEN_X_RATIO;

			m_ddmenu_vec.push_back(ddmenu);
		}

		//load continue button in bottom left corner
		Answer* ans = new Answer( font, (*answers.begin()), 800.0f,	600.0f, m_questionbox.GetBoxWidth()- 50.0f);

		m_answers.push_back( ans);

		m_priv_but = Answer(pText_man, std::string("Request privacy."), DATA("PRIV_BUT_POSX"),	DATA("PRIV_BUT_POSY"), 500.0f);
	}
}

/***********************************
Question Destructor

Author: Jamie Gault
***********************************/
Question::~Question()
{
	//delete the drop down menu

	for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
			ddmv != m_ddmenu_vec.end(); ++ddmv )
	{
		delete(*ddmv);
	}

	//delete all answers
	for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
	{
		delete (*ans_it);
	}

		//delete all answer buttons
	for( std::vector<Answer*>::iterator ansbut_it = m_answer_buts.begin(); ansbut_it != m_answer_buts.end() ; ++ansbut_it )
	{
		delete (*ansbut_it);
	}
}

/***********************************
Question Update

Author: Jamie Gault
***********************************/
void Question::Update(float t)
{
	//fade the question in over time
	if( m_backblend < 1.0f )
	{
		m_backblend += t/20.0f;
		return;
	}
	else
		m_backblend = 1.0f;

	//animate only when the trans_anim is less than 100
	if( trans_anim < 100.0f )
	{
		trans_anim += t * 2.0f;

			//used for scaling movement by the questions and the answers
		if( trans_anim > 100.0f )
			trans_anim = 100.0f;

		//run the animation for transitioning into the question
		if( !m_answered )
		{
			m_questionbox.SetPosY( DATA("question_pos_y") + 500.0f*trans_anim/100.0f - 500.0f);

			//shift the open ended questions
			float x = 0.0f;
			for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
				ddmv != m_ddmenu_vec.end(); ++ddmv )
			{
				(*ddmv)->SetPos(x+m_questionbox.GetPosX() + 1000.0f*trans_anim/100.0f - 1000.0f, (*ddmv)->GetPosY());

				x += (*ddmv)->GetWidth() + 80.0f*SCREEN_X_RATIO;
			}

			if(m_qtype != OPEN_ENDED)
			{
				//shift over all the buttons

				for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
				{
					(*ans_it)->GetTextBox().SetPosX( m_questionbox.GetPosX() + 50.0f + 1000.0f*trans_anim/100.0f - 1000.0f);
				}
			}
			else
			{
				//shift the continue button
				(*m_answers.begin())->GetTextBox().SetPosX( 800.0f*SCREEN_X_RATIO + 1000.0f*trans_anim/100.0f - 1000.0f);
			}

			//shift all the letter buttons
			for( std::vector<Answer*>::iterator ansbut_it = m_answer_buts.begin(); ansbut_it != m_answer_buts.end() ; ++ansbut_it )
			{
				(*ansbut_it)->GetTextBox().SetPosX( m_questionbox.GetPosX() - 30.0f + 1000.0f*trans_anim/100.0f - 1000.0f );
			}

			m_priv_but.GetTextBox().SetPosY( DATA("PRIV_BUT_POSY") + 400.0f*(1.0f - trans_anim/100.0f));

		}
		else
		{
			m_questionbox.SetPosY( DATA("question_pos_y")*SCREEN_Y_RATIO - 500.0f*trans_anim/100.0f);
			m_resultsbox.SetPosX(DATA("result_pos_x")*SCREEN_X_RATIO + 1000.0f*trans_anim/100.0f - 1000.0f);

			//shift the open ended questions
			float x = 0.0f;
			for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
				ddmv != m_ddmenu_vec.end(); ++ddmv )
			{
				(*ddmv)->SetPos(x+m_questionbox.GetPosX() + 1000.0f*trans_anim/100.0f , (*ddmv)->GetPosY());

				x += (*ddmv)->GetWidth() + 80.0f;
			}

			if(m_qtype != OPEN_ENDED)
			{
				for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
				{
					(*ans_it)->GetTextBox().SetPosX( m_questionbox.GetPosX() + 50.0f + 1000.0f*trans_anim/100.0f );
				}

				//shift all the letter buttons
				for( std::vector<Answer*>::iterator ansbut_it = m_answer_buts.begin(); ansbut_it != m_answer_buts.end() ; ++ansbut_it )
				{
					(*ansbut_it)->GetTextBox().SetPosX( m_questionbox.GetPosX() - 30.0f + 1000.0f*trans_anim/100.0f  );
				}
			}
			else
			{
				m_priv_but.GetTextBox().SetPosY( DATA("PRIV_BUT_POSY") + 400.0f*trans_anim/100.0f);
			}

			//update all the buttons
			for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
			{
				(*ans_it)->Update(t);
			}

				//update all drop down menus
			for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
					ddmv != m_ddmenu_vec.end(); ++ddmv )
			{
				(*ddmv)->Update(t);
			}

			if( m_privacy.m_enable && ( m_privacy.m_limit_value > 0 || ! m_privacy.m_limited ) )
				m_priv_but.Update(t);

			return;
		}
	}

	if(m_qtype != OPEN_ENDED)
	{
		int id = 0;
		//if the left mouse was pressed
		for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it, ++id )
		{
			m_answer_buts[id] ->Update(t);

			if( (*ans_it)->IsMouseOver() || m_answer_buts[id]->IsMouseOver() )
			{
				//m_answer_buts[id]->use_maintxt = false;
				m_answer_buts[id]->selected = true;
				(*ans_it)->selected = true;
			}
			else
			{
				m_answer_buts[id]->selected = false;
				(*ans_it)->selected = false;
			}
		}
	}

	//update all the buttons
	for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
	{
		(*ans_it)->Update(t);
	}

	//update all drop down menus
	for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
			ddmv != m_ddmenu_vec.end(); ++ddmv )
	{
		(*ddmv)->Update(t);
	}

	//updating
	if( m_privacy.m_enable)
		m_priv_but.Update(t);
}

/***********************************
Question GetAnswer

Author: Jamie Gault
***********************************/
int Question::PollForAnswer()
{
	//wait until the answers are available before there can be an answer
	if( m_backblend < 1.0f )
		return NO_ANSWER;

	//privacy check
	if( m_qtype == OPEN_ENDED )
	{
		//if privacy was selected
		if( m_priv_but.IsMouseOver() && pInput->PollMouseBut(MLEFT) )
		{
			m_answered = true;
			m_answer_index = ANSWER_PRIVACY;
			return ANSWER_PRIVACY;
		}
	}

	int id = 0;
	//if the left mouse was pressed
	for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it, ++id )
	{
		if( (*ans_it)->IsMouseOver()|| (*ans_it)->selected )
		{
			//if the score is based on the groups answers
			if( m_qtype == OPEN_ENDED )
			{
				float av_score = 0.0f;
				float cnt = 0.0f;

				for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
					ddmv != m_ddmenu_vec.end(); ++ddmv )
				{
					av_score += (float)(*ddmv)->GetScore();
					++cnt;
				}

				//so that the score truncates in the right direction
				float posnegscale = ( av_score > 0.0f ) ? 1.0f: -1.0f;

				m_result_score = av_score/cnt + posnegscale* 0.5f;
			}

			m_answered = true;
			m_answer_index = id;
			return m_answer_IDs[id];
		}
	}

	return NO_ANSWER;
}

/***********************************
Question GetAnswerIndex

Author: Zak Whaley
***********************************/
int Question::GetAnswerIndex()
{
	return this->m_answer_index;
}

/***********************************
Question AssignResults

Author: Jamie Gault
***********************************/
void Question::AssignResults( std::string results , int score)
{
	if( m_qtype != OPEN_ENDED && m_answer_index != ANSWER_PRIVACY)
	m_resultsbox = TextBox( m_questionbox.GetFontPtr(),   results, DATA("result_pos_x")-1000.0f,
							DATA("result_pos_y"), DATA("result_wid"),
							DATA("result_gap") , DATA("result_let_wid"),
							DATA("result_let_ht"));

	m_resultsbox.SetBackground( m_questionbox.GetBackGround());
	m_resultsbox.SetBGOffset(DATA("res_brd_lf"), 0, 0 );
	m_resultsbox.SetBGOffset(DATA("res_brd_rt"), 1, 0 );
	m_resultsbox.SetBGOffset(DATA("res_brd_up"), 0, 1 );
	m_resultsbox.SetBGOffset(DATA("res_brd_dn"), 1, 1 );

	m_result_score = score;
	trans_anim = 0.0f; //reset counter for animating
}

/***********************************
Question Render

Author: Jamie Gault
***********************************/
void Question::Render()
{
	if( m_backblend < 1.0f )
	{
		Box_2D q_title( pTexture_man->GetTexture(std::string("SitHd_")+m_category),DATA("Q_LOAD_POSX"),
						DATA("Q_LOAD_POSY"), DATA("Q_LOAD_WD"), DATA("Q_LOAD_HT"));
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f- m_backblend);//m_cam_man.GetTransInterp()*DATA("Q_LOAD_SPEED"));
		q_title.Render();
	}

	RenderCoordScreenImage( m_background,
							50.0f,
							50.0f,
							(DATA("SCREEN_WIDTH") -50.0f),
							(DATA("SCREEN_HEIGHT") -100.0f),
							m_backblend );

	//if the background has been completely faded in
	if( m_backblend == 1.0f )
	{
		m_questionbox.Render();
		m_identifier.Render();

		if( m_qtype == OPEN_ENDED )
		{
			//textbox for explaining what's going on for open ended questions
			TextBox explain = TextBox( m_questionbox.GetFontPtr(),  std::string("When you have answered the question, the other players award points based on whether you were honest in thinking about the problem and coming up with a response."),
							(m_questionbox.GetPosX()+150.0f), (m_questionbox.GetPosY()+m_questionbox.GetBoxHeight()+30.0f) ,
							730.0f, 6.660f ,
							20.0f, 20.0f);

			explain.SetBGOffset(30.0f, 1 , 0 );

			explain.Render();

			//render the privacy button
			if( m_privacy.m_enable )
			{
				//if there is a limit to how many privacy requests they can use
				if(  m_privacy.m_limited && trans_anim == 100.0f )
					m_priv_lim_display.Render();

				//render the privacy button if it's available
				if( ( m_privacy.m_limit_value > 0 || ! m_privacy.m_limited ) )
					m_priv_but.Render();
			}
		}

		//render all the answers
		for( std::vector<Answer*>::iterator ans_it = m_answers.begin(); ans_it != m_answers.end() ; ++ans_it )
		{
			(*ans_it)->Render();
		}

		for( std::vector<Answer*>::iterator ansbut_it = m_answer_buts.begin(); ansbut_it != m_answer_buts.end() ; ++ansbut_it )
		{
			(*ansbut_it)->Render();
		}

		//render all the drop down menus in the question
		for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
			ddmv != m_ddmenu_vec.end(); ++ddmv )
		{
			(*ddmv)->Render();
		}

		//if the question has been completed
		if(m_answered == true)
		{
			if( m_qtype != OPEN_ENDED)
				m_resultsbox.Render();

			//if the transition animations are complete
			if( trans_anim == 100.0f )
			{
				std::stringstream scoretext;
				float scale = ( m_qtype != OPEN_ENDED)? 150.0f: 250.0f;

				//figure out the string for the correct stamp icon
				if( m_result_score >= 0)
					scoretext<< "score_stamp+" << m_result_score;
				else
					scoretext<< "score_stamp-"<< -m_result_score;

				Box_2D score = Box_2D( pTexture_man->GetTexture(scoretext.str() ),
										DATA("result_pos_x")*SCREEN_X_RATIO + 1000.0f*trans_anim/100.0f - 1000.0f,
										DATA("result_pos_y")*SCREEN_Y_RATIO + m_resultsbox.GetBoxHeight() + DATA("result_point_gap"),
										scale, scale );
				score.Render();
			}
		}
	}
}

/***********************************
Question GetOpenEndedScore

Author: Jamie Gault
***********************************/
void Question::GetOpenEndedScore( int score[4] )
{
	int i = 0;
	for(std::vector<DropDownMenu*>::iterator ddmv = m_ddmenu_vec.begin();
			ddmv != m_ddmenu_vec.end(); ++ddmv, ++i)
	{
		score[i] = (*ddmv)->GetScore();
	}
}

/***********************************
Question GetOpenEndedScore

Author: Jamie Gault
***********************************/
void Question::SetPrivacySettings(Privacy privsetup, int player_priv_cnt )
{
	m_privacy = privsetup;
	m_privacy.m_limit_value = player_priv_cnt; //set up the players privacy limit

	//if there is a need to display how many privacies the user has available
	if( m_privacy.m_limited && m_privacy.m_enable)
	{
		std::stringstream s;

		if( m_privacy.m_limit_value > 0 )
		{
			s<< m_privacy.m_limit_value<< " privacy requests left.";
		}
		else
			s << "No more privacy requests left.";

		m_priv_lim_display = TextBox( pText_man, s.str(), DATA("PRIV_BUT_POSX") - 20.0f,
								DATA("PRIV_BUT_POSY")-50.0f, 500.0f, 10.0f,
								20.0f, 20.0f);
	}
}