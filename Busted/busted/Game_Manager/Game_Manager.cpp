#include "../busted.all.h"
#include "Game_Manager.h"




//Global to poll user-input
extern Input *pInput;
extern Mentor_Manager *pMentor_man;
extern Texture_Manager* pTexture_man;

/***********************************
Game_Manager Constructor

Author: Jamie Gault
***********************************/
Game_Manager::Game_Manager(): stat("../Busted/assets/stats/gameStats.txt"),
							turndump("../TurnDump/gameTurnDump"),//WTF is a turnDump?
							m_ig_state(IGS_DICE), 
							m_ig_mode(IGM_NONE),
							m_question(NULL),
							framerate(1.0f),
							framescale(1.0f),
							m_pSetupmenu(NULL),	
							m_pMainMenu(NULL),
							m_pPauseMenu(NULL),
							m_pEndMenu(NULL),
							m_pOrderRoll(NULL),
							m_enddisplay(NULL),
							m_cam_man( Camera ),
							m_discuss_fade(0.0f),
							turncount(0)
							
							
{

	//setup clock
	m_system_clock.oldtime = GetTickCount();
	m_system_clock.m_clock_speed = 1.0f;
	m_system_clock.m_delay_counter = 0.0f;

	m_win_data.goal_score = 5;
	m_win_data.turn = 1;
	m_win_data.turn_limit = 2;
	m_win_data.rule = BOTH_LIMIT;
	

	//load in a texture
	textboxdata.background = pTexture_man->GetTexture("TID_TEXTBG");
	textboxdata.text_m = new Text_Manager( std::string("../Busted/assets/font1.png")); //load in the font

	pText_man = textboxdata.text_m;

	m_ex_state = ES_PREGAME_LOAD;

	m_dicestop_but = Answer( pText_man, std::string("Stop!"), 
							DATA("STOPBUT_X"), DATA("STOPBUT_Y"), 500.0f); 

	m_continue_but = Answer( pText_man, std::string("Continue..."),
							DATA("CONTBUT_X"), DATA("CONTBUT_Y"), 500.0f);

	pModel_man->open();

	
	tabmenu = new TabMenu(&m_win_data);

	m_mentor_text = TextBox(pText_man, std::string(), 
							DATA("INSTMSG_POSX") + DATA("menttxt_x"), 
							DATA("INSTMSG_POSY") + DATA("menttxt_y"), DATA("menttxt_wid"), 
							DATA("menttxt_letSpc"), DATA("menttxt_letWd"), DATA("menttxt_letHt"));
	m_mentor_text.SetBackground(0);

}


/***********************************
Game_Manager Deconstructor

Author: Jamie Gault
***********************************/
Game_Manager::~Game_Manager()
{
	EndGameCleanUp();
	delete textboxdata.text_m;

	pModel_man->close();
}


/***********************************
Game_Manager Deconstructor

Author: Jamie Gault
***********************************/
static int milspf = 0;
static float tmpfps = 0.0f;
bool Game_Manager::UpdateFramerate()
{
	unsigned int currenttime = GetTickCount();

	//if the framerate is set to be limited
	if( DATA("limit_frame_rate") )
	{
		//if the frame rate has surpassed it's limit
		if( currenttime - m_system_clock.oldtime < 10  )
		{
			return false;
		}
	}

	//accumulate the milliseconds per frame
	milspf += currenttime - m_system_clock.oldtime;
	
	tmpfps += 1.0f;

	if( milspf > 1000 )
	{
		framerate = tmpfps;

		milspf = 0;
		tmpfps = 0;

	}

	framescale = ((float)(currenttime - m_system_clock.oldtime))/16.0f;
	
	//update the old time 
	m_system_clock.oldtime = currenttime;

	return true;

}


/***********************************
Game_Manager Update

Author: Jamie Gault
***********************************/
void Game_Manager::Update(float t)
{

	//options for debug
	if( DATA("debug") )
	{
		if(pInput->PollKey('p') )
			DATA_MAN->RefreshData();
	}

	
	
	switch(m_ex_state)
	{
	case ES_PREGAME_LOAD:

		break;
	case ES_MAIN_MENU:
		m_pMainMenu->Update(t);

		{
			vec3 newRot = Camera->getRot(); 
			newRot.y += t/24.0f;	
			Camera->setRot(newRot);
		}

		if( m_pMainMenu->GetMenuResult() == MR_START )
		{
			UnloadMainMenu();
			LoadSetupMenu();

			//reset camera position
			vec3 newRot = Camera->getRot();
			newRot.y = 0.0f;
			Camera->setRot(newRot);
		}

		break;
	case ES_GAME_SETUP:
		m_pSetupmenu->Update( t );

		//if the game is ready to start
		if( m_pSetupmenu->StartGame() )
		{
			//load up system for who goes first
			m_pOrderRoll->Load( m_pSetupmenu->getPlayers() );
			m_ex_state = ES_ORDER_ROLL;
		}
		else if( m_pSetupmenu->BackToMain() )
		{
			UnloadSetupMenu();
			LoadMainMenu();
		}
		break;

	case ES_ORDER_ROLL:
		{
			//update the dice rollers
			m_pOrderRoll->Update(t);
			
			//if the game is ready to be played.
			if( m_pOrderRoll->IsComplete )	
			{
				LoadGame();
			}
		}
		break;
	case ES_INGAME:

		tabmenu->Update(t);
		m_dicestop_but.Update(t);
		m_continue_but.Update(t);


		//multiply time by speed up time 
		t = m_system_clock.m_clock_speed*t;

		//if game is paused
		if( m_ig_mode == IGM_DISCUSS )
		{
			m_discuss_fade += t/20.0f;

			if( m_discuss_fade > 1.0f )
				m_discuss_fade = 1.0f;
			
			//if the discussion button was pressed
			if( tabmenu->m_action == TMA_DISCUSS)
			{
				pMusic_man->play("PAUSE_O");
				m_ig_mode = IGM_NONE;
			}
			return;
		}	
		//if game is paused
		else if( m_ig_mode == IGM_PAUSE )
		{
			m_pPauseMenu->Update( t );

			//the user pressed p
			if( pInput->PollKey(ENTER)|| pInput->PollKey(ESCAPE)||
				m_pPauseMenu->GetMenuResult() == MR_RESUME || tabmenu->m_action == TMA_PAUSE )
			{
				UnloadPauseMenu();
				return;
			}
			else if(  m_pPauseMenu->GetMenuResult() == MR_EXITGAME )
			{
				UnloadGame();
				LoadMainMenu(); //get rid of
				return;
			}
		}
		else
		{
			m_discuss_fade -= t/20.0f;

			if( m_discuss_fade < 0.0f )
				m_discuss_fade = 0.0f;
			else 
				return;

			if( pInput->PollKey(ENTER)|| pInput->PollKey(ESCAPE)|| tabmenu->m_action == TMA_PAUSE)//check for pausing the game
			{
				LoadPauseMenu();
			}
		}

			


		//if the game is not paused
		if( m_ig_mode != IGM_PAUSE )
		{
			
			

			//poll for input
			InputInGameContext(t);

			if(  m_ig_state == IGS_QUESTION || m_ig_state == IGS_RESULT)
			{
				//pause the game if in a discussion
				if( /*pInput->PollKey(' ') ||*/ tabmenu->m_action == TMA_DISCUSS )
				{
					pMusic_man->play("PAUSE_I");
					m_ig_mode = IGM_DISCUSS;
					return;
				}

				//only update when the camera is done moving
				if( m_cam_man.GetTransInterp() == 1.0f)
					m_question->Update(t);


			}
			//update the current player until it is done moving
			else if( m_ig_state == IGS_MOVE )
			{
			
				//the user has reached their destination
				if( (*m_current_player)->bAtDest )
				{
					//the camera has stopped moving
					if( ! m_cam_man.IsActive() )
					{
						m_ig_state = IGS_QUESTION;
						pMusic_man->fadeOut((*m_current_player)->getSndSet() + "-happy");

						//have the camera zoom in to the player
						m_cam_man.SetCamFollowType( CFT_Q_TRANS_IN );

						LoadQuestion( DATA( "question_override") == 1, DATA("question_id"));

						//reset clock speed scale that was possibly changed during movement
						m_system_clock.m_clock_speed = 1.0f;
					}
				}
				else
				{
					//if the camera isn't transitioning
					if(!m_cam_man.IsTransActive())
					{
				
						int i = (*m_current_player)->getTempTile()->getNum();
						
						(*m_current_player)->update(t);

						if( pInput->PollMouseBut(MLEFT) )
							m_system_clock.m_clock_speed = 3.0f;
						

						
						//set the camera to follow the player
						m_cam_man.SetFocalPoint( (*m_current_player)->pos.x, DATA("TILE_HEIGHT") , (*m_current_player)->pos.z);


						if( i != (*m_current_player)->getTempTile()->getNum() || (*m_current_player)->bAtDest )
							m_dicedisplay->IncrementStep();

					}
				}
			}
			//else if(  m_ig_state == IGS_START )
			//{
			//	//render the intro pass
			//	partfeat->Update(t);
			//}
			//roll dice if game mode is still going
			else if( m_ex_state == ES_INGAME )
			{
				m_dicedisplay->Update( t );
			}

			if(  partfeat )
			{
				//render the intro pass
				partfeat->Update(t);
			}


			//if the game is still going, update the camera
			if( m_ex_state == ES_INGAME )
			{
				//update the camera
				m_cam_man.Update( t );

				//update score board
				m_scoredisplay->Update(t);
			}

		}

		break;
	}

	

}


