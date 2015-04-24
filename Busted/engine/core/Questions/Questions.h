/*
	Questions.h
	Created by: Jamie Gault
	Created on 4/25/07

*/


#pragma once
#include "../all.h"
#include "../../tools/textdisplay/TextManager.h"
#include "../../tools/datadrive/datadrive.h"
#include "../glut/Input.h"

struct BtnTextureSet
	{
		GLuint m_idle; //!<texture to be displayed when there's nothing going on
		GLuint m_mouse_over; //!<texture to be displayed when there's a mouse over it
		GLuint m_use; //!<texture displayed when the answer is selected.
	};

enum BOX_TYPE{ BT_LINES, BT_SQUARE, BT_SQUARE_MIRROR };

//!basic box class for using with other classes for screen selection
class Box_2D
{
public:
	Box_2D();
	Box_2D( BOX_TYPE bt, float x, float y, float w, float h);
	Box_2D( unsigned int texture, float x, float y, float w, float h);
	Box_2D( unsigned int texture, float x, float y, float w, float h, unsigned int over_texture);
	Box_2D( Box_2D& box_2D);
	Box_2D& operator=(Box_2D& box_2D);

	bool IsMouseOver();

	void Render();

	float GetPosX(){ return m_pos[0]; }
	float GetPosY(){ return m_pos[1]; }
	float GetWidth(){ return m_dim[0]; }
	float GetHeight(){ return m_dim[1]; }
	BOX_TYPE GetBoxType(){ return m_boxtype; }
	GLuint GetBackGround(){ return m_textureset.m_idle; }
	GLuint GetAltBackGround(){ return m_textureset.m_mouse_over; }

	void SetBackGround(GLuint i){ m_textureset.m_idle = i; UseBG();}
	void SetAltBackGround(GLuint i){ m_textureset.m_mouse_over = i; UseAltBG(); }

	void UseBG(void) { m_textureset.m_use = m_textureset.m_idle; }
	void UseAltBG(void) { m_textureset.m_use = m_textureset.m_mouse_over; }

	void SetPosX(float x){ m_pos[0] = x; }
	void SetPosY(float y){ m_pos[1] = y; }

	float angle;

	bool use_maintxt; //!<bool simply set for determining whether to use the alternate texture or not

private:
	BOX_TYPE m_boxtype; //!<type of box to be rendered
	float m_pos[2]; //!<position of the box
	float m_dim[2]; //!<dimensions of the box	

	//!texture sets to be used as backgrounds
	BtnTextureSet m_textureset;
};




///////////////////////////////////////////////////////////////////////////////
/*!
Answer class
	-class for handling user answers, controlled by Question class

*/
///////////////////////////////////////////////////////////////////////////////
class Answer
{
public:
	Answer();
	Answer( Text_Manager *tx_m, std::string &s, float x0, float y0, float width ); 
	Answer( Answer& a );
	Answer( TextBox& t );
	Answer& operator=( Answer& a);

	bool IsMouseOver();

	TextBox& GetTextBox();

	void Update(float t);

	void Render();
	void SetBGs(GLuint idle, GLuint mo, GLuint click );

	bool selected; //!<used to mark that the answer is select in some other way

	//!texture sets to be used as backgrounds
	struct TextureSet
	{
		GLuint m_idle; //!<texture to be displayed when there's nothing going on
		GLuint m_mouse_over; //!<texture to be displayed when there's a mouse over it
		GLuint m_selected; //!<texture displayed when the answer is selected.
	}m_textureset;

	


private:
	TextBox m_textbox;

	float m_press_delay; //!<delay counter for when the button is pressed.

	
};



///////////////////////////////////////////////////////////////////////////////
/*!
DropDownMenu class
	-class for giving users a drop down menu

*/
///////////////////////////////////////////////////////////////////////////////
class DropDownMenu
{
public:
	DropDownMenu(Text_Manager *tx_m, int player_id, std::string player_name, float x, float y);
	~DropDownMenu();

