/*
	Mainmenu.cpp
	Created by: Jamie Gault
	Created on 5/10/07

*/

#include "Mainmenu.h"
#include "busted.all.h"

extern Input *pInput;
extern Mentor_Manager *pMentor_man;
extern Texture_Manager* pTexture_man;

/***********************************
MainMenu Constructor

Author: Jamie Gault
***********************************/
MainMenu::MainMenu(Text_Manager* tm): m_men_res(MR_NONE), m_men_act(MA_BASE), m_optmenu(tm)
{
	m_title = Box_2D(BT_SQUARE,  DATA("MM_TITLE_POSX"), DATA("MM_TITLE_POSY"), DATA("MM_TITLE_WD"), DATA("MM_TITLE_HT"));//200.0f, 100.0f, 400.0f, 200.0f);
	m_title.SetBackGround( pTexture_man->GetTexture("title"));

	//load all the buttons to be used for the menu
	m_startBut = Answer(tm, std::string("Start Game"), DATA("MM_START_POSX"), DATA("MM_START_POSY"), 500.0f );
	m_Mod_Inst = Answer(tm, std::string("Moderator Instructions"), DATA("MM_MODIN_POSX"), DATA("MM_MODIN_POSY"), 500.0f );
	m_Pla_Inst = Answer(tm, std::string("Player Instructions"), DATA("MM_PLAIN_POSX"), DATA("MM_PLAIN_POSY"), 500.0f );
	m_options = Answer(tm, std::string("Options"), DATA("MM_OPTIONS_POSX"), DATA("MM_OPTIONS_POSY"), 500.0f );
	m_credits = Answer(tm, std::string("Credits"), DATA("MM_CREDITS_POSX"), DATA("MM_CREDITS_POSY"), 500.0f );
	m_quitBut = Answer(tm, std::string("Quit"), DATA("MM_QUIT_POSX"), DATA("MM_QUIT_POSY"), 500.0f );

	m_pla_slide_men = NumberMenu( tm, "Slide Number ", 1, 1, DATA("PLA_SLIDE_MAX"), 1, DATA("PLA_SLDMENU_POSX"), DATA("PLA_SLDMENU_POSY"), true );
	m_slide_men = NumberMenu( tm, "Slide Number ", 1, 1, DATA("MOD_SLIDE_MAX"), 1, DATA("MOD_SLDMENU_POSX"), DATA("MOD_SLDMENU_POSY"), true );

	m_exit_mod_but = Answer(tm, std::string("Back to the Main Menu"), 600.0f, 600.0f, 500.0f );
}

/***********************************
MainMenu Update

Author: Jamie Gault
***********************************/
void MainMenu::Update(float t)
{
	//update all the buttons
	m_startBut.Update(t);
	m_quitBut.Update(t);
	m_options.Update(t);
	m_Mod_Inst.Update(t);
	m_Pla_Inst.Update(t);  //<!DDDD added
	m_credits.Update(t);
	m_exit_mod_but.Update(t);

	//if the menu is in the base set of actions
	if( m_men_act == MA_BASE )
	{
		//if the left buttons was pressed
		if( pInput->PollMouseBut(MLEFT) )
		{
			//check which button was pressed
			bool butpress = false;
			if( m_startBut.IsMouseOver() )
			{
				m_men_res = MR_START;
				butpress = true;
			}
			else if( m_quitBut.IsMouseOver() )
			{
				butpress = true;
				//kill the program

				PostQuitMessage(0);
			}
			else if( m_options.IsMouseOver() )
			{
				butpress = true;
				m_men_act = MA_OPTIONS;
			}
			else if( m_Mod_Inst.IsMouseOver() )
			{
				m_men_act = MA_MODERATOR;
				LoadModInst();
				butpress = true;
			}
			else if ( m_Pla_Inst.IsMouseOver() )
			{
				m_men_act = MA_PLAYER;
				LoadPlaInst();  //DDDD load player instructions
				butpress = true;
			}
			else if( m_credits.IsMouseOver() )
			{
				m_men_act = MA_CREDITS;
				m_cred_dis = new CreditsDisplay();
				butpress = true;
			}

			if(butpress)
			{
				pMusic_man->play("BTN_C");
			}
		}
	}
	else if( m_men_act == MA_OPTIONS )
	{
		m_optmenu.Update( t );

		if( m_optmenu.GetMenuResult() == MR_RETURN )
		{
			m_men_act = MA_BASE;
			m_optmenu.Reset();
			pMusic_man->play("BTN_C");
		}
	}
	else if( m_men_act == MA_MODERATOR )
	{
		m_slide_men.Update();

		//the user selects tto go back
		if( pInput->PollMouseBut(MLEFT ) )
		{
			if( m_exit_mod_but.IsMouseOver() )
			{
				m_men_act = MA_BASE;
				m_slide_men.SetValue( 1 );
				UnloadModInst();
				pMusic_man->play("BTN_C");
			}
		}
	}
	else if( m_men_act == MA_PLAYER )
	{
		m_pla_slide_men.Update();

		//the user selects tto go back
		if( pInput->PollMouseBut(MLEFT ) )
		{
			if( m_exit_mod_but.IsMouseOver() )
			{
				m_men_act = MA_BASE;
				m_pla_slide_men.SetValue( 1 );
				UnloadPlaInst();
				pMusic_man->play("BTN_C");
			}
		}
	}
	else if( m_men_act == MA_CREDITS )
	{
		m_cred_dis->Update(t);

		//the user selects to go back or the credits finish
		if( pInput->PollMouseBut(MLEFT ) || m_cred_dis->IsFinished())
		{
			m_men_act = MA_BASE;
			delete m_cred_dis;
			pMusic_man->play("BTN_C");
		}
	}
}