vec4 ORIGIN = vec4(0, 0, 0, 0);



/***********************************
Game_Manager InputContext

Author: Jamie Gault
***********************************/
void Game_Manager::InputInGameContext(float t)
{

	if( m_ig_mode == IGM_PAUSE )
		return;



	if( m_ig_state == IGS_DICE )
	{
		//while the camera is moving
		if( ! m_cam_man.IsActive() )
		{
			//if dice results are shown
			if( m_dicedisplay->ResultsShown() )
			{
				//increment counter
				m_system_clock.m_delay_counter += t;

				if( pInput->PollMouseBut(MLEFT) || m_system_clock.m_delay_counter > DATA("POST_DICE_DELAY")  )
				{
					m_system_clock.m_delay_counter = 0.0f;
					SetTokenToMove();
					m_cam_man.SetCamFollowType( CFT_TOKEN_FOLLOW);
					m_cam_man.TransitionCamera( (*m_current_player)->pos.x, (*m_current_player)->pos.y, (*m_current_player)->pos.z);
					m_cam_man.speed = DATA("CAMERA_DEF_SPEED"); 
				}
			}
			else
			{
				//event to roll the die
				if( pInput->PollMouseBut(MLEFT) )
				{
					
					//if the mouse is over the button and the dice haven't been rolled yet
					if( m_dicestop_but.IsMouseOver() && m_dicedisplay->GetRollState() == ROLL_IDLE)
					{
						m_dicedisplay->RollDice();	
						pMusic_man->fadeOut("DICE_ROLL");
						pMusic_man->play("DICE_LAND");
					}
					//else if(  m_dicedisplay->GetRollState() == ROLL_STOP )
							//pMusic_man->play("DICE_LAND");
				}
			}
		}
	}
	else if(  m_ig_state == IGS_QUESTION )
	{
		
		//if the left button was pressed
		if( pInput->PollMouseBut( MLEFT ) )
		{
			int result_id = m_question->PollForAnswer();
			
			if( result_id != NO_ANSWER )
			{
				pMusic_man->play("BTN_C");
				//this is where the answers need to be found by sql
				int score = 0;
				//add data for what the user answered 
				turndump.currentData.answer = result_id;

				if( result_id == ANSWER_PRIVACY)
				{
					m_question->AssignResults( "People have a right to privacy.", 0 );
					//decrement the number of privacies they can use
					--(*m_current_player)->privacy;
				}
				else
				{

					

					switch(m_question->GetQuestionType())
					{
					case MULTIPLE_CHOICE:
						{
							//Get option moral...
							{
								std::stringstream cmd;
								cmd << "SELECT * FROM options WHERE id = " << result_id;

								sitDB->exec( cmd.str() );
							}
							int option = atoi((*sitDB->rows[0])["moral"].c_str());
							if(DATA("debug")) { std::cout << "Option moral: " << option << std::endl; }

							//Get the outcomes...
							{
								std::stringstream cmd;
								cmd << "SELECT * FROM outcomes WHERE optID = " << result_id;

								sitDB->exec( cmd.str() );
								if(int(sitDB->rows.size()) <= 0)
								{
									m_question->AssignResults( "Ask again later", 0 );
									break;
								}
							}
							int id = random( 0, int(sitDB->rows.size())-1 );

							if( option == 1 )//Good option
							{
								if( atoi((*sitDB->rows[id])["score"].c_str()) <= 0 )//Bad outcome
								{
									pMusic_man->play("OUT_B");
									float chance = random(0,100) * (*m_current_player)->getChance()/100;
									if(chance < (*m_current_player)->getChance())
									{
										//Roll again, increasing odds of good outcome
										id = random( 0, int(sitDB->rows.size())-1 );
									}
									(*m_current_player)->lastOutcome = false;
								}
								else
								{
									pMusic_man->play("OUT_G");
									(*m_current_player)->lastOutcome = true;
								}
								(*m_current_player)->setOutID(id);
								(*m_current_player)->incChance();
								(*m_current_player)->lastMoral = MORALITY::POSITIVE;
							}
							else if( option == 0 )//Neutral option
							{
								if( atoi((*sitDB->rows[id])["score"].c_str()) <= 0 )//Bad outcome
								{
									pMusic_man->play("OUT_B");
									(*m_current_player)->lastOutcome = false;
								}
								else
								{
									pMusic_man->play("OUT_G");
									(*m_current_player)->lastOutcome = true;
								}
								(*m_current_player)->setOutID(id);
								(*m_current_player)->lastMoral = MORALITY::NEUTRAL;
							}
							else if( option == -1 )//Bad option
							{
								if( atoi((*sitDB->rows[id])["score"].c_str()) > 0 )//Good outcome
								{
									pMusic_man->play("OUT_G");
									float chance = random(0,100) * (*m_current_player)->getChance()/100;
									if(chance > (*m_current_player)->getChance())
									{
										//Roll again, increasing odds of bad outcome
										id = random( 0, int(sitDB->rows.size())-1 );
									}
									(*m_current_player)->lastOutcome = true;
								}
								else
								{
									pMusic_man->play("OUT_B");
									(*m_current_player)->lastOutcome = false;
								}
								(*m_current_player)->setOutID(id);
								(*m_current_player)->decChance();
								(*m_current_player)->lastMoral = MORALITY::NEGATIVE;
							}

							//set up answer to be written
							std::map<std::string, std::string> outcome = (*sitDB->rows[id]);
							score = atoi(outcome["score"].c_str());
							m_question->AssignResults(outcome["text"], score);

							//data recorder
							turndump.currentData.result = atoi(outcome["id"].c_str());
							
						}

						break;
					case TRUE_FALSE:
						{
							std::stringstream cmd;
							cmd << "SELECT * FROM trueFalse WHERE sitID = " << m_question->GetQuestionID();
							if(DATA("debug")) { std::cout << cmd.str() << std::endl; }

							sitDB->exec( cmd.str() );
							if(sitDB->rows.size() <= 0)
							{
								if(DATA("debug")) { std::cout << "No True/False found." << std::endl; }
								m_question->AssignResults("No True/False found.", 0);
								break;
							}
								

							std::map<std::string, std::string> option = (*sitDB->rows[0]);

							if( result_id == (int)(option["answer"] != "0") )
							{
									pMusic_man->play("OUT_G");
								if(DATA("debug")) { std::cout << "Correct TF" << std::endl; }
								score = atoi(option["score"].c_str());
								(*m_current_player)->incChance();
								(*m_current_player)->lastMoral = MORALITY::POSITIVE;
								(*m_current_player)->lastOutcome = true;
							}
							else
							{
									pMusic_man->play("OUT_B");
								if(DATA("debug")) { std::cout << "Incorrect TF" << std::endl; }
								score = -atoi(option["score"].c_str());
								(*m_current_player)->decChance();
								(*m_current_player)->lastMoral = MORALITY::NEGATIVE;
								(*m_current_player)->lastOutcome = false;
							}

							m_question->AssignResults( ((option["text"] != "")?option["text"]:"NULL"), score );
						}
						break;
					
					case OPEN_ENDED:

						if(DATA("TURN_DUMP")) m_question->GetOpenEndedScore( turndump.currentData.openend_score );
						score = m_question->GetScore();
						if(m_question->GetScore() > 0)
						{
									pMusic_man->play("OUT_G");
							(*m_current_player)->incChance();
							(*m_current_player)->lastMoral = MORALITY::POSITIVE;
							(*m_current_player)->lastOutcome = true;
						}
						else
						{
									pMusic_man->play("OUT_B");
							(*m_current_player)->decChance();
							(*m_current_player)->lastMoral = MORALITY::NEGATIVE;
							(*m_current_player)->lastOutcome = false;
						}
						m_question->AssignResults( "The group has decided.", m_question->GetScore() );
						break;
					default:
						m_question->AssignResults( "...", 0 );
						break;
					}
				}

				(*m_current_player)->incScore(score);

				//assign points to the hud and play the animation
				m_scoredisplay->AddPoints((*m_current_player)->getID(), score);

				if(DATA("TURN_DUMP")) turndump.currentData.score = score;

				m_ig_state = IGS_RESULT; 
			}
		}
	}
	//if the game is in the results state after answering a question
	else if(  m_ig_state == IGS_RESULT )
	{
		if(  pInput->PollMouseBut(MLEFT ) && m_continue_but.IsMouseOver()  )
		{
			pMusic_man->play("BTN_C");

			//if the user opted out by hitting the privacy button
			if( m_question->GetAnswer() == ANSWER_PRIVACY )
			{
				//EndTurnShift();

				if (DATA("debug")) { std::cout << "mentor set default name\n"; }
				m_mentor_text.SetString("Everyone has a right to privacy.", true);				

				//need to set randomizer here for rendering mentor
				m_ig_state = IGS_MENTOR;
				m_cam_man.SetCamFollowType( CFT_Q_TRANS_OUT );

			}
			else
			{
				
				//need to set randomizer here for rendering mentor
				m_ig_state = IGS_MENTOR;
				std::string mentorText = (*m_current_player)->getMentor()->makeText(
					(*m_current_player)->lastMoral, 
					(*m_current_player)->lastOutcome, 
					(*m_current_player)->getGender(),
					(*m_current_player)->getOutID());
				if (DATA("debug")) { std::cout << "mentor set this text: " << mentorText << "\n"; }
				m_mentor_text.SetString(mentorText, true);
				m_cam_man.SetCamFollowType( CFT_Q_TRANS_OUT );
			}
		}
	}
	else if(  m_ig_state == IGS_MENTOR )
	{
		//quit button picture
		Box_2D box = Box_2D(BT_LINES , 
					DATA("INSTMSG_POSX")+ DATA("XMSG_POSX"), 
					DATA("INSTMSG_POSY")+ DATA("XMSG_POSY"), 
					DATA("XMSG_W"), DATA("XMSG_H"));

		//if the left mouse is pressed and the mouse is over the continue button
		if(  pInput->PollMouseBut(MLEFT ) && ( m_continue_but.IsMouseOver()|| box.IsMouseOver()) )
		{
			pMusic_man->play("BTN_C");

			EndTurnShift();
		}
	}
	else if(  m_ig_state == IGS_STATUS )
	{
		//increment counter
		m_system_clock.m_delay_counter += t;

		//if has pussed on beyond the delay
		if(  pInput->PollMouseBut(MLEFT ) || m_system_clock.m_delay_counter > DATA("STATUS_DELAY")  )
		{
			//reset delay counter
			m_system_clock.m_delay_counter = 0.0f;

			//switch states according to what should 
			if( (*m_current_player)->tile_action == TA_ROLL_AGAIN )
			{
				m_ig_state = IGS_DICE; pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL");
				m_dicedisplay->SetToRoll();

				//set the camera to follow the player
				m_cam_man.SetCamFollowType( CFT_OVERVIEW);

				//play sounds
				pMusic_man->fadeIn((*m_current_player)->getSndSet() + "-happy");
				pMusic_man->play((*m_current_player)->getSndSet() + "-happy");

			}
			else if( (*m_current_player)->tile_action == TA_LOSE_TURN )
			{
				(*m_current_player)->tile_action = TA_NONE;

				//set m_current_player to point to the next player for their turn
				ShiftPlayerTurn();

				//if it's the first players turn again
				if( (*m_current_player)->getID() == 1 )
				{
					//show everyone's score
					//m_ig_state = IGS_SCORE;
					
				
					if( TurnWinCheck() )
					{
						m_cam_man.SetCamFollowType( CFT_OVERVIEW);
						return;
					}
				}
				
				//status change
				if( (*m_current_player)->tile_action == TA_ROLL_AGAIN || (*m_current_player)->tile_action == TA_LOSE_TURN )
				{
					m_ig_state = IGS_STATUS;
					//set the camera to follow the player

					//have the camera focus on the player
					if( (*m_current_player)->tile_action == TA_LOSE_TURN )
					{
						//set the camera to follow the player
						m_cam_man.SetCamFollowType( CFT_TOKEN_FOLLOW);
						m_cam_man.TransitionCamera( (*m_current_player)->pos.x, (*m_current_player)->pos.y, (*m_current_player)->pos.z);
					}
				}
				else
				{
					m_ig_state = IGS_DICE; pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL"); 
					m_dicedisplay->SetToRoll();

					//set the camera to follow the player
					m_cam_man.SetCamFollowType( CFT_OVERVIEW);

					//play sounds
					pMusic_man->fadeIn((*m_current_player)->getSndSet() + "-happy");
					pMusic_man->play((*m_current_player)->getSndSet() + "-happy");

				}
			

			}

			
		}
	}
	else if(  m_ig_state == IGS_END )
	{
		//update the end for the game menu
		m_pEndMenu->Update(t);

		//the game is over
		if(  m_pEndMenu->GetMenuResult() == MR_EXITGAME )
		{
			UnloadGame();
			LoadMainMenu();
			return;
		}
		//select the retry option
		else if( m_pEndMenu->GetMenuResult() == MR_RETRY )
		{
			RestartGame();
			return;
		}
	}
	else if( m_ig_state == IGS_START )
	{
		if( !m_cam_man.IsTransActive() )
		{
			//delete the intro feature animation if it exists
			if( partfeat )
			{delete partfeat; partfeat = NULL;}

			
			m_ig_state = IGS_DICE;

			pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL");
			//play sounds
			pMusic_man->fadeIn((*m_current_player)->getSndSet() + "-happy");
			pMusic_man->play((*m_current_player)->getSndSet() + "-happy");
		}
		
	}



}