	void Update(float t); //!<check whether to set the question to active
	void Render();

	bool IsActive();
	
	void SetScore( int s );
	int GetScore(); 

	void SetPos(float x, float y);
	float GetPosX(){ return user_id_box->GetPosX(); }
	float GetPosY(){ return user_id_box->GetPosY(); }

	float GetWidth(){ return ( user_id_box->GetWidth());}

private:
	void SetActivity(bool act);


	bool m_active; //!<bool saying whether or not the menu has dropped down
	int m_score_given;
	int m_player_num;
	std::vector<Answer*> m_score_menu; //!<boxes that can be used to keep score
	Box_2D* user_id_box;
	Box_2D	frame;
	

};




class player; //!declaration for use of player vectors to be passed to the question constructor


//!struct for managing what happens with privacy
class Privacy
{
public:
	Privacy(){ 	m_enable = false; m_limit_value = 0; m_limited = false; }
	void operator=(Privacy& priv){m_enable = priv.m_enable; m_limit_value = priv.m_limit_value; m_limited = priv.m_limited;}

	bool m_enable; //!<privacy is an available option
	bool m_limited; //!<if privacy is limited
	int m_limit_value; //!<how many privacy calls can be used
};


const int NO_ANSWER = -1;
const int ANSWER_PRIVACY = -2; //value associated with privacy for an answer


enum QuestionType { MULTIPLE_CHOICE, TRUE_FALSE, OPEN_ENDED};

///////////////////////////////////////////////////////////////////////////////
/*!
Question class
	-class for managing questions and giving answers based on text given

*/
///////////////////////////////////////////////////////////////////////////////
class Question
{
public:
	Question( unsigned int question_id,
				QuestionType qt,  //!<the type of question, ie multiple choice, true false, etc.
				std::string category,
				std::string question, //!<the question in string form
				std::vector<std::string> answers, //!<the answer strings
				std::vector<int> answer_ids,
				std::vector< player* > *player_vec = NULL,
				int player_id = 0, int playercnt = 1); //!<the texture ID for the background

	~Question();

	void Update(float t);

	int PollForAnswer(); //!<returns the answer id of the selected question

	int GetAnswerIndex(); //!<returns the answer index of the last selected question

	void AssignResults( std::string results, int score ); //assign the results from the given answer

	void Render(); 

	unsigned int GetQuestionID() { return m_question_id; }
	QuestionType GetQuestionType(){ return m_qtype; }
	int GetScore() { return m_result_score; }
	int GetAnswer(){ if(m_answered) return m_answer_index; return NO_ANSWER; }

	void GetOpenEndedScore( int score[4] );

	void SetBackground(GLuint bg) { m_background = bg; }


	void SetPrivacySettings(Privacy privsetup, int player_priv_cnt );

	std::string m_category;


private:
	unsigned int m_question_id;
	Box_2D m_identifier; //!<label for the question based on the catagory of the question
	TextBox m_questionbox; //!<box where the queston appears
	TextBox m_resultsbox; //!<box where the results appears
	std::vector<Answer*> m_answers; //!<box where the answers appear.
	std::vector<Answer*> m_answer_buts; //!<buttons that correspond to the answers
	std::vector<int> m_answer_IDs; //!<ids corresponding to the answer strings
	bool m_answered; //!<returns true if the question was answered
	QuestionType m_qtype; //!<the type of question being asked
	int m_answer_index; //!<index into the table for the answer chosen
	int m_result_score; //!<points earned for the answer

	GLuint m_background; //!<texture id for the background
	float m_backblend; //!<value for blending in the background

	std::vector<DropDownMenu*> m_ddmenu_vec;

	float trans_anim; //!<variable to account for the transition animation between the question and the result

	//!displays for privacy
	Answer m_priv_but; //!<privacy button to override the question
	TextBox m_priv_lim_display; //!<box for display how many privacy requests they have left


	Privacy m_privacy; //The settings for privacy selected in menu
};