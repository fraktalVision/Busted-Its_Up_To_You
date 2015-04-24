/*
	Menu.h
	Created by: Jamie Gault
	Created on 5/07/07

*/

#pragma once

#include "busted.all.h"
#include "Mentor.h"
#include "../engine/core/stat_manager/stat_manager.h"
#include "../engine/core/Questions/Questions.h"

extern enum GENDER;


//!class for recording their selections in menus
class PlayerSetup
{
public:
	typedef std::vector<PlayerSetup> list_t;

	PlayerSetup() {}
	PlayerSetup( int player_id, pina* piece, MENTOR_ID men_id, int soundset, GENDER gender);

	friend bool operator<(const PlayerSetup & lhs, const PlayerSetup & rhs) {
		return lhs.m_rollvalue > rhs.m_rollvalue;
	}

public:
	unsigned int m_ID; //!< the player's id number

	pina* m_boardpiece; //!<pointer to the mesh they want to use
	MENTOR_ID m_mentor; //i!<d of the mentor they choose
	int m_soundset; //!<set of sounds that the user chooses
	
	GENDER m_gender; //!<gender of the indivdual
	std::string m_name; //!<player name

	std::string m_token_name;	//!<name of the token assigne to the player

	int m_rollvalue; //!<value of dices rolled when deciding the order of players
};





//!actions that the arrows can preform
enum ARROW_ACTION{ AA_NONE, AA_INC, AA_DEC };

//!class for displaying arrows and registering presses
class IncDecArrows
{
public:
	IncDecArrows();
	IncDecArrows( unsigned int defvalue, float xL, float yL, float xR, float yR, 
					unsigned min = 0, unsigned int max = 10, unsigned int inc = 1);
	IncDecArrows( IncDecArrows &arrow );
	IncDecArrows& operator=( IncDecArrows &arrows);

	void Update();

	void Render();
	ARROW_ACTION IsPressed(); //!<returns whether or not either button was pressed that turn;

	void SetValue(int i );
	void Reset();			//!<resets the arrows back tot he default value

	unsigned int m_value;
	unsigned int m_defvalue;	//!<the original value that the arrows started with
	unsigned int m_min;			//!<the minimum value that can be set
	unsigned int m_max;			//!<the max value that can be set
	unsigned int m_inc;			//!<the incremental value for each arrow press

	ARROW_ACTION m_pressed;
	Box_2D m_Left_but; //!<button to decrease or move left
	Box_2D m_Right_but; //!<button to increase or move right

};



//!class for general number management like determining the number of players
class NumberMenu : public thing
{
public:
	NumberMenu();
	NumberMenu( Text_Manager* tm, std::string identifier, unsigned defvalue, unsigned min, unsigned max, unsigned int inc, float x, float y, bool displaymax = false);
	NumberMenu( NumberMenu& menu );
	NumberMenu& operator=( NumberMenu& menu );

	void Update();
	void Render();
	void Reset(){ m_arrows.Reset(); UpdateValueDisplay(); }

	unsigned int GetValue() { return m_arrows.m_value; }

	void SetValue(unsigned i ) { m_arrows.m_value = i; UpdateValueDisplay();}

	IncDecArrows m_arrows; //!<buttons for increasing and decreasing


private:
	void UpdateValueDisplay();
	
	TextBox m_identifier; //!< box to say what the number corresponds to
	TextBox m_value_display; //!<box showing value
	bool m_displaymax; //!<whether or not to display the maximum along witht he current value
};



//!class for options with words instead of numbers
class WordMenu : public thing
{
public:
	WordMenu();
	WordMenu( Text_Manager* tm, std::string identifier, std::list<std::string> &options, float x, float y );
	WordMenu( WordMenu& menu );
	WordMenu& operator=( WordMenu& menu );

	void Update();
	void Render();

	void Reset() {
		SetOption(0);
	}
	int GetOption() { 
		return m_arrows.m_value; 
	}
	void SetOption(int i);

private:

	void UpdateDisplay();

	std::list<std::string> m_options;
	std::list<std::string>::iterator m_cur_option;