/***********************************
MainMenu Render

Author: Jamie Gault
***********************************/
void MainMenu::Render()
{
	if( m_men_act == MA_BASE )
	{
		m_title.Render();
		m_startBut.Render();
		m_quitBut.Render();
		m_options.Render();
		m_Mod_Inst.Render();
		m_Pla_Inst.Render();
		m_credits.Render();
	}
	else if( m_men_act == MA_OPTIONS )
	{
		m_optmenu.Render();
	}
	else if( m_men_act == MA_MODERATOR )
	{
			//drop a gel on the screen
		RenderScreenGel( 0.0f, 0.0f, 1.0f,0.5f);

		glColor3f(1.0f, 1.0f, 1.0f);
		//render the frame background
		RenderCoordScreenImage( pTexture_man->GetTexture(std::string("MENU_BACKGROUND")),
						50.0f*SCREEN_X_RATIO,
						50.0f*SCREEN_Y_RATIO,
						(DATA("SCREEN_WIDTH") -50.0f),
						(DATA("SCREEN_HEIGHT") -50.0f));

		//render the moderator screen data
		RenderCoordScreenImage( m_mod_slides[m_slide_men.GetValue()-1],
						150.0f*SCREEN_X_RATIO,
						50.0f*SCREEN_Y_RATIO,
						(DATA("SCREEN_WIDTH") -150.0f)*SCREEN_X_RATIO,
						(DATA("SCREEN_HEIGHT") -100.0f)*SCREEN_Y_RATIO);
		m_slide_men.Render();
		m_exit_mod_but.Render();
	}
	else if( m_men_act == MA_PLAYER )
	{
			//drop a gel on the screen
		RenderScreenGel( 0.0f, 0.0f, 1.0f,0.5f);

		glColor3f(1.0f, 1.0f, 1.0f);
		//render the frame background
		RenderCoordScreenImage( pTexture_man->GetTexture(std::string("MENU_BACKGROUND")),
						50.0f*SCREEN_X_RATIO,
						50.0f*SCREEN_Y_RATIO,
						(DATA("SCREEN_WIDTH") -50.0f),
						(DATA("SCREEN_HEIGHT") -50.0f));

		//render the moderator screen data
		RenderCoordScreenImage( m_pla_slides[m_pla_slide_men.GetValue()-1],
							150.0f*SCREEN_X_RATIO,
							50.0f*SCREEN_Y_RATIO,
							(DATA("SCREEN_WIDTH") -150.0f)*SCREEN_X_RATIO,
							(DATA("SCREEN_HEIGHT") -100.0f)*SCREEN_Y_RATIO);

		m_pla_slide_men.Render();
		m_exit_mod_but.Render();
	}
	else if( m_men_act == MA_CREDITS )
	{
		m_cred_dis->Render();
	}
}

/***********************************
MainMenu GetMenuResult

Author: Jamie Gault
***********************************/
MENU_RESULT MainMenu::GetMenuResult()
{
	return m_men_res;
}

