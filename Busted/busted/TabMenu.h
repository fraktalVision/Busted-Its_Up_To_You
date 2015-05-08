/*
	TabMenu.h
	Created by: Jamie Gault
	Created on 6/20/07

*/

#pragma once

#include "busted.all.h"
#include "Menu.h"
#include "../engine/core/stat_manager/stat_manager.h"
#include "../engine/core/Questions/Questions.h"
#include "hud.h"

//!enum that gives actions back to the game manager in order to correctly assert what needs to be done
enum TAB_MENU_ACT{ TMA_NONE, TMA_PAUSE, TMA_STATS, TMA_DISCUSS };

//!Menu that shows up ingame at the bottom that pops up.
class TabMenu
{
public:
	TabMenu(WinData *wd);

	void Update( float t );
	void Render();
	void SetTurnText();

	Answer			m_pause_but;	//!<button to bring up pause menu
	Answer			m_discuss_but;	//!<button for bringing up discussions
	Box_2D			m_tab;			//!<position of the tab
	Box_2D			m_folder;		//!<the graphic for the folder
	bool			m_isOpen;		//!<true if the menu has been activated
	float			m_pos_y;		//!<y position on screen
	TAB_MENU_ACT	m_action;		//!<action preformed by the menu selection
	int				m_turn;			//!<current turn of the game

	//stuff for display winning stats
	WinData*		m_windata;		//!<data that the user needs to know about to win
	TextBox			m_rndDisplay;	//!<display of the current round and a round limit;
	TextBox			m_scrMax;		//!<displays the max score that the users are going for
};