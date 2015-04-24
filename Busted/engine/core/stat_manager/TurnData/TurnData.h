#include "../../all.h"


#ifndef _TURN_DATA_H_
#define _TURN_DATA_H_


extern enum QuestionType;

//calculate minutes, seconds and milliseconds given only milliseconds
void TimeCalc( int &time, int &minutes, int &seconds );

//struct for containing the players data
struct TurnData
{
	int Turn;
	int playerID;
	int roll;
	std::string space_id;
	QuestionType q_type;
	int question;
	int answer;
	int result;
	int score;
	int openend_score[4];
	DWORD turn_time;

};

//class for recording player stats
class TurnDump
{
private:
	std::vector<TurnData> m_turndata_vec; // vector of all the data recorded for the game
	std::string m_filename; //name
	std::ofstream m_file;	 //handle
	DWORD m_start_time;


public:
	TurnDump( std::string filename );
	~TurnDump();

	
	void SubmitTurnData();
	void DumpData();
	void CreateNewFile(std::string filename);
	void StartTimer() { m_start_time = GetTickCount(); currentData.turn_time = GetTickCount(); }


	TurnData currentData; //data for the current turn
	int player_cnt; //number of players playing
};

#endif