/***********************************
PauseMainMenuMenu LoadModInst

Author: Jamie Gault
***********************************/
void MainMenu::LoadModInst()
{
	for( int i = 1; i <= DATA("MOD_SLIDE_MAX"); ++i)
	{
		std::stringstream id;
		id << i;

		pTexture_man->SetupTexture( std::string("MOD_SLIDE_") + id.str() );
		m_mod_slides.push_back( pTexture_man->GetTexture( std::string("MOD_SLIDE_") + id.str() ) );
	}
}

/***********************************
PauseMainMenuMenu LoadModInst

Author: Derek Dunnom
***********************************/
void MainMenu::LoadPlaInst()
{
	for( int i = 1; i <= DATA("PLA_SLIDE_MAX"); ++i)
	{
		std::stringstream id;
		id << i;

		pTexture_man->SetupTexture( std::string("PLA_SLIDE_") + id.str() );
		m_pla_slides.push_back( pTexture_man->GetTexture( std::string("PLA_SLIDE_") + id.str() ) );
	}
}

/***********************************
PauseMenu UnloadModInst

Author: Jamie Gault
***********************************/
void MainMenu::UnloadModInst()
{
	for( int i = 1; i <= DATA("MOD_SLIDE_MAX"); ++i)
	{
		std::stringstream id;
		id << i;

		pTexture_man->UnloadTexture( std::string("MOD_SLIDE_") + id.str() );
	}
}

/***********************************
PauseMenu UnloadPlaInst

Author: Jamie Gault
***********************************/
void MainMenu::UnloadPlaInst()
{
	for( int i = 1; i <= DATA("PLA_SLIDE_MAX"); ++i)
	{
		std::stringstream id;
		id << i;

		pTexture_man->UnloadTexture( std::string("PLA_SLIDE_") + id.str() );
	}
}

/***********************************
PauseMenu Constructor

Author: Jamie Gault
***********************************/
PauseMenu::PauseMenu(Text_Manager* tm): m_men_res(MR_NONE), m_men_act( MA_BASE ), m_optmenu(tm)
{
	m_title = TextBox(tm, std::string(" P a u s e d ! "), 200.0f*SCREEN_X_RATIO, 100.0f*SCREEN_Y_RATIO, 1000.0f, 30.0f, 50.0f, 50.0f );
	m_resumeBut = Answer(tm, std::string("Resume Game"), 400.0f*SCREEN_X_RATIO, 400.0f*SCREEN_Y_RATIO, 500.0f );
	m_optionsBut = Answer(tm, std::string("Options"), 400.0f*SCREEN_X_RATIO, 500.0f*SCREEN_Y_RATIO, 500.0f );
	m_quitBut = Answer(tm, std::string("Back To Main Menu"), 400.0f*SCREEN_X_RATIO, 600.0f*SCREEN_Y_RATIO, 500.0f );

	m_yes = Answer(tm, std::string("Yes"), 350.0f*SCREEN_X_RATIO, 500.0f*SCREEN_Y_RATIO, 500.0f );
	m_no = Answer(tm, std::string("No"), 600.0f*SCREEN_X_RATIO, 500.0f*SCREEN_Y_RATIO, 500.0f );

}

/***********************************
MainMenu Update

Author: Jamie Gault
***********************************/
void PauseMenu::Update(float t)
{
	m_resumeBut.Update(t);
	m_optionsBut.Update(t);
	m_quitBut.Update(t);
	m_yes.Update(t);
	m_no.Update(t);

	if( m_men_act == MA_BASE )
	{
		if( pInput->PollMouseBut(MLEFT) )
		{
			bool butpress = false;

			if( m_resumeBut.IsMouseOver() )
			{
				m_men_res = MR_RESUME;
				butpress = true;
			}
			else if( m_quitBut.IsMouseOver() )
			{
				//kill the program

				m_men_act = MA_QUIT;
				butpress = true;
			}
			else if( m_optionsBut.IsMouseOver() )
			{
				m_men_act = MA_OPTIONS;
				butpress = true;
			}

			if(butpress)
			{
				pMusic_man->play("BTN_C");
			}
		}
	}
	else if( m_men_act == MA_OPTIONS )
	{
		m_optmenu.Update( t );

		if( pInput->PollMouseBut(MLEFT) )
		{
			if( m_optmenu.GetMenuResult() == MR_RETURN )
			{
				m_men_act = MA_BASE;
				m_optmenu.Reset();
				pMusic_man->play("BTN_C");
			}
		}
	}
	else if( m_men_act == MA_QUIT )
	{
		if( pInput->PollMouseBut(MLEFT) )
		{
			//check if the user opts to quit the game
			if( m_yes.IsMouseOver() )
			{
				m_men_res = MR_EXITGAME;
				pMusic_man->play("BTN_C");
			}
			else if( m_no.IsMouseOver() )
			{
				m_men_act = MA_BASE;
				pMusic_man->play("BTN_C");
			}
		}
	}
}