/***********************************
Game_Manager Render

Author: Jamie Gault
***********************************/
void Game_Manager::Render()
{

	//render all the 3D stuff that needs to be done in world space
	if( m_ex_state == ES_INGAME)
	{
		//if the game is in the intro
		if(  partfeat) //m_ig_state == IGS_START )
		{
			//render the intro pass
			partfeat->Render();
		}
	}
	

	//setup display for drawing things in hud space (like 3D dice that aren't occluded)
	glDisable(GL_DEPTH_TEST);
	glClear( GL_DEPTH_BUFFER_BIT);

	//render 3D Dice
	if( m_ex_state == ES_INGAME)
	{
		if(  m_ig_state == IGS_DICE || m_ig_state == IGS_MOVE )
		{
			glLoadIdentity();
			m_dicedisplay->Render3D();
		}
	}

	//set to orthogonal projection(2D) when all 3D work is done 
	ViewOrtho();

	//Render the music...
	pMusic_man->render();

	//if the user is ingame
	switch( m_ex_state)
	{
	case ES_MAIN_MENU:
		m_pMainMenu->Render();
		break;

	case ES_ORDER_ROLL:

		m_pOrderRoll->Render();
		break;

	case ES_INGAME:
		
		RenderInGame();


		//render pause menu
		if( m_ig_mode == IGM_PAUSE )
		{
			RenderScreenGel( 0.0f, 0.3f, 0.8f, 0.4f );
			m_pPauseMenu->Render();
		}

		
		
		if( m_discuss_fade)
			RenderFullScreenImage( pTexture_man->GetTexture( "discuss_bg"), m_discuss_fade);

		m_scoredisplay->Render();
		tabmenu->Render();

		break;

	case ES_GAME_SETUP:
		RenderGameSetup();
		break;
	}
		


	if( DATA("debug") )
	{
		//render frame counter
		char frame[100];
		sprintf(frame, "fps: %0.1f", framerate );
		TextBox frameratebox(textboxdata.text_m, std::string(frame) , 
							000.0f*SCREEN_X_RATIO, 700.0f*SCREEN_Y_RATIO, 1000.0f );
		frameratebox.Render();
	}

	ViewPerspective();
}


