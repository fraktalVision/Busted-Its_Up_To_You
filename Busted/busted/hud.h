/*
	hud.h
	Created by: Jamie Gault
	Created on 5/23/07

*/


#pragma once

#include "busted.all.h"
#include "../engine/core/stat_manager/stat_manager.h"
#include "../engine/core/Questions/Questions.h"
#include "Menu.h"
#include "player/player.h"


enum DIE_SIDE{ LEFT_DIE = 0, RIGHT_DIE };
enum ROLL_STATE{ ROLL_IDLE, ROLL_OUT, ROLL_IN, ROLL_STOP };



//!enum for winning rule
enum WIN_RULE{  ROUND_LIMIT, FIRST_TO_LIMIT, BOTH_LIMIT };

//!structure for holding winning rules and parameters
struct WinData
{
	std::vector<player*> winner;
	int goal_score;
	unsigned int turn;
	unsigned int turn_limit;

	WIN_RULE rule;
};

//!Class for handling 2D and 3D dice displaying as well as calculating the roll.
class DiceDisplay
{
public:

	DiceDisplay(bool two_d,  //!<whether to render a 2D or 3D dice
				float x = 375,  //!<x position on screen for 2d dice rolls
				float y = 550); //!<y position on screen for 2d dice rolls


	void Update(float t);
	void Render2D();
	void Render3D();

	void SetToRoll();
	void RollDice();

	int  GetTotalValue(){ return m_display_values[0] + m_display_values[1]; }
	void GetDisplayValues( int &first, int &second );

	bool ResultsShown() { return !m_rolling; }

	ROLL_STATE GetRollState() { return m_roll_state;}

	void IncrementStep(); //!<Mark another spot on the dice in increment.

	void SetPosX(float x) {
		screen_pos[0] = x;
	}
	float GetPosX() {
		return screen_pos[0];
	}

	void SetPosY(float y) {
		screen_pos[1] = y;
	}
	float GetPosY() {
		return screen_pos[1];
	}

	const bool m_3D; //!<whether the dice is to be rendered in 2 or 3D

private:

	void PickNewNumbers();
	GLuint GetMarkerValue(int dieside, int cur_count );

	bool m_rolling; //!<if the dice are still spinning
	GLuint m_dice_textures[6];

	int m_display_values[2];

	float screen_pos[2];

	float m_updatedelay; //!< Delay counter that slows down the speed of a roll

	int m_total_steps; //!<total number of steps that the player is taking
	int m_cur_step; //!<current number of steps taken

	std::vector<unsigned int> m_marker_vec; //!<vector of all the marker slides that have been added


	//added for 3D dice
	void RenderDie3D( DIE_SIDE s );
	void Render3DSide( int val );
	void SetDie3DOrient( DIE_SIDE side );
	float m_orig_ang[2][3]; //!<the angle of orientation used as a basis for each particular dice 
	float m_delt_ang[2][3]; //!<angular rotation collected over time used for animating the dice 
	ROLL_STATE m_roll_state; //!<state of the dice

};



//!Object for running and managing which player goes first
class PlayOrderRoll
{
public:
	PlayOrderRoll();
	~PlayOrderRoll();

	void Update(float t);
	void Render();

	void Load(PlayerSetup::list_t & players);

	PlayerSetup::list_t & getOrderedPlayers() {
		return m_players;
	}

	bool IsComplete; //!<is true if the player order has been set
	
private:
	bool DeterminePlayerOrder(); //!<calculates the order of everyone and determines whether rerolls need to occur.

	PlayerSetup::list_t			m_players;
	int							m_roll_count; //!<count for how many users have rolled;
	TextBox						m_title; //!<window title
	TextBox						m_footer; //!<window footer
	std::vector<DiceDisplay*>	m_dicerolls;//!<the dice for each player that is to be rolled to determine order.
	std::vector<TextBox*>		m_titlebox; //!<title of each user corresponding to the dice and stop button.
	std::vector<TextBox*>		m_orderbox; //!<order number for every player
	std::vector<Box_2D*>		m_tok_roll_icons; //!<corresponding player icons
	std::vector<Box_2D*>		m_tok_results_icons; //!<corresponding player icons
	std::vector<Answer*>		m_stopbut_vec; //!<a list of each player's name in a box to be rendered to the screen
	std::vector<int>			m_values_taken; //!<list of all values rolled to decide when a reroll needs to occur (prevent ties from happening)
	
	std::vector<float>			m_animation_speed;//!<speed of animation when reordering players
	
	bool ol_loaded; //!<bool for if the order list has already been loaded
	bool results_shown; //!<bool for if the results have been found


	float GetInterp(){ return interp_cntr/INTERPMAX;}

	float interp_cntr; //!<counter for how long a delay the game should take
	const float INTERPMAX;

};


//!this class manages the active score system that is always displayed at the bottom of the screen in game
class ScoreDisplay
{
public:
	ScoreDisplay(std::vector< player* > &playerlist );
	~ScoreDisplay();

	void Update(float t);
	void Render();

	void AddPoints(int player_id, int points); //!<gives the cue to play an animation for the number of points given
	void Reset();

	int active_player;

private:

	std::vector<int> score; //!<score displayed to the screen
	std::vector<TextBox*> score_boxes; //!<all the boxes used for scores.
	std::vector<Box_2D*> icon_boxes; //!<positions of all the icons

	Box_2D turnHL;


	//stuff used for animating the score
	bool Anim_on; //!<bool for whether or not the animation is on
	int anim_score; //!<score displayed in the animation when a person scores.
	int anim_pl_id; //!<id of the person that scored
	Box_2D anim_box; //!<used to have the animated score when a person gets points
	float accum_time; //!<the amount of time accumulated over a given frame, used for interpolation animation of the score animation

};






//!class for displaying results at the end of the game
class EndDisplay
{
public:
	EndDisplay( WinData & windata );
	~EndDisplay();

	void Update(float t);
	void Render();	


private:

	std::vector<Box_2D*> tok_ics; //!vector of the token icons that won
	std::vector<TextBox*> txtbox_vec; //!vector of all the text boxes created for this display
};