	IncDecArrows m_arrows; //!<buttons for increasing and decreasing
	TextBox m_identifier; // !<box to say what the number corresponds to
	TextBox m_option_display; //!<box showing value
};



//!class for text input
class EditMenu : public thing
{
public:
	EditMenu();
	EditMenu(Text_Manager* tm, std::string identifier, float x, float y, int max_length);

	void Update();
	void Render();

	void Reset();
	void SetText(const std::string & text);
	std::string GetText();

private:
	int m_max_text_length;
	TextBox m_title_box;
	TextBox m_input_box;
};


//!option for picking pieces
class PieceOption
{
public:
	PieceOption( pina* obj, std::string name);
	~PieceOption();

	obj m_piece;	//!<Piece pointer 
	bool m_taken;	//!< Whether the piece has been taken or not
	std::string m_token_name; //!<Name of the token assigned.
};





enum PIECE_ACTION{ PA_MOVELEFT, PA_MOVERIGHT, PA_PAUSE };


//!menu for managing piece options in the setup menu
class PieceMenu
{
public:
	PieceMenu();
	~PieceMenu();


	void Update(float t);						//!<Update position etc. based on current input
	void Render();								//!<Draw members to the screen
	void Reset();								//!<Kill all player associations
	std::list<PieceOption*>::iterator GetSelectedPiece(){ return m_selected_piece; }

	void OpenPiece( pina* model );				//!<Load a token model
	PieceOption* ChoosePiece();					//!<Select the hi-lighted token and associate it with the player

	void SetVisible( bool v, bool all = false);	//!<

	std::list<PieceOption*> m_pie_opt_list;		//!<vector of all of the piece options

private:
	void MoveLeft();			//!<Get the next token to the left
	void MoveRight();			//!<Get the next token to the right

	void LoadDisplayPieces();	//!<Load all the tokens for drawing
	void UpdatePiecePos();		//!<Update rotational position

	void FindUnselected();		//!<Find the next token not already selected by a player

	float base_pos[3];			//!<the position all tokens have in relation to each other
	float goal_pos[3];			//!<the position everthing will rotate to
	PIECE_ACTION m_action;		//!<the current action of selectiong (left, right, none)

	
	std::list<PieceOption*>::iterator m_selected_piece;	//!<Current hi-lighted piece
	IncDecArrows m_arrows;		//!<buttons for increasing and decreasing

};





//!options for the mentors
class MentorOption
{
public:
	MentorOption( Box_2D box, Mentor* men_id );

	bool	playingPickMe;	//!<Is the sound playing
	bool	playingThanks;	//!<Is the sound playing

	void	pickMe(void);	//!<Play the pleading audio
	void	thanks(void);	//!<Play the selection audio

	MENTOR_ID GetID();		//!<Which mentor is it?
	
	void Render();			//!<Draw the mentor

	Box_2D	m_box;			//!<The box to frame this mentor in
	Mentor* m_mentor;		//!<The actual mentor
	bool m_selected;		//!<Is this mentor selected

};




//!class for manage the mentor options a
class MentorMenu
{
public:
	MentorMenu();
	~MentorMenu();

	MentorOption* PollOptions();	//!<Get the selected mentor option
	void Render();
	void SetSelectedMentor(int id);
	MentorOption* GetSelectedMentor();

private:
	std::vector<MentorOption*> m_men_opt_vec; //!vector of all of the mentor options
	MentorOption* m_selected_mentor;
};





//!screen showing the configurations for all screens
class ConfigConfirm
{
public:
	ConfigConfirm(PlayerSetup::list_t & setups);
	~ConfigConfirm();

	void Render();

private:
	std::vector<TextBox*> name_vec;		//!<The names of all the players
	std::vector<Box_2D*> token_pics;	//!<The images of the tokens selected by the players
	std::vector<Box_2D*> mentor_pics;	//!<The images of the mentors selected by the players
	std::vector<Box_2D*>  sound_pics;	//!<The images of the sounds selected by the players
	std::vector<TextBox*> gender_vec;	//!<The genders of all the players

};




//!options for the mentors
class PictureOption
{
public:
	PictureOption( Box_2D box, int val ): m_box(box), value(val){}

	Box_2D	m_box;
	int value;

};