/***********************************
PauseMenu Render

Author: Jamie Gault
***********************************/
void PauseMenu::Render()
{
	m_title.Render();

	if( m_men_act == MA_BASE )
	{
		m_resumeBut.Render();
		m_optionsBut.Render();
		m_quitBut.Render();
	}
	else if( m_men_act == MA_OPTIONS )
	{
		m_optmenu.Render();
	}
	else if( m_men_act == MA_QUIT )
	{
		TextBox sure = TextBox(m_yes.GetTextBox().GetFontPtr(), std::string("Are you sure you want to quit "),
						150.0f*SCREEN_X_RATIO, 300.0f, 1000.0f, 10.0f*SCREEN_X_RATIO, 30.0f*SCREEN_X_RATIO, 30.0f*SCREEN_Y_RATIO );
		sure.SetBackground(m_resumeBut.m_textureset.m_idle);
		sure.Render();

		m_yes.Render();
		m_no.Render();
	}
}

/***********************************
PauseMenu GetMenuResult

Author: Jamie Gault
***********************************/
MENU_RESULT PauseMenu::GetMenuResult()
{
	return m_men_res;
}

/***********************************
GenOptionsMenu Constructor

Author: Jamie Gault
***********************************/
GenOptionsMenu::GenOptionsMenu(Text_Manager* tm):  m_men_res(MR_NONE),m_men_act(MA_BASE)
{
	m_title = TextBox(tm, std::string(" O P T I O N S "), 200.0f*SCREEN_X_RATIO, 100.0f*SCREEN_Y_RATIO, 1000.0f, 30.0f, 50.0f, 50.0f );

	std::list<std::string> texture_op;

	texture_op.push_back( std::string("Low"));
	texture_op.push_back( std::string("Medium"));
	texture_op.push_back( std::string("High"));

	m_TextureDetail = WordMenu( tm, "Texture Detail:", texture_op, 300.0f, 200.0f );
 	m_MusicVol_men = NumberMenu( tm, "Music Volume:", pMusic_man->musVolume*10, 0, 10, 1, 300.0f, 330.0f );
	m_SoundVol_men = NumberMenu( tm, "Sound Volume:", pMusic_man->sndVolume*10, 0, 10, 1, 300.0f, 470.0f );

	m_TextureDetail.SetOption( pTexture_man->GetLevel() );

	m_back_but = Answer(tm, std::string("Back"), 400.0f*SCREEN_X_RATIO, 600.0f*SCREEN_Y_RATIO, 500.0f );
}

/***********************************
PauseMenu Update

Author: Jamie Gault
***********************************/
void GenOptionsMenu::Update(float t)
{
	m_back_but.Update(t);

	if( m_men_act == MA_BASE )
	{
		m_TextureDetail.Update();
		m_MusicVol_men.Update();
		m_SoundVol_men.Update();

		//the user selects tto go back
		if( pInput->PollMouseBut(MLEFT ) )
		{
			if( m_back_but.IsMouseOver() )
			{
				ApplyOptions();
				m_men_res = MR_RETURN;
				pMusic_man->play("BTN_C");
			}
		}
	}
}

/***********************************
PauseMenu Render

Author: Jamie Gault
***********************************/
void GenOptionsMenu::Render()
{
	RenderCoordScreenImage( pTexture_man->GetTexture(std::string("MENU_BACKGROUND")),
						50.0f*SCREEN_X_RATIO,
						50.0f*SCREEN_Y_RATIO,
						(DATA("SCREEN_WIDTH") -50.0f),
						(DATA("SCREEN_HEIGHT") -50.0f));

	if( m_men_act == MA_BASE )
	{
		m_title.Render();
		m_TextureDetail.Render();
		m_MusicVol_men.Render();
		m_SoundVol_men.Render();

		m_back_but.Render();
	}
}