/***********************************
Game_Manager RenderInGame

Author: Jamie Gault
***********************************/
void Game_Manager::RenderInGame()
{


	//if there is a question
	if(m_question && m_ig_state != IGS_MENTOR)
	{
		if( m_cam_man.GetTransInterp() == 1.0f )
		{
			
			
			m_question->Render();

			if( DATA("questnum_display" ) )
			{
				char q[100];
				sprintf(q, "Question# %i", m_question->GetQuestionID() );
				

				TextBox questionnum(textboxdata.text_m, std::string(q) , 
									400.0f, 650.0f, 1000.0f,
									10.0f, 30, 30);

				questionnum.Render();
			}
		}
		else
		{
			//start fading to white half way through the camera transition
			if( m_cam_man.GetTransInterp() > 0.5f )
				RenderScreenGel( 1.0f, 1.0f, 1.0f, m_cam_man.GetTransInterp()*2.0f -1.0f);

			//render a box that slowly fades in as the camera zooms
			Box_2D q_title( pTexture_man->GetTexture(std::string("SitHd_")+m_question->m_category),DATA("Q_LOAD_POSX"), 
							DATA("Q_LOAD_POSY"), DATA("Q_LOAD_WD"), DATA("Q_LOAD_HT"));
			glColor4f(1.0f, 1.0f, 1.0f, m_cam_man.GetTransInterp()*DATA("Q_LOAD_SPEED"));
			q_title.Render();
			
		}

		if( m_ig_state == IGS_RESULT )
			m_continue_but.Render();
	}


	//if it's time to roll the dice
	if(  m_ig_state == IGS_DICE || m_ig_state == IGS_MOVE )
	{

		//if the dice haven't been rolled yet
		if( m_dicedisplay->GetRollState() == ROLL_IDLE )
		{
			TextBox diceroll(textboxdata.text_m, std::string("Hit the button to roll the dice!"), 
							300.0f*SCREEN_X_RATIO, 250.0f*SCREEN_X_RATIO, 1000.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO );
			diceroll.Render();
			m_dicestop_but.Render();
		}

		m_dicedisplay->Render2D();	

	}
	else if(  m_ig_state == IGS_MENTOR )
	{
		// instant message box picture
		Box_2D box = Box_2D(pTexture_man->GetTexture(std::string("MentMSG_box")), 
								DATA("INSTMSG_POSX"), DATA("INSTMSG_POSY"), 
								DATA("INSTMSG_W"), DATA("INSTMSG_H"));
		box.Render();

		//mentor picture
		if(m_question->GetAnswerIndex() != ANSWER_PRIVACY )
		box = Box_2D((*m_current_player)->getMentor()->GetEmoPic(m_question->GetScore()) , 
								DATA("INSTMSG_POSX")+ DATA("MENTMSG_POSX"), 
								DATA("INSTMSG_POSY")+ DATA("MENTMSG_POSY"), 
								DATA("MENTMSG_W"), DATA("MENTMSG_H"));
		else
		box = Box_2D((*m_current_player)->getMentor()->GetEmoPic(HAPPY) , 
								DATA("INSTMSG_POSX")+ DATA("MENTMSG_POSX"), 
								DATA("INSTMSG_POSY")+ DATA("MENTMSG_POSY"), 
								DATA("MENTMSG_W"), DATA("MENTMSG_H"));

		box.Render();

		//quit button picture
		box = Box_2D(pTexture_man->GetTexture("MSG_Xbut"), 
					DATA("INSTMSG_POSX")+ DATA("XMSG_POSX"), 
					DATA("INSTMSG_POSY")+ DATA("XMSG_POSY"), 
					DATA("XMSG_W"), DATA("XMSG_H"));
		
		//render this box if the mouse is over it
		if( box.IsMouseOver() )
			box.Render();

		m_mentor_text.Render(); //render what the mentor is supposed to say.

		m_continue_but.Render();


		//start fading to white half way through the camera transition
		if( m_cam_man.GetTransInterp() < 0.5f )
			RenderScreenGel( 1.0f, 1.0f, 1.0f, 1.0f-m_cam_man.GetTransInterp()*2.0f);			
	}
	//announcement before player turn (for turn loss or roll again)
	else if(  m_ig_state == IGS_STATUS )
	{
		std::stringstream title;

		if( (*m_current_player)->tile_action == TA_ROLL_AGAIN )
		{
			title << " gets to roll again." ;
		}
		else 
		{
			title << " lost a turn." ;
		}

		Box_2D b(pTexture_man->GetTexture(std::string("icon_") + (*m_current_player)->getName() ),
						DATA("status_x"), DATA("status_y"), 70.0f, 70.0f );

		//display the title
		TextBox titlebox(textboxdata.text_m, title.str(), 
							DATA("status_x")+65.0f, (DATA("status_y"))*SCREEN_Y_RATIO+10.0f, 
							700.0f, 16.0f, 50, 50 );

		titlebox.SetBGOffset( -70.0f, 0, 0 );

		titlebox.Render();
		b.Render();


		//flash a message to say to click the mouse to continue
		TextBox cont_txt(pText_man, std::string("Click the mouse to continue"), 
							200.0f, 600.0f, 1000.0f, 
							10.0f, 30.0f, 30.0f );
		cont_txt.Render();

	}
	else if( m_ig_state == IGS_END )
	{

		m_enddisplay->Render();
		

		//render the end game menu
		m_pEndMenu->Render();
	}	
}


