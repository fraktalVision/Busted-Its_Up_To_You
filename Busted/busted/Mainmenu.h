/*
	Mainmenu.h
	Created by: Jamie Gault
	Created on 5/10/07

*/


#pragma once

#include "busted.all.h"
#include "Menu.h"
#include "../engine/core/stat_manager/stat_manager.h"
#include "../engine/core/Questions/Questions.h"





//all the possible results to return from the menus
enum MENU_RESULT{ MR_NONE, MR_START, MR_QUIT, MR_RESUME, MR_EXITGAME, MR_RETURN , MR_RETRY}; //posible options selecting yielding results


//menu that is currently active
enum MENU_ACTIVE{ MA_BASE, MA_OPTIONS, MA_QUIT, MA_MODERATOR, MA_PLAYER, MA_CREDITS};



/*
! General options menu for changing video and sound settings.
*/
class GenOptionsMenu
{
public:
	GenOptionsMenu(Text_Manager* tm);

	void Update(float t);			//!<
	void Render();					//!<
	void ApplyOptions();			//!<
	MENU_ACTIVE GetActiveMenu(){return m_men_act;}

	MENU_RESULT GetMenuResult();	//!<
	void Reset() { m_men_res = MR_NONE; m_men_act = MA_BASE; }

private:



	MENU_RESULT m_men_res;		//!<result of the user's choices
	MENU_ACTIVE m_men_act;		//!<active menu


	TextBox m_title;			//!<title of the pause menu

	
	WordMenu m_TextureDetail;	//!<level of detail with the textures
	NumberMenu m_MusicVol_men;	//!<volume level of music
	NumberMenu m_SoundVol_men;	//!<volume level of sound effects
	
	Answer m_back_but;
};



/*
! Class for handling the display of credits

*Credits loaded from the credits.xtx file
*/
class CreditsDisplay
{
public:
	CreditsDisplay();		//!<
	~CreditsDisplay();		//!<

	void Update(float t);	//!<
	void Render();			//!<

	bool IsFinished();		//!<
	
private:
	bool LoadCreditsFile();	//!<
	
	std::vector<Box_2D*> charPics;		//!<characatures of everyone to be displayed throughout
	std::vector<TextBox*> textboxlist;	//!<
	int num_tboxes;
	float *endheight;					//!<pointer to the y value of the last text box on the list;

	float m_YRange;						//!<range in the Y direction of where the credits start and end

	bool loaded;
};





//! Class for handling the button input and display.
class MainMenu
{
public:
	MainMenu(Text_Manager* tm);		//!<
	
	void Update(float t);			//!<
	void Render();					//!<Draw to the screen

	MENU_RESULT GetMenuResult();	//!<Which menu to pull up

	GenOptionsMenu& GetOptMenu(){ return m_optmenu; }//!<Get the options menu

	MENU_ACTIVE GetActiveMenu(){return m_men_act;}//!<Which menu is running


private:


	void LoadModInst();			//!< Loads all the textures onto the graphics card
	void LoadPlaInst();			//!< Loads all the textures onto the graphics card
	void UnloadModInst();		//!< unload all the textures from the graphics card
	void UnloadPlaInst();		//!< unload all the textures from the graphics card

	MENU_RESULT m_men_res;		//!<result of the user's choices
	MENU_ACTIVE m_men_act;		//!<active menu

	Answer m_startBut;			//!<button for starting the game
	Answer m_quitBut;			//!<button for quiting the game
	Answer m_options;			//!<butt for entering into options
	Answer m_Mod_Inst;			//!<butt for entering into moderator instructions
	Answer m_Pla_Inst;			//!<button for entering into player instructions
	Answer m_credits;

	Box_2D m_title;				//!busted title box
	GenOptionsMenu m_optmenu;	//!menu for additional options


	//moderator stuff
	Answer m_exit_mod_but;				/*!<button for exiting the moderator slides	*/
	
	std::vector<GLuint> m_mod_slides;	/*!<all the texture ID's for the moderator slides		*/
	std::vector<GLuint> m_pla_slides;	/*!<all the texture ID's for the player slides		*/
	NumberMenu m_slide_men;				/*!<Menu for incrementing through moderator slides	*/
	NumberMenu m_pla_slide_men;			/*!<Menu for incrementing through player slides	*/

	//object for handling credits display
	CreditsDisplay* m_cred_dis;			//!<
	
	

};





//! Class for handing the input and display of the ingame pause menu
class PauseMenu
{
public:
	PauseMenu(Text_Manager* tm);

	void Update(float t);
	void Render();					/*!<Render all the pause menu buttons		*/

	MENU_RESULT GetMenuResult();	/*!<Get the selection result from the menu	*/

private:

	MENU_RESULT m_men_res;			/*!<result of the user's choices			*/
	MENU_ACTIVE m_men_act;			/*!<active menu								*/


	TextBox m_title;				/*!<title of the pause menu					*/

	Answer m_resumeBut;				/*!<button for starting the game			*/
	Answer m_optionsBut;			/*!<button to bring up the option menu		*/

	Answer m_quitBut;				/*!<button for quiting the game				*/


	Answer m_yes;					/*!<button for exiting the game				*/
	Answer m_no;					/*!<button for exiting the game				*/


	GenOptionsMenu m_optmenu;		/*!<menu for additional options				*/




};





//! Handles the input and displaying of the endgame menu buttons
class EndGameMenu
{
public:
	EndGameMenu(Text_Manager* tm);	//!<

	void Update(float t);			//!<
	void Render();					//!<
	
	MENU_RESULT GetMenuResult();	//!<


private:
	MENU_RESULT m_men_res;	/*!<result of the user's choices	*/

	Answer m_playagainBut;	/*!<button for starting the game	*/
	Answer m_quitBut;		/*!<button for quiting the game		*/

};






