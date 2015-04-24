#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "../busted.all.h"
#include "../Mentor.h"
#include "../Menu.h"
#include "../Camera.h"
#include "../MainMenu.h"
#include "../TabMenu.h"
#include "../hud.h"
#include "../Polish.h"

#include "../../engine/core/stat_manager/stat_manager.h"
#include "../../engine/core/stat_manager/TurnData/TurnData.h"
#include "../../engine/core/Questions/Questions.h"
#include "../player/player.h"


enum EXTERNAL_STATE{ ES_PREGAME_LOAD, ES_TITLE, ES_MAIN_MENU, ES_GAME_SETUP, ES_ORDER_ROLL, ES_INGAME, ES_CREDITS }; 
enum INGAME_STATE{  IGS_START, IGS_DICE, IGS_MOVE, IGS_QUESTION, IGS_RESULT, IGS_MENTOR, IGS_STATUS, IGS_COMIC, IGS_SCORE, IGS_END}; 
enum INGAME_MODE{ IGM_NONE, IGM_PAUSE, IGM_DISCUSS }; 


struct Textboxdata
{
	Text_Manager* text_m;
	GLuint background;
};





//!class to encapsulate the entire game state, handle menus, loading and managing all modules.
//! This pulls everything together
class Game_Manager
{
public:
	Game_Manager();
	~Game_Manager();

	//!returns wheter or not to update and render a new frame
	bool UpdateFramerate();

	//updates everything in the game state based on a scale of t (t is the scale of change in time)
	void Update(float t);

	//!Handle input based on what's going on in the IN Game Mode
	void InputInGameContext(float t);


	//!Creates a player character and adds it to the list of players.
	void CreatePlayer(Mentor* m,  pina* mesh, GENDER gen, std::string Name, int set = 1, int priv_limit = 0);

	//!Reset all the players back to what they were when they started the game
	void ResetPlayers();

	//!Deletes all the players and empties the player list
	void ClearAllPlayers();

	//!Cleans up when the game ends
	void EndGameCleanUp();

	player* GetCurrentPlayer(void)					{ return (*m_current_player); };

	INGAME_STATE GetInGameState();		//!<Get the state of the game when the main game is active
	INGAME_MODE GetMode();			//!<Get the mode of the game(whether it is paused or not)
	EXTERNAL_STATE GetExternalGS(){return m_ex_state;} //!<Get's the overal game state


	//!Special case function for rendering tokens in HUD space for when the game is in the menu.
	void ChooseTokenRender();

	//renders the hud, particles, and menus that the game_manager is accountable for
	void Render();

	void LoadStart(); //!<loads in all models and the main menu


	std::vector< player* > m_player_vec; //!< List of all the created players

	float framerate;//!<The number of frames per second
	float framescale; //!<the scalar for how much relative time changed each frame base on the frame limit
	Textboxdata textboxdata;	//!< holds basic text box data that the game manager takes care of


	Camera_Manager m_cam_man; //!<manager for the camera



private:


	//! load the setup menu upon exiting the main menu
	void LoadSetupMenu();
	//! Free up the Setup menu when it's done
	void UnloadSetupMenu();
	//!Load the game and players based on the setup menu
	void LoadGame();
	//!Unload all data created for the game
	void UnloadGame();
	//!Restart the game over again for a new game
	void RestartGame();
	

	void LoadMainMenu();
	void UnloadMainMenu();
	void LoadPauseMenu();
	void UnloadPauseMenu();

	//render functions
	void RenderInGame();
	void RenderGameSetup(); //render calls for the setup menu

	Stat stat;
	TurnDump turndump; //!< list of all the data for each turn
	
	void SetTokenToMove();	//!< Sets the player to move based on what get's rolled and change the game state
	void ShiftPlayerTurn(); //!< iterates to the next player who's turn it is
	void EndTurnShift(); //!< after a player has answered a question, this fuction handles all the transitions to the next person
	bool PointWinCheck(player* p); //!< checks to see if the specific player has won with a certain number of points and loads end game
	bool TurnWinCheck(); //!< check to see if the maximum amount of turns has been hit and loads in the end game mode
	void LoadQuestion(bool qoverride, int q_id); //!< loads in the question from sql and makes it something we can use
	
	

	//members
	INGAME_STATE m_ig_state;  //!< state of the game when playing
	INGAME_MODE m_ig_mode; //!< curent mode that the game is in
	EXTERNAL_STATE m_ex_state; //!< external state of the game that doesn't need to care about game state stuff
	std::vector<player*>::iterator m_current_player;  //!< player whose turn it is
	Question* m_question; //!< the current question loaded
	DiceDisplay* m_dicedisplay; //!< the 3d dice roll system
	ScoreDisplay* m_scoredisplay; //!< the scores that appear at the bottom of the screen
	EndDisplay* m_enddisplay; //!< end of the game display
	

	//buttons with no place else to go
	Answer		m_dicestop_but; //!< button to stop the dice from rolling
	Answer		m_continue_but; //!< button for moving on to other screens.
	TextBox		m_mentor_text; //!< text that the mentor says

	//menus
	SetupMenu* m_pSetupmenu;//!< pointer to the setup menu.
	MainMenu* m_pMainMenu; //!< pointer to the main menu.
	PauseMenu* m_pPauseMenu; //!< point to the pause menu.
	EndGameMenu* m_pEndMenu; //!< pointer to the end game menu.
	PlayOrderRoll* m_pOrderRoll; //!< pointer to the system that determines the order people play in.
	
	ParticleFeature* partfeat; //!< class for managing the intro display
	TabMenu *tabmenu;		//!<  pointer to the tab menu that shows up in game

	struct clockcounter
	{
		unsigned int oldtime;
		float m_delay_counter;	//!< value for accumulating how many frames have passed
		float m_clock_speed; //!< value for speeding up time in game when the usere gets impatient
	} m_system_clock;

	WinData m_win_data; //!< data to record winning time and such

	float m_discuss_fade; //!< variable for accumulating how the discussion screen interpolates in and out.

	Privacy m_privacy;	//!< struct for the privacy options in game 

	int turncount; //!< number of turns that have passed


};

#endif