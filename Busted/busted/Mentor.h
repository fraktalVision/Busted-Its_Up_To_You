
#pragma once

#include "busted.all.h"

enum GENDER{ GEN_MALE, GEN_FEMALE };

enum MENTOR_ID{ BLACK_GUY, ASIAN_GIRL,
				LATIN_GUY, INDIAN_GIRL,
				WHITE_GUY, BLACK_GIRL,
				ASIAN_GUY, LATIN_GIRL,
				INDIAN_GUY,WHITE_GIRL, M_NONE};

enum MENTOR_EMOTIONS{ NORMAL, HAPPY, SAD, CONFUSED };

//!A class to control each mentor with
class Mentor
{
public:
	Mentor(MENTOR_ID id);						//!<Constructor based on mentor ID
	Mentor(Mentor& mentor);						//!<Copy-constructor
	~Mentor();									//!<Destructor

	std::string pickMe;							//!<The name of the audio byte to play when hovering over this mentor
	std::string thanks;							//!<The name of the audio byte to play when this mentor is selected

	void LoadMentor(MENTOR_ID id);				//!<Based on the ID load the textures of the mentor's expressions
	MENTOR_ID GetMentorID();					//!<Get the ID of this mentor

	GLuint GetEmoPic( int score );				//!<Get the expression of the mentor based on player score
	GLuint GetEmoPic( MENTOR_EMOTIONS emo );	//!<returns the texture ID of the emotion

	int		dbID;								//!<ID of the mentor in the SQL database
	std::string	makeText(MORALITY, bool, GENDER, int);	//!<Get what to say from the SQL database
	void	drawText(Text_Manager *);			//!<Draw the text box

private:
	MENTOR_ID m_id;								//!<id for what character they are
	GLuint m_emo_pic[4];						//!<ID's for the textures loaded
	
};


//!An class to manage all mentors
class Mentor_Manager
{
public:
	Mentor_Manager();
	~Mentor_Manager();

	Mentor* GetMentor( MENTOR_ID men_id );		//!<Return the requested mentor in all player mentors.  If non-existent: returns NULL

private:
	std::vector<Mentor*> m_mentor_vec;			//!<Vector of all the mentors playing
};