/***********************************
Game_Manager RenderMenuMode

Author: Jamie Gault
***********************************/
void Game_Manager::RenderGameSetup()
{
	RenderScreenGel( 0.0f, 1.0f, 1.0f, 0.4f );
	m_pSetupmenu->Render();
}


/***********************************
Game_Manager LoadSetupMenu

Author: Jamie Gault
***********************************/
void Game_Manager::LoadSetupMenu()
{
	if( m_pSetupmenu == NULL )
		m_pSetupmenu = new SetupMenu(textboxdata.text_m, textboxdata.background);

	//initialize the system for deciding player order.
	if( m_pOrderRoll == NULL )
		m_pOrderRoll = new PlayOrderRoll();

	vec3 rot = vec3(40.0f, 0, 0);
	Camera->setRot(rot);

	m_ex_state = ES_GAME_SETUP;
	
}



/***********************************
Game_Manager UnloadSetupMenu

Author: Jamie Gault
***********************************/
void Game_Manager::UnloadSetupMenu()
{
	if( m_pSetupmenu != NULL )
	{
		delete m_pSetupmenu;
		m_pSetupmenu = NULL;
	}

	if( m_pOrderRoll != NULL )
	{
		delete m_pOrderRoll;
		m_pOrderRoll = NULL;
	}
}


/***********************************
Game_Manager LoadGame

Author: Jamie Gault
***********************************/
void Game_Manager::LoadGame()
{
	//setup privacy 
	m_privacy.m_enable = m_pSetupmenu->GetPrivEnabled();

	//setup privacy accordingly
	if( m_privacy.m_enable )
	{
		m_privacy.m_limit_value = m_pSetupmenu->GetPrivLimitVal();
		m_privacy.m_limited = m_pSetupmenu->GetPrivLimEnabled();
	}
	else
	{
		m_privacy.m_limit_value = 0;
		m_privacy.m_limited = false;
	}


	PlayerSetup::list_t playset_vec; //vector for indexing into
	playset_vec.swap(m_pOrderRoll->getOrderedPlayers());
	
	//load in the players in the correct order
	for(size_t i = 0; i < playset_vec.size(); ++i) {
		PlayerSetup & p = playset_vec[i];
		CreatePlayer(pMentor_man->GetMentor( p.m_mentor),  p.m_boardpiece, 
			p.m_gender, p.m_token_name, p.m_soundset, m_privacy.m_limit_value);
	}

	//sets user at the beginning of the list to be the active player
	m_current_player = m_player_vec.begin();



	//load in score keeper here
	m_scoredisplay = new ScoreDisplay( m_player_vec );

	

	m_win_data.rule = (WIN_RULE)( m_pSetupmenu->GetGameRules() );
	m_win_data.turn_limit = m_pSetupmenu->GetTurnLimit();
	m_win_data.goal_score = m_pSetupmenu->GetGoalScore();

	
	//create a new dice displayer
	m_dicedisplay = new DiceDisplay( true );
	m_dicedisplay->SetToRoll();

	



	//now that everything is done with the setup menu, unload it
	UnloadSetupMenu();

	m_ex_state = ES_INGAME;
	m_ig_state = IGS_START; 
	

	//load in the balloons
	partfeat = new ParticleFeature(PT_BALLOON);


	//setup overview positioning
	m_cam_man.SetFocalPoint( 0.0f, 100.0f, -10.0f);
	m_cam_man.SetCamPoint( 0.0f, 600.0f, -10.0f);
	m_cam_man.speed = DATA("CAMERA_START_SPEED");

	Camera->setCamDistance(10.0f);
	//set the camera to follow the player
	m_cam_man.SetCamFollowType( CFT_OVERVIEW);
	

	m_win_data.turn = 1;
	m_win_data.winner.clear();

	tabmenu->SetTurnText();


	if(DATA("TURN_DUMP") )
	{
		//begin the game counter
		turndump.StartTimer();
		turndump.player_cnt = (int)m_player_vec.size();
	}

}


