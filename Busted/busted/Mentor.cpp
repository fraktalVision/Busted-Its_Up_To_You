#include "Mentor.h"

Mentor_Manager* pMentor_man;
extern Texture_Manager* pTexture_man;

/***********************************
Mentor Constructor

Author: Jamie Gault
***********************************/
Mentor::Mentor(MENTOR_ID id)
{
	switch(id)
	{
	case BLACK_GUY:
		pickMe = "AfM01";
		thanks = "AfM02";
		break;
	case BLACK_GIRL:
		pickMe = "AfG01";
		thanks = "AfG02";
		break;
	case ASIAN_GUY:
		pickMe = "AM01";
		thanks = "AM02";
		break;
	case ASIAN_GIRL:
		pickMe = "AG01";
		thanks = "AG02";
		break;
	case WHITE_GUY:
		pickMe = "WM01";
		thanks = "WM02";
		break;
	case WHITE_GIRL:
		pickMe = "WG01";
		thanks = "WG02";
		break;
	case INDIAN_GUY:
		pickMe = "LM01";
		thanks = "LM02";
		break;
	case LATIN_GIRL:
		pickMe = "LG01";
		thanks = "LG02";
		break;
	case LATIN_GUY:
		pickMe = "IM01";
		thanks = "IM02";
		break;
	case INDIAN_GIRL:
		pickMe = "IG01";
		thanks = "IG02";
		break;
	default:
		break;
	}
	LoadMentor( id );
}

/***********************************
Mentor Constructor

Author: Jamie Gault
***********************************/
Mentor::Mentor(Mentor& mentor) : dbID(-1)
{
	LoadMentor( mentor.GetMentorID() );
}

/***********************************
Mentor Deconstructor

Author: Jamie Gault
***********************************/
Mentor::~Mentor()
{
	glDeleteTextures( 4,  m_emo_pic );
}

/***********************************
Mentor LoadMentor

Author: Jamie Gault
***********************************/
void Mentor::LoadMentor(MENTOR_ID id)
{
	m_id = id;

	switch( m_id )
	{
	case BLACK_GUY:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenAfriBR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenAfriBH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenAfriBS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenAfriBP");
		break;

	case BLACK_GIRL:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenAfriGR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenAfriGH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenAfriGS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenAfriGP");
		break;

	case ASIAN_GUY:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenAsiaBR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenAsiaBH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenAsiaBS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenAsiaBP");
		break;

	case ASIAN_GIRL:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenAsiaGR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenAsiaGH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenAsiaGS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenAsiaGP");
		break;

	case INDIAN_GUY:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenIndiBR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenIndiBH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenIndiBS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenIndiBP");
		break;

	case INDIAN_GIRL:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenIndiGR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenIndiGH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenIndiGS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenIndiGP");
		break;

	case LATIN_GUY:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenLatiBR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenLatiBH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenLatiBS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenLatiBP");
		break;

	case LATIN_GIRL:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenLatiGR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenLatiGH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenLatiGS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenLatiGP");
		break;

	case WHITE_GUY:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenWhitBR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenWhitBH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenWhitBS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenWhitBP");
		break;

	default:
	case WHITE_GIRL:
		m_emo_pic[NORMAL] = pTexture_man->GetTexture("MenWhitGR");
		m_emo_pic[HAPPY] = pTexture_man->GetTexture("MenWhitGH");
		m_emo_pic[SAD] = pTexture_man->GetTexture("MenWhitGS");
		m_emo_pic[CONFUSED] = pTexture_man->GetTexture("MenWhitGP");
		break;
	}
}

std::string Mentor::makeText(MORALITY choice, bool outcome, GENDER gen, int outcomeID)
{
	std::string mentorText;
	// if gender is male, avoid using female texts from database which are
	// marked 2, otherwise avoid male texts with mark 1
	int opSex = (gen == GEN_MALE ? 2 : 1);

	std::stringstream ss;
	ss	<< "SELECT * FROM `mentor` WHERE `outID` = " << outcomeID << " OR (`moral` = " << (int)choice << " AND `point` = "
		<< outcome << " AND `sex` <> " << opSex << " AND `outID` = 0);";

	/*if(DATA("debug")) { */std::cout << ss.str() << std::endl;// }

	sitDB->exec(ss.str());
	if(sitDB->rows.size() > 0)
	{
		int id = random(0,(int)sitDB->rows.size()-1);
		//this->dbID = atoi((*sitDB->rows[id])["id"].c_str());

		mentorText = (*sitDB->rows[id])["text"];

		if((*sitDB->rows[id])["speech"] != "" && (*sitDB->rows[id])["speech"] != "NULL")
		{
			if(DATA("debug")) { std::cout << "Speech: " << (*sitDB->rows[id])["speech"] << std::endl; }
			pMusic_man->play((*sitDB->rows[id])["speech"]);
		}
	}
	return mentorText;
}

/***********************************
Mentor GetMentorID

Author: Jamie Gault
***********************************/
MENTOR_ID Mentor::GetMentorID()
{
	return m_id;
}

/***********************************
Mentor GetEmoPic

Author: Jamie Gault
***********************************/
GLuint Mentor::GetEmoPic( int score )
{
	if( score > 2 )
		return GetEmoPic( HAPPY );
	else if( score > 0 )
		return GetEmoPic( NORMAL );
	else if( score > -1 )
		return GetEmoPic( CONFUSED );
	else
		return GetEmoPic( SAD );
}

/***********************************
Mentor GetEmoPic

Author: Jamie Gault
***********************************/
GLuint Mentor::GetEmoPic( MENTOR_EMOTIONS emo )
{
	return m_emo_pic[emo];
}

/***********************************
Mentor_Manager Constructor

Author: Jamie Gault
***********************************/
Mentor_Manager::Mentor_Manager()
{
	for( int i = 0; i < 10 ; ++i )
	{
		Mentor* men = new Mentor((MENTOR_ID)i);
		m_mentor_vec.push_back(men);
	}
}

/***********************************
Mentor_Manager Deconstructor

Author: Jamie Gault
***********************************/
Mentor_Manager::~Mentor_Manager()
{
	for(std::vector<Mentor*>::iterator i = m_mentor_vec.begin();
		i != m_mentor_vec.end(); ++i)
	{
		delete(*i);
	}
}

/***********************************
Mentor_Manager GetMentor

Author: Jamie Gault
***********************************/
Mentor* Mentor_Manager::GetMentor( MENTOR_ID men_id )
{
	for(std::vector<Mentor*>::iterator i = m_mentor_vec.begin();
	i != m_mentor_vec.end(); ++i)
	{
		if( (*i)->GetMentorID() == men_id )
			return (*i);
	}
	return NULL;
}