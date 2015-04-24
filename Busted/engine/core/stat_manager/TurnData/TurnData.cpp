#include "./TurnData.h"


#include "../../Questions/Questions.h"


extern enum QuestionType;






void TimeCalc( int &time, int &minutes, int &seconds)
{
	minutes = 0;
	seconds = 0;

	//subtract out minutes
	while( time > 60000 )
	{
		time -= 60000;
		++minutes;
	}

	//subtract out seconds
	while( time > 1000 )
	{
		time -= 1000;
		++seconds;
	}
}

/***********************************
TurnDump Constructor

Author: Jamie Gault
***********************************/
TurnDump::TurnDump( std::string filename ): m_start_time(0)
{
	CreateNewFile(filename);

	//set up current data
	currentData.Turn = 1;
	currentData.playerID = 1;
	currentData.roll = 0;
	currentData.question = 0;
	currentData.answer = 0;
	currentData.result = 0;
	currentData.space_id = std::string("None");
	currentData.q_type = MULTIPLE_CHOICE;
	currentData.openend_score[0] = 0;
	currentData.openend_score[1] = 0;
	currentData.openend_score[2] = 0;
	currentData.openend_score[3] = 0;
	currentData.score= 0;
	currentData.turn_time = 0;


}


/***********************************
TurnDump Destructor

Author: Jamie Gault
***********************************/
TurnDump::~TurnDump()
{}


/***********************************
TurnDump SubmitTurnData

Author: Jamie Gault
***********************************/
void TurnDump::SubmitTurnData()
{
	currentData.turn_time = GetTickCount();

	m_turndata_vec.push_back( currentData );

	//reset all the data
	currentData.playerID = 0;
	currentData.roll = 0;
	currentData.question = 0;
	currentData.answer = 0;
	currentData.result = 0;
	currentData.space_id = std::string("None");
	currentData.openend_score[0] = 0;
	currentData.openend_score[1] = 0;
	currentData.openend_score[2] = 0;
	currentData.openend_score[3] = 0;
	currentData.score= 0;
}





/***********************************
TurnDump DumpData

Author: Jamie Gault
***********************************/
void TurnDump::DumpData()
{
	//set it for writing
	m_file.open(m_filename.c_str(), std::ios::trunc);


	int time = GetTickCount() - m_start_time;

	int minutes = 0;
	int seconds = 0;

	//calculate time
	TimeCalc( time, minutes, seconds );


	//write out time
	m_file << " Time in-game: " << minutes << ":" << seconds << ":" << time << std::endl << std::endl;

	int begintime = m_start_time;

		//Iterate over the map, writing to the file-stream
	for(std::vector<TurnData>::iterator td = m_turndata_vec.begin(); td != m_turndata_vec.end(); ++td )
	{

		time = (*td).turn_time - begintime; 
		//calculate time
		TimeCalc( time, minutes, seconds );


		m_file << " Turn: "<< (*td).Turn << " " <<
				  " Player: "<< (*td).playerID << " " <<
				  " Roll: " << (*td).roll << " " <<
				  " Space: " << (*td).space_id << " ";
		
		switch( (*td).q_type )
		{
		case MULTIPLE_CHOICE:
			m_file << " Multiple-Choice ";
			break;
		case TRUE_FALSE: 
			m_file << " True-False ";
			break;
		case OPEN_ENDED:
			m_file << " Open-Ended ";
			break;

		}
		
		m_file << " Question: " << (*td).question << " "<<
				  " Answer: "<< (*td).answer << " "<<
				  " Result: "<< (*td).result << 
				  " Score: "<< (*td).score << std::endl; 

		//if the question was open ended
		if( (*td).q_type == OPEN_ENDED )
		{
			m_file << " Open Ended ";
			

			for( int i = 0; i < player_cnt-1 ; ++i )
			{
				//if( i != (*td).playerID )
				m_file << " score: "<<(*td).openend_score[i] ;
			}

			m_file << std::endl; 
										
		}
		

		m_file << " Turn Time: " << minutes << ":" << seconds << ":" << time << std::endl<< std::endl; 

		begintime = (*td).turn_time;
	}

	//Close the file
	m_file.close();
}

/***********************************
TurnDump CreateNewFile

Author: Jamie Gault
***********************************/
void TurnDump::CreateNewFile(std::string filename)
{
	std::stringstream num;

	num << GetTickCount();

	m_filename = filename + num.str() + std::string(".txt");
}