/***********************************
Game_Manager UnloadGame

Author: Jamie Gault
***********************************/
void Game_Manager::UnloadGame()
{
	//delet the end game menu if it exists
	if( m_pEndMenu )
	{
		delete m_pEndMenu;
		m_pEndMenu = NULL; 
	}

	//unload the display of who won
	if( m_enddisplay )
	{
		delete m_enddisplay;
		m_enddisplay = NULL; 
	}

	//unload the particle effects
	if( partfeat )
	{delete partfeat; partfeat = NULL;}

	

	//clear questions
	if( m_question )
	{
		delete m_question;
		m_question = NULL;
	}

	//get rid of the question display
	if( m_dicedisplay)
	{
		delete m_dicedisplay;
		m_dicedisplay = NULL;
	}

	if( m_scoredisplay )
	{
		delete m_scoredisplay;
		m_scoredisplay = NULL;
	}

	ClearAllPlayers();

	UnloadPauseMenu();

	m_ig_state = IGS_DICE; pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL");
	m_ig_mode = IGM_NONE;

	//create a new file to be dumped into
	if(DATA("TURN_DUMP") ) turndump.CreateNewFile("./TurnDump/gameTurnDump");

	
	//Default camera positioning
	vec3 rot = vec3(DATA("CAMERA_DEF_ANGLE"), 0, 0);
	vec4 pos = vec4(0, 0, 0, 0);
	Camera->setPos(pos);
	Camera->setRot(rot);
	Camera->setCamDistance( DATA("CAMERA_DISTANCE") );
	m_cam_man.ClearPaths();

	//Camera->rot.x = 45.0f;

	//Stop music
	for(std::map<std::string, int>::iterator si = pMusic_man->getAudio()->begin(); si != pMusic_man->getAudio()->end(); ++si)
	{
		pMusic_man->stop((*si).first);
	}

}


/***********************************
Game_Manager RestartGame

Author: Jamie Gault
***********************************/
void Game_Manager::RestartGame()
{
	//delet the end game menu if it exists
	if( m_pEndMenu )
	{
		delete m_pEndMenu;
		m_pEndMenu = NULL; 
	}

	//create a new file for a new game
	if(DATA("TURN_DUMP") ) turndump.CreateNewFile("./TurnDump/gameTurnDump");


	ResetPlayers();

	//reset all the tile actions
	for( std::vector<player*>::iterator p = m_player_vec.begin(); p!= m_player_vec.end(); ++p )
	{
		(*p)->tile_action = TA_NONE;
	}

	m_current_player = m_player_vec.begin();
	
	m_ex_state = ES_INGAME;
	m_ig_state = IGS_START; // pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL");

	//set the camera to follow the player
	m_cam_man.SetCamFollowType( CFT_OVERVIEW);


	//begin the game counter
	if(DATA("TURN_DUMP") )
	{
		turndump.StartTimer();
		turndump.player_cnt = (int)m_player_vec.size();
	}

	m_dicedisplay->SetToRoll();

	//reset win data
	m_win_data.turn = 1;
	m_win_data.winner.clear();

	m_scoredisplay->Reset();


	//unload the particle effects
	if( partfeat )
	{delete partfeat; partfeat = NULL;}

	partfeat = new ParticleFeature(PT_BALLOON);


	//setup overview positioning
	m_cam_man.SetFocalPoint( 0.0f, 100.0f, -10.0f);
	m_cam_man.SetCamPoint( 0.0f, 600.0f, -10.0f);
	m_cam_man.speed = DATA("CAMERA_START_SPEED");

	Camera->setCamDistance(10.0f);
	//set the camera to follow the player
	m_cam_man.SetCamFollowType( CFT_OVERVIEW);

}



/***********************************
Game_Manager LoadMainMenu

Author: Jamie Gault
***********************************/
void Game_Manager::LoadMainMenu()
{
	if(!m_pMainMenu)
		m_pMainMenu = new MainMenu(textboxdata.text_m );

	pMusic_man->play("MAIN_MENU");

	//Default camera positioning
	vec3 rot = vec3(DATA("CAMERA_DEF_ANGLE"), 0, 0);
	vec4 pos = vec4(0, 0, 0, 0);
	Camera->setPos(pos);
	Camera->setRot(rot);
	Camera->setCamDistance( DATA("CAMERA_DISTANCE") );
	m_cam_man.ClearPaths();


	m_ex_state = ES_MAIN_MENU;
}


/***********************************
Game_Manager UnloadMainMenu

Author: Jamie Gault
***********************************/
void Game_Manager::UnloadMainMenu()
{
	if(m_pMainMenu)
	{
		delete m_pMainMenu;
		m_pMainMenu = NULL;
	}
}

/***********************************
Game_Manager LoadPauseMenu

Author: Jamie Gault
***********************************/
void Game_Manager::LoadPauseMenu()
{
	if(!m_pPauseMenu)
		m_pPauseMenu = new PauseMenu(textboxdata.text_m );


	m_ig_mode = IGM_PAUSE;

	pMusic_man->pause();
}


/***********************************
Game_Manager UnloadPauseMenu

Author: Jamie Gault
***********************************/
void Game_Manager::UnloadPauseMenu()
{
	if(m_pPauseMenu)
	{
		delete m_pPauseMenu;
		m_pPauseMenu = NULL;
	}

	m_ig_mode = IGM_NONE;
	pMusic_man->play();
	//Fill the buffers -- I know it's cheap, but it's a split mili-second
	pMusic_man->render();
}


/***********************************
Game_Manager CreatePlayer

Author: Jamie Gault
***********************************/
void Game_Manager::CreatePlayer(Mentor* m, pina* mesh, GENDER gen, std::string Name, int set, int priv_limit)
{
    player* myPlayer = new player;
    myPlayer->setPina(mesh);

   	myPlayer->setName( Name );
	
	myPlayer->setMentor( m);
	myPlayer->setGender( gen);

	myPlayer->setSndSet( pMusic_man->getSndSet(set) );

	m_player_vec.push_back(myPlayer);
	myPlayer->setID((int)m_player_vec.size());

	myPlayer->privacy = priv_limit;


	myPlayer->pos.x = myPlayer->getTile()->pos.x + myPlayer->getTile()->getBases()[myPlayer->getID()-1].x;
	myPlayer->pos.y = myPlayer->getTile()->pos.y + myPlayer->getTile()->getBases()[myPlayer->getID()-1].y;
	myPlayer->pos.z = myPlayer->getTile()->pos.z + myPlayer->getTile()->getBases()[myPlayer->getID()-1].z;
}


