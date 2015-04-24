/*
	TabMenu.cpp
	Created by: Jamie Gault
	Created on 5/10/07

*/

#include "TabMenu.h"
#include "busted.all.h"


extern Input *pInput;
extern Mentor_Manager *pMentor_man;
extern Texture_Manager* pTexture_man;
extern Text_Manager *pText_man; 


/***********************************
TabMenu Constructor

Author: Jamie Gault
***********************************/
TabMenu::TabMenu(WinData *wd): m_isOpen(false), m_action(TMA_NONE), m_windata(wd)
{
	m_pos_y = DATA("TAB_MENU_POSY") - DATA("TAB_OPEN_HT");

	m_pause_but = Answer(pText_man, std::string("Pause Game"), DATA("TAB_PBUT_POSX"), DATA("TAB_MENU_POSY")+DATA("TAB_BUT_OFFSETY"), 500.0f );
	m_discuss_but = Answer(pText_man, std::string("Discuss"), DATA("TAB_DISCBUT_POSX"), DATA("TAB_MENU_POSY")+DATA("TAB_BUT_OFFSETY"), 500.0f );

	//set the string so the box is the right width
	std::string rndcnt;
	if( m_windata->rule == BOTH_LIMIT || m_windata->rule == ROUND_LIMIT)
		rndcnt = std::string("Round 1 out of 10");
	else
		rndcnt = std::string("Round 1");

	m_rndDisplay = TextBox(pText_man, rndcnt, DATA("TAB_RNDCNT_POSX"), DATA("TAB_MENU_POSY")+DATA("TAB_BUT_OFFSETY"), 700.0f);
	SetTurnText();


	//setup max score
	std::stringstream score;
	score<< "Goal Score is " << m_windata->goal_score << ".";
	m_scrMax = TextBox(pText_man, score.str(), DATA("TAB_SCRMAX_POSX"), DATA("TAB_MENU_POSY")+DATA("TAB_BUT_OFFSETY"), 500.0f);

	
	m_folder = Box_2D(BT_SQUARE, 0.0f, m_pos_y, 1026.0f, 128.0f);
	m_folder.SetBackGround( pTexture_man->GetTexture("TAB_MENU"));

	m_tab = Box_2D(BT_LINES, 0.0f, m_pos_y, 250.0f, 68.0f);

}


/***********************************
TabMenu Update

Author: Jamie Gault
***********************************/
void TabMenu::Update( float t )
{
	//reset the action for the turn
	m_action = TMA_NONE;

	m_pause_but.Update(t);
	m_discuss_but.Update(t);

	



	//if the tab menu is not open
	if( ! m_isOpen )
	{
		if( m_pos_y < DATA("TAB_MENU_POSY") - DATA("TAB_OPEN_HT"))
		{
			m_pos_y += 2.0f*t;
		}
			else
			m_pos_y = DATA("TAB_MENU_POSY") - DATA("TAB_OPEN_HT");

		if( m_tab.IsMouseOver() )
		{
			m_isOpen = true;
		}
	}
	else	//if the tab menu is open
	{

		//check for option selections
		if( pInput->PollMouseBut(MLEFT ) )
		{
			if( m_discuss_but.IsMouseOver())
			{
				m_action = TMA_DISCUSS;
			}
			else if( m_pause_but.IsMouseOver())
			{
				m_action = TMA_PAUSE;
			}
		}


		if( m_pos_y > DATA("TAB_MENU_POSY") - DATA("TAB_CLOSE_HT"))
		{
			m_pos_y -= 2.0f*t;
		}
		else
			m_pos_y = DATA("TAB_MENU_POSY") - DATA("TAB_CLOSE_HT");

		if( pInput->GetMousePosY() < m_pos_y)
		{
			m_isOpen = false;
		}
	}

	m_pause_but.GetTextBox().SetPosY((m_pos_y +DATA("TAB_BUT_OFFSETY")));
	m_discuss_but.GetTextBox().SetPosY((m_pos_y +DATA("TAB_BUT_OFFSETY")));

	m_rndDisplay.SetPosY((m_pos_y +DATA("TAB_BUT_OFFSETY")));
	m_scrMax.SetPosY((m_pos_y +DATA("TAB_BUT_OFFSETY")));

	m_tab.SetPosY((m_pos_y ));
	m_folder.SetPosY((m_pos_y ));
}


/***********************************
TabMenu Update

Author: Jamie Gault
***********************************/
void TabMenu::Render()
{
	
	m_folder.Render();
	m_pause_but.Render();
	m_discuss_but.Render();


	//determine what to display
	if( m_windata->rule == BOTH_LIMIT)
	{
		m_rndDisplay.Render();
		m_scrMax.Render();
	}
	else if( m_windata->rule == ROUND_LIMIT)
	{
		m_rndDisplay.Render();
	}
	else
		m_scrMax.Render();

}


/***********************************
TabMenu SetTurnText

Author: Jamie Gault
***********************************/
void TabMenu::SetTurnText()
{
	//setup the round text
	if( m_windata->rule == BOTH_LIMIT ||  m_windata->rule == ROUND_LIMIT)
	{
		std::stringstream rnd;
		rnd << "Turn " << m_windata->turn << " out of " << m_windata->turn_limit<<".";

		m_rndDisplay.SetString( rnd.str() );
	}
	else 
	{
		std::stringstream rnd;
		rnd << "Turn " << m_windata->turn ;
		m_rndDisplay.SetString( rnd.str() );
	}
}