/***********************************
PauseMenu Render

Author: Jamie Gault
***********************************/
void GenOptionsMenu::ApplyOptions()
{
	pTexture_man->SetLevel( m_TextureDetail.GetOption());

	pMusic_man->sndVolume = ((float)m_SoundVol_men.GetValue()/10.0f);
	pMusic_man->musVolume = ((float)m_MusicVol_men.GetValue()/10.0f);
}

/***********************************
PauseMenu GetMenuResult

Author: Jamie Gault
***********************************/
MENU_RESULT GenOptionsMenu::GetMenuResult()
{
	return m_men_res;
}

/***********************************
EndGameMenu Constructor

Author: Jamie Gault
***********************************/
EndGameMenu::EndGameMenu(Text_Manager* tm): m_men_res(MR_NONE)
{
	m_playagainBut = Answer(tm, std::string("Play again!"), 400.0f*SCREEN_X_RATIO, 500.0f*SCREEN_Y_RATIO, 500.0f );
	m_quitBut = Answer(tm, std::string("Exit"), 400.0f*SCREEN_X_RATIO, 600.0f*SCREEN_Y_RATIO, 500.0f );
}

/***********************************
EndGameMenu Update

Author: Jamie Gault
***********************************/
void EndGameMenu::Update(float t)
{
	m_playagainBut.Update(t);
	m_quitBut.Update(t);

	//the user selects tto go back
	if( pInput->PollMouseBut(MLEFT ) )
	{
		if( m_playagainBut.IsMouseOver() )
		{
			m_men_res = MR_RETRY;

			pMusic_man->play("BTN_C");
		}
		else if( m_quitBut.IsMouseOver() )
		{
			m_men_res = MR_EXITGAME;
			//playsond
		}
	}
}

/***********************************
EndGameMenu Render

Author: Jamie Gault
***********************************/
void EndGameMenu::Render()
{
	m_playagainBut.Render();
	m_quitBut.Render();
}

/***********************************
EndGameMenu GetMenuResult

Author: Jamie Gault
***********************************/
MENU_RESULT EndGameMenu::GetMenuResult()
{
	return m_men_res;
}

/***********************************
CreditsDisplay Constructor

Author: Jamie Gault
***********************************/
CreditsDisplay::CreditsDisplay()
{
	loaded = LoadCreditsFile();

	//load in all the characatures in sequence
	for( int i = 0; i < DATA("NUM_CHARAC_PICS"); ++i )
	{
		float alt = ( i%2 == 0 )? 1.0f: -1.0f;
		//the characatures each have an incremental id starting with 0 and going through DATA("NUM_CHARAC_PICS") - 1
		std::stringstream s; s << i;
		Box_2D* b = new Box_2D( pTexture_man->GetTexture(std::string("char")+s.str()), 350.0 + DATA("CHARAC_XOFFSET")*alt,
			DATA("CRED_START_HEIGHT") +((float)i)/ DATA("NUM_CHARAC_PICS")*m_YRange,DATA("CHARAC_WD"),DATA("CHARAC_HT") );

		charPics.push_back(b);
	}
}

/***********************************
CreditsDisplay Destructor

Author: Jamie Gault
***********************************/
CreditsDisplay::~CreditsDisplay()
{
	for(std::vector<Box_2D*>::iterator bi = charPics.begin(); bi != charPics.end(); ++bi)
		delete *bi;
}

/***********************************
CreditsDisplay Update

Author: Jamie Gault
***********************************/
void CreditsDisplay::Update(float t)
{
	//slowly increment the text towards the top of the screen
	for( std::vector<TextBox*>::iterator i = textboxlist.begin(); i != textboxlist.end() ; ++i )
	{
		(*i)->SetPosY((*i)->GetPosY() - 2.0f*t );
	}

	//slowly reposition the boxes towards the top of the screen
	for( std::vector<Box_2D*>::iterator bi = charPics.begin(); bi != charPics.end() ; ++bi )
	{
		(*bi)->SetPosY((*bi)->GetPosY() - 2.0f*t );
	}
}