/***********************************
Game_Manager ResetPlayers

Author: Jamie Gault
***********************************/
void Game_Manager::ResetPlayers()
{
	for( std::vector<player*>::iterator i = m_player_vec.begin(); i != m_player_vec.end(); ++i )
	{
		(*i)->Reset();
	}
}


/***********************************
Game_Manager EndTurnShift

Author: Jamie Gault
***********************************/
void Game_Manager::EndTurnShift()
{
	//dump all the data from this turn into a file
	if(DATA("TURN_DUMP")) 
	{
		turndump.SubmitTurnData();
		turndump.DumpData();
	}

	//free up the question pointer since the question isn't needed any more
	delete m_question;
	m_question = NULL;

	if( PointWinCheck(*m_current_player) )
	{
		//set the camera to follow the player
		m_cam_man.SetCamFollowType( CFT_OVERVIEW);
		

		return;
	}

	//shift the player in control if not having another turn
	if( (*m_current_player)->tile_action != TA_ROLL_AGAIN )
	{
		ShiftPlayerTurn();
	}
	

	//if it's the first players turn again
	if( (*m_current_player)->getID() == 1 && ((*m_current_player)->tile_action != TA_ROLL_AGAIN ) )
	{
		//show everyone's score
		//m_ig_state = IGS_SCORE;
	
		if( TurnWinCheck() )
		{
			m_cam_man.SetCamFollowType( CFT_OVERVIEW);
			return;
		}
		
	}
	//else
	{
		if( (*m_current_player)->tile_action == TA_ROLL_AGAIN || (*m_current_player)->tile_action == TA_LOSE_TURN )
		{
			m_ig_state = IGS_STATUS;


			if( (*m_current_player)->tile_action == TA_LOSE_TURN )
			{
				//set the camera to follow the player
				m_cam_man.SetCamFollowType( CFT_TOKEN_FOLLOW);
				m_cam_man.TransitionCamera( (*m_current_player)->pos.x, (*m_current_player)->pos.y, (*m_current_player)->pos.z);
			}
			
		}
		else
		{
			m_ig_state = IGS_DICE; pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL"); 
			m_dicedisplay->SetToRoll();

			m_cam_man.SetCamFollowType( CFT_OVERVIEW);

			//play sounds
			pMusic_man->fadeIn((*m_current_player)->getSndSet() + "-happy");
			pMusic_man->play((*m_current_player)->getSndSet() + "-happy");
				
			
		}
	}
}


/***********************************
Game_Manager ClearAllPlayers

Author: Jamie Gault
***********************************/
void Game_Manager::ClearAllPlayers()
{
	for(std::vector<player*>::iterator pi = m_player_vec.begin(); 
							pi != m_player_vec.end(); ++pi )
	{
		if( *pi )
		{
			RemoveObject( (*pi) );
			delete *pi;
			*pi = NULL;
		}
	}

	m_player_vec.clear();
	
}


/***********************************
Game_Manager EndGameCleanUp

Author: Jamie Gault
***********************************/
void Game_Manager::EndGameCleanUp()
{
	pMusic_man->close();

	ClearAllPlayers();

	//get rid of the end game menu
	if( m_pEndMenu )
	{
		delete m_pEndMenu;
		m_pEndMenu = NULL;
	}
	
	if(m_question)
	{
		delete m_question;
		m_question = NULL;
	}

}

/***********************************
Game_Manager GetInGameState

Author: Jamie Gault
***********************************/
INGAME_STATE Game_Manager::GetInGameState()
{
	return m_ig_state;
}


/***********************************
Game_Manager GetMode

Author: Jamie Gault
***********************************/
INGAME_MODE Game_Manager::GetMode()
{
	return m_ig_mode;
}



/***********************************
Game_Manager SetTokenToMove

Author: Jamie Gault
***********************************/
void Game_Manager::SetTokenToMove()
{
	
	int value1 = 0.0f;
	int value2 = 0.0f;

	m_dicedisplay->GetDisplayValues(value1, value2);

	if(DATA("debug")) { std::cout << "Dice Roll: " << value1 << " " << value2 <<  std::endl; }
	(*m_current_player)->incTile(value1 + value2);

	m_ig_state = IGS_MOVE;

	if(DATA("TURN_DUMP") ) turndump.currentData.roll = value1 + value2;

	m_cam_man.AddPath(CPT_INTERP_POS, &(*m_current_player)->pos );
	m_cam_man.AddPath(CPT_ZOOM, NULL, -2.0f, -100.0f  );
	m_cam_man.AddPath(CPT_FOLLOW, (*m_current_player));
	m_cam_man.AddPath(CPT_ROTATE_XZ, NULL, 2.0f, 360.0f);
	//m_cam_man.AddPath(CPT_ZOOM, NULL, 1.0f, 200.0f  );
}


/***********************************
Game_Manager ShiftPlayerTurn

Author: Jamie Gault
***********************************/
void Game_Manager::ShiftPlayerTurn()
{

	++m_current_player;

	if( m_current_player == m_player_vec.end() )
		m_current_player = m_player_vec.begin();

	if(DATA("TURN_DUMP") ) turndump.currentData.playerID = (*m_current_player)->getID();

	m_scoredisplay->active_player = (*m_current_player)->getID() - 1;
}



/***********************************
Game_Manager ShiftPlayerTurn

Author: Jamie Gault
***********************************/
bool Game_Manager::PointWinCheck(player* p)
{
	if( p->getScore() >= m_win_data.goal_score && ( m_win_data.rule == FIRST_TO_LIMIT || m_win_data.rule == BOTH_LIMIT ))
	{
	
		m_win_data.winner.push_back(p);
		m_ig_state = IGS_END;
		m_pEndMenu = new EndGameMenu(textboxdata.text_m);

		partfeat = new ParticleFeature(PT_CONFETTI);

		m_enddisplay = new EndDisplay(m_win_data);
		return true;
	}

	return false;
}


/***********************************
Game_Manager TurnWinCheck

Author: Jamie Gault
***********************************/
bool Game_Manager::TurnWinCheck()
{
	

	//if the turn limit has been reached
	if( m_win_data.turn == m_win_data.turn_limit && ( m_win_data.rule == ROUND_LIMIT || m_win_data.rule == BOTH_LIMIT ))
	{
		float maxscore = -1000.0f;

		//check each player's score
		for( std::vector<player*>::iterator p = m_player_vec.begin(); p != m_player_vec.end() ; ++p )
		{
			if( (*p)->getScore() > maxscore )
			{
				m_win_data.winner.clear();
				m_win_data.winner.push_back( (*p) );
				maxscore = (*p)->getScore();				
			}
			else if( (*p)->getScore() == maxscore )
			{
				m_win_data.winner.push_back( (*p) );		
			}
		}

		m_ig_state = IGS_END;
		m_pEndMenu = new EndGameMenu(textboxdata.text_m);
		m_enddisplay = new EndDisplay(m_win_data); //load up the end display

		//load the ending particle effect
		partfeat = new ParticleFeature(PT_CONFETTI);
		return true;
	}

	++m_win_data.turn;

	//update the turn in the hud
	tabmenu->SetTurnText();

	return false;
}