//!class for manage the mentor options a
class PictureMenu
{
public:
	PictureMenu(std::vector<unsigned int> &pic_ids, std::vector<int> &menu_ids, float x, float y, int max_row);
	PictureMenu(std::vector<BtnTextureSet> &pic_ids, std::vector<int> &menu_ids, float x, float y, int max_row);
	~PictureMenu();

	int PollOptions(); //!<returns the value selected, otherwise returns -1
	void Render();
	void Reset();
	void SetSelectedOpt(int value) {
		m_selected_option = value;
	}
	int GetSelectedOpt() { 
		return m_selected_option; 
	}

private:
	std::vector<PictureOption*> m_pic_opt_vec; //!<vector of all of the mentor options
	int m_selected_option;
};




//!This is the sequence in which the users setup the game DDDD
enum SETUP_MENU_STATE {
	SMS_MAINBACK, 
	SMS_GAME_SETUP, 
	SMS_PLAYER_NAME, 
	SMS_PLAYER_INFO, 
	SMS_PLAYER_TOKEN, 
	SMS_MENTOR, 
	SMS_CONFIRM, 
	SMS_START
};

//!Menu system to encorperate all the given menus
class SetupMenu
{
public:
	SetupMenu(Text_Manager* tm, GLuint t_id = 0);
	~SetupMenu();

	void Update(float t);	//!<general update the system
	bool StartGame();		//!<gives the flag to start the game
	bool BackToMain();		//!<return to the main menu check
	void Render();

	unsigned int GetPlayerCount() { return m_player_cnt; }

	int GetGameRules() { return m_game_mode.GetOption(); }
	int GetTurnLimit() { return m_roundcount.GetValue(); }
	int GetGoalScore() { return m_scorecount.GetValue(); }
	bool GetPrivEnabled(){ return m_priv_on.GetOption() == 1; }
	int GetPrivLimitVal(){ return m_priv_lim_on.GetOption(); } // enabled privacy starts from index 1 and is equal to 1
	bool GetPrivLimEnabled(){return m_priv_lim_on.GetOption() > 0; }

	PlayerSetup::list_t & getPlayers() { 
		return m_psetup_list;
	}

	SETUP_MENU_STATE m_state;	//!<state of the menu
	PieceMenu m_piece_menu;		//!<menu for selecting pieces
	

private:

	void ClearPlayerSetups();	//!<duh

	
	void MoveNext();			//!<
	void MoveBack();			//!<

	void ActivateGameSetupView();
	void DeactivateGameSetupView();

	void ActivatePlayerNameView();
	void DeactivatePlayerNameView();

	void ActivatePlayerInfoView();
	void DeactivatePlayerInfoView();

	void ActivatePlayerTokenView();
	void DeactivatePlayerTokenView();

	void ActivateMentorView();
	void DeactivateMentorView();

	void ActivateConfirmView();
	void DeactivateConfirmView();

private:
	PlayerSetup::list_t m_psetup_list; //!<vector of each players configurations
	PlayerSetup::list_t::iterator m_cur_setup; //!<iterator to the current player that's setting things up

	MentorMenu m_ment_menu;		//!<menu for selecting menus

	Answer	m_Next_but;			//!<button to continue on
	Answer	m_Back_but;			//!<button to go back

	unsigned int m_player_cnt;	//!<Number of players
	
	NumberMenu m_playercounter;	//!<specifies the player counter
	WordMenu m_game_mode;		//!<specifies the rules of winning
	NumberMenu m_roundcount;	//!<specifies limit on rounds
	NumberMenu m_scorecount;	//!<specifies limit on score;

	WordMenu m_priv_on;			//!<menu for selecting whether to have privacy on or not
	WordMenu m_priv_lim_on;		//!<menu for whether or not to limit the number of privacies to be used.

	PictureMenu* m_soundsetter;	//!<specifies the set they choose for their effects
	WordMenu m_gendersetter;	//!<specifies the gender of the user

	EditMenu m_playername;		//!<specifies player name

	ConfigConfirm* m_confirm;	//!<Confirmation dialogue

	TextBox m_menu_title;		//!<Menu title shown on top
	TextBox m_menu_footer;		//!<Menu footer show below
};