/***********************************
CreditsDisplay Render

Author: Jamie Gault
***********************************/
void CreditsDisplay::Render()
{
	RenderScreenGel( 1.0f, 1.0f, 1.0f,0.5f);

	glColor4f(1.0f, 1.0f, 1.0f,0.8f);

	float win_height = DATA("SCREEN_HEIGHT");

	for( std::vector<Box_2D*>::iterator bi = charPics.begin(); bi != charPics.end() ; ++bi )
	{
		//if the textbox is on screen
		if((*bi)->GetPosY() + (*bi)->GetWidth() > -50.0f && (*bi)->GetPosY() < win_height )
		{
			(*bi)->Render();
		}
	}

	for( std::vector<TextBox*>::iterator i = textboxlist.begin(); i != textboxlist.end() ; ++i )
	{
		//if the textbox is on screen
		if((*i)->GetPosY() > -50.0f && (*i)->GetPosY() < win_height )
		{
			(*i)->Render();
		}
	}

}

/***********************************
CreditsDisplay IsFinished

Author: Jamie Gault
***********************************/
bool CreditsDisplay::IsFinished()
{
	if(  (textboxlist[num_tboxes-1])->GetPosY() < -50.0f )
		return true;
	return false;
}

const int MAX_CHAR_PER_LINE = 4096;

/***********************************
CreditsDisplay LoadCreditsFile

Author: Jamie Gault
***********************************/
bool CreditsDisplay::LoadCreditsFile()
{
	std::ifstream myfile("../Busted/assets/Credits/credits.xtx");

	float yinc = 0.0f;

	num_tboxes = 0;

	//if the file exists
	if(myfile.is_open())
	{
		//run through the entire file
		while (!myfile.eof())
		{
			char charPeekChar = static_cast<char>(myfile.peek());
			if(charPeekChar == '/')
			{
				//skip to the next line
				myfile.ignore(MAX_CHAR_PER_LINE, '\n');
			}
			else // line has stuff to read in
			{
				std::string strline;
				std::getline(myfile, strline);
				size_t firstQuote = 0;
				firstQuote = strline.find('"', firstQuote);

				// If empty line, the position returned is string::npos
				if(firstQuote  == std::string::npos)
				{
					continue; // go to next line
				}
				else // process line
				{
					// Get GroupTitle
					size_t secondQuote = strline.find('"', firstQuote + 1);
					std::string strtitle = strline.substr(firstQuote + 1, secondQuote - firstQuote - 1);

					TextBox* box = new TextBox(pText_man, strtitle, DATA("CRED_TITLE_POS_X"),
										DATA("CRED_START_HEIGHT")+yinc, 420.0f);
					box->SetBackground(0);
					textboxlist.push_back(box);
					yinc += textboxlist[num_tboxes]->GetBoxHeight();
					++num_tboxes;

					bool firsttime = true;

					//find additional paths for textures
					do
					{
						firstQuote = strline.find('"', secondQuote + 1);

						if( firstQuote != -1 )
						{
							//mark that this text block is for a list of names
							firsttime = false;

							//pull out the name to be used from the file string
							secondQuote = strline.find('"', firstQuote + 1);
							strtitle = strline.substr(firstQuote + 1, secondQuote - firstQuote - 1);

							//make a new textbox for the person's name
							TextBox* box = new TextBox(pText_man, strtitle, DATA("CRED_NAME_POS_X"), DATA("CRED_START_HEIGHT")+yinc, 750.0f);
							box->SetBackground(0); //set a transparent background
							textboxlist.push_back(box);

							//increment the y position for the next box to be below the box that was just made
							yinc += textboxlist[num_tboxes]->GetBoxHeight();
							++num_tboxes;
						}
					}while( firstQuote != -1);

					if(firsttime)
					{
						textboxlist[num_tboxes-1]->SetPosY( textboxlist[num_tboxes-1]->GetPosY()+ DATA("CRED_GAP_Y"));
						textboxlist[num_tboxes-1]->SetBoxWidth( textboxlist[num_tboxes-1]->GetBoxWidth()*DATA("CRED_TITLE_SCALE") );
						textboxlist[num_tboxes-1]->SetBoxHeight( textboxlist[num_tboxes-1]->GetBoxHeight()*DATA("CRED_TITLE_SCALE"));
						textboxlist[num_tboxes-1]->SetLetterWidth( textboxlist[num_tboxes-1]->GetLetterWidth()*DATA("CRED_TITLE_SCALE"));
						textboxlist[num_tboxes-1]->SetLetterHeight( textboxlist[num_tboxes-1]->GetLetterHeight()*DATA("CRED_TITLE_SCALE") );

						yinc += DATA("CRED_GAP_Y");
					}

					yinc += DATA("CRED_GAP_Y");
				}
			}
		}

		//save the start and end ranges
		m_YRange = yinc;
		myfile.close();

		return true;
	}

	return false;
}