GLuint FindBackGround( std::string s)
{
	s.append( "_BG" );

	return pTexture_man->GetTexture(s);
}



/***********************************
Game_Manager ChooseTokenRender

Author: Jamie Gault
***********************************/
void Game_Manager::ChooseTokenRender()
{
	//only do this if the game is in the setup mode
	if(m_ex_state == ES_GAME_SETUP )
	{
		//if in the pieces section
		if( m_pSetupmenu->m_state == SMS_PLAYER_TOKEN )
		{
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glLoadIdentity();
				Camera->render();
				//iterat through the list and render all the pieces
				for( std::list<PieceOption*>::iterator poi =  m_pSetupmenu->m_piece_menu.m_pie_opt_list.begin() ;
						poi !=  m_pSetupmenu->m_piece_menu.m_pie_opt_list.end(); ++poi )
				{
					(*poi)->m_piece.render();
				}
			glPopMatrix();
		}
	}
}




/***********************************
Game_Manager LoadQuestion

Author: Jamie Gault
***********************************/
void Game_Manager::LoadQuestion(bool qoverride, int q_id)
{
	std::vector<std::string> answers;
	std::vector<int> answer_ids;
	int id =0;

	//How many questions have gone by
	++turncount;


	if(!qoverride)
	{
	//Get random situation
		//sitDB->exec("SELECT * FROM situations");
		std::stringstream ss;

		ss << "SELECT * FROM situations WHERE category = '" << (*m_current_player)->getTile()->getType() << "' ";

		if(turncount < DATA("TOUCHY_TURN"))
			ss << "AND touchy <> 1 ";
		
		ss <<"AND sex <> ";
		if((*m_current_player)->getGender() == GEN_MALE)
		{
			//Female
			ss << "2";
		}
		else
		{
			//Male
			ss << "1";
		}

		sitDB->exec(ss.str());
		//Random id%
		id = random( 0, int(sitDB->rows.size())-1 );

		if(DATA("debug")) { std::cout << (*m_current_player)->getTile()->getType() << std::endl; }

		if(id < 0 || id >= int(sitDB->rows.size()))
		{
			if(DATA("debug")) { std::cout << "Out of range: " << id << " size: " << sitDB->rows.size() << std::endl; }

			 m_ig_state = IGS_DICE; pMusic_man->fadeIn("DICE_ROLL"); pMusic_man->play("DICE_ROLL");
			 m_dicedisplay->SetToRoll();

			 m_cam_man.AddPath(CPT_ZOOM, NULL, 3.0f, 100.0f  );
			m_cam_man.AddPath(CPT_INTERP_POS, &ORIGIN);
			return;
		}

		if(DATA("TURN_DUMP") ) turndump.currentData.space_id = (*m_current_player)->getTile()->getType();

	}
	else
	{
		std::stringstream num;
		num << q_id;
		
		sitDB->exec("SELECT * FROM situations WHERE id = " + num.str());
		id = 0;
	}

	
	std::map<std::string, std::string> situation = (*sitDB->rows[id]);
	
	//Update statistics
	if(DATA("q_stats"))
	{
		this->stat.open();

		std::stringstream count;
		count << atoi(
				(this->stat.read(situation["id"]) != "")
				?this->stat.read(situation["id"]).c_str()
				:"0" ) + 1;
		this->stat.write(situation["id"], count.str());

		this->stat.close();	
	}

	if(situation["type"] == "mc")
	{
		sitDB->exec("SELECT * FROM options WHERE sitID = " + situation["id"]);
		for(int i = 0; i < int(sitDB->rows.size()); ++i)
		{
			answers.push_back( std::string( (*sitDB->rows[i])["text"] ) );
			answer_ids.push_back( atoi((*sitDB->rows[i])["id"].c_str()) );
		}
	}
	else if(situation["type"] == "tf")
	{
		answers.push_back( "True" );
			answer_ids.push_back( 1 );
		answers.push_back( "False" );
			answer_ids.push_back( 0 );
	}
	else if(situation["type"] == "oe")
	{
		answers.push_back("Continue...");
		answer_ids.push_back( 0 );
	}

	QuestionType qType;
	if(situation["type"] == "mc")
		qType = MULTIPLE_CHOICE;
	else if(situation["type"] == "tf")
		qType = TRUE_FALSE;
	else if(situation["type"] == "oe")
		qType = OPEN_ENDED;

	//why is this a problem
	if( !answers.size() || !answer_ids.size() )
	{
		answers.push_back("This Question has no answers....(retard)...");
		answer_ids.push_back( 0 );
	}

	
	m_question = new Question( atoi(situation["id"].c_str()), 
								qType, 
								(*m_current_player)->getTile()->getType(),
								std::string(situation["text"]), answers,
								answer_ids,
								&m_player_vec, (*m_current_player)->getID(), (int)m_player_vec.size());

	m_question->SetPrivacySettings(m_privacy, (*m_current_player)->privacy);


	//set the back ground
	m_question->SetBackground( FindBackGround((*m_current_player)->getTile()->getType()));

	
	if(DATA("TURN_DUMP") )
	{
		//assign question id
		turndump.currentData.question = atoi(situation["id"].c_str());
		turndump.currentData.q_type = qType;
	}
	


}






void Game_Manager::LoadStart()
{
	//Assets...
	drawBoard();

	//Play the ambient sound
	(*pMusic_man->getOggs())[ (*pMusic_man->getAudio())["MAIN_MENU"] ]->loop = true;
	(*pMusic_man->getOggs())[ (*pMusic_man->getAudio())["DICE_ROLL"] ]->loop = true;
	
	for(std::map<std::string, Model*>::iterator mi = pModel_man->getInstances()->begin(); mi != pModel_man->getInstances()->end(); ++mi)
	{
		obj *object = new obj();

		object->setName((*mi).first);

		object->setPos	( (*mi).second->loc.getPos() );
		object->setRot	( (*mi).second->loc.getRot() );
		object->setScale( (*mi).second->loc.getScale() );

		object->setLighting((*mi).second->lit);

		(*mi).second->mesh->cull_face = (*mi).second->cull_face;

		object->setPina((*mi).second->mesh);
		object->setTexture( pTexture_man->GetTexture((*mi).second->texture));
	}

	pTexture_man->UnloadTexture( "MOD_SLIDE_1" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_2" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_3" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_4" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_5" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_6" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_7" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_8" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_9" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_10" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_11" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_12" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_13" );
	pTexture_man->UnloadTexture( "MOD_SLIDE_14" );

	pTexture_man->UnloadTexture( "PLA_SLIDE_1" );
	pTexture_man->UnloadTexture( "PLA_SLIDE_2" );
	pTexture_man->UnloadTexture( "PLA_SLIDE_3" );

	LoadMainMenu();

	//pMusic_man->play("SET1-happy");

}