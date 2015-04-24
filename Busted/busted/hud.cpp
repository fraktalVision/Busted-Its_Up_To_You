/*
	hud.cpp
	Created by: Jamie Gault
	Created on 5/23/07

*/




#include "busted.all.h"
#include "hud.h"


extern Texture_Manager* pTexture_man;


/***********************************
DiceDisplay Constructor

Author: Jamie Gault
***********************************/
DiceDisplay::DiceDisplay(bool two_d, float x, float y): m_3D(two_d), m_rolling(false), m_updatedelay(0.0f), m_roll_state(ROLL_IDLE)
{

	if( m_3D )
	{
		screen_pos[0] = 0.0f;
		screen_pos[1] = 0.0f;
	}
	else
	{
		//set screen position 
		screen_pos[0] = x;
		screen_pos[1] = y;
	}

	m_dice_textures[0] = pTexture_man->GetTexture("DIE_ONE");
	m_dice_textures[1] = pTexture_man->GetTexture("DIE_TWO");
	m_dice_textures[2] = pTexture_man->GetTexture("DIE_THREE");
	m_dice_textures[3] = pTexture_man->GetTexture("DIE_FOUR");
	m_dice_textures[4] = pTexture_man->GetTexture("DIE_FIVE");
	m_dice_textures[5] = pTexture_man->GetTexture("DIE_SIX");

	PickNewNumbers();

	m_cur_step = 0.0f;

	//initialize the angles of rotation
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j<3; ++j )
		{
			m_orig_ang[i][j] = 0.0f;
			m_delt_ang[i][j] = 0.0f;
		}
	}

}



/***********************************
DiceDisplay Update

Author: Jamie Gault
***********************************/
void DiceDisplay::Update(float t)
{
	//if the die is being rolled
	if( m_rolling && m_roll_state == ROLL_IDLE)
	{
		m_updatedelay += t;

		if(m_updatedelay > DATA("DICE_ROLL_SPEED"))
		{
			m_updatedelay = 0.0f;
			PickNewNumbers();
		}

		//spin the dice over time
		if( m_3D )
		{
			//rotate the dice over time
			m_delt_ang[0][0] += DATA("DICE_SPIN_X")*t;
			m_delt_ang[1][0] += DATA("DICE_SPIN_X")*t;
			m_delt_ang[0][1] += DATA("DICE_SPIN_Y")*t;
			m_delt_ang[1][1] += DATA("DICE_SPIN_Y")*t;
		}
	}
	else
	{
		//handle the animations for the 3D dice
		if( m_3D )
		{
			if( m_roll_state == ROLL_OUT )
			{
				m_updatedelay +=t;

				float timelimit = DATA("D_ROLLOUT_TIME");

				if( m_updatedelay < timelimit )
				{
					screen_pos[1] = m_updatedelay/timelimit * 30.0f;
					screen_pos[0] = 0.0f/*-m_updatedelay/45.0f * 50.0f*/;
					//rotat
					m_delt_ang[0][0] += 12.1f*t;
					m_delt_ang[1][0] += 12.1f*t;
				}
				else
				{
					m_roll_state = ROLL_IN;
					//setup the 3D dice orientations
					SetDie3DOrient( LEFT_DIE );
					SetDie3DOrient( RIGHT_DIE );
					m_updatedelay = 0.0f;
				}
			}
			else if( m_roll_state == ROLL_IN )
			{
				m_updatedelay += t;

				float timelimit = DATA("D_ROLLIN_TIME");

				if( m_updatedelay < timelimit)
				{
					screen_pos[1] = (1.0f-m_updatedelay/timelimit)* DATA("DICEROLLIN_Y");
					screen_pos[0] = -(1.0f-m_updatedelay/timelimit)* DATA("DICEROLLIN_Z");

					m_delt_ang[0][0] = (1.0f-m_updatedelay/timelimit)*-540.0f;
					m_delt_ang[1][0] = (1.0f-m_updatedelay/timelimit)*-540.0f;
				}
				else
				{
					m_roll_state = ROLL_STOP;
					m_updatedelay = 0.0f;
					m_rolling = false;
					m_delt_ang[0][0] = 0.0f;
					m_delt_ang[0][1] = 0.0f;
					m_delt_ang[0][2] = 0.0f;
					m_delt_ang[1][0] = 0.0f;
					m_delt_ang[1][1] = 0.0f;
					m_delt_ang[1][2] = 0.0f;
					screen_pos[1] = 0.0f;
				}
			}
		}
	}

	

}



/***********************************
DiceDisplay Render3D

Author: Jamie Gault
***********************************/
void DiceDisplay::Render3D()
{
	if( m_3D )
	{
		glEnable(GL_DEPTH_TEST);

			RenderDie3D( RIGHT_DIE );
			RenderDie3D( LEFT_DIE );

		glDisable(GL_DEPTH_TEST);
	}
}



/***********************************
DiceDisplay RenderDie3D

Author: Jamie Gault
***********************************/
void DiceDisplay::RenderDie3D( DIE_SIDE s )
{
	glPushMatrix();

		if( s == LEFT_DIE )
			glTranslatef( -3.0f, 0.0f, 0.0f );
		else
			glTranslatef( 3.0f, 0.0f, 0.0f );

		glTranslatef( 0.0f, -4.0f + screen_pos[1], -30.0f + screen_pos[0]);

		
		glRotatef( m_delt_ang[s][0], 1.0f, 0.0f, 0.0f );
		glRotatef( m_delt_ang[s][1], 0.0f, 1.0f, 0.0f );
		glRotatef( m_delt_ang[s][2], 0.0f, 0.0f, 1.0f );

		glRotatef( m_orig_ang[s][0], 1.0f, 0.0f, 0.0f );
		glRotatef( m_orig_ang[s][1], 0.0f, 1.0f, 0.0f );
		glRotatef( m_orig_ang[s][2], 0.0f, 0.0f, 1.0f );

		glScalef(2.0f, 2.0f, 2.0f );

		//render side 1;
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[0] );
		Render3DSide(1);
		//render side 2
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[1] );
		Render3DSide(2);
		//render side 3
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[2] );
		Render3DSide(3);
		//render side 4;
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[3] );
		Render3DSide(4);
		//render side 5;
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[4] );
		Render3DSide(5);
		//render side 6;
		glBindTexture(GL_TEXTURE_2D,m_dice_textures[5] );
		Render3DSide(6);

		//draw the marker counters
		if( s == LEFT_DIE )
		{
			//iterate through drawing the markers
			int i = 1;
			for( std::vector<unsigned int>::iterator marker_it = m_marker_vec.begin(); 
				marker_it != m_marker_vec.end(); ++marker_it, ++i )
			{
				if( i <= m_display_values[0] )
				{
					glBindTexture(GL_TEXTURE_2D,*marker_it);
					Render3DSide(m_display_values[0]);
				}
				else
					break;
			}
		}
		else
		{
			//Draw the right die markers
			//iterate through drawing the markers
			int i = 1;
			for( std::vector<unsigned int>::iterator marker_it = m_marker_vec.begin(); 
				marker_it != m_marker_vec.end(); ++marker_it, ++i )
			{
				if( i > m_display_values[0] )
				{
					glBindTexture(GL_TEXTURE_2D,*marker_it);
					Render3DSide(m_display_values[1]);
				}
			}


		}
	glPopMatrix();

}



/***********************************
DiceDisplay Render3DSide

Author: Jamie Gault
***********************************/
void DiceDisplay::Render3DSide( int val )
{

	switch( val )
	{

	case 1:
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);			
			glVertex3f(-1.0f, 1.0f, 1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(-1.0f, -1.0f, 1.0f );
			glTexCoord2f(1,1);			
			glVertex3f(1.0f, -1.0f, 1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(1.0f, 1.0f, 1.0f );
		glEnd();
		break;

	case 2:
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);			
			glVertex3f(1.0f, 1.0f, 1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(1.0f, -1.0f, 1.0f );
			glTexCoord2f(1,1);			
			glVertex3f(1.0f, -1.0f, -1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(1.0f, 1.0f, -1.0f );
		glEnd();
		break;

	case 3:
		//render side 3
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);			
			glVertex3f(-1.0f, 1.0f, -1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(-1.0f, -1.0f, -1.0f );
			glTexCoord2f(1,1);			
			glVertex3f(-1.0f, -1.0f, 1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(-1.0f, 1.0f, 1.0f );
		glEnd();
		break;

	case 4:
		//render side 4;
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);			
			glVertex3f(-1.0f, -1.0f, 1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(-1.0f, -1.0f, -1.0f );
			glTexCoord2f(1,1);			
			glVertex3f(1.0f, -1.0f, -1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(1.0f, -1.0f, 1.0f );
		glEnd();
		break;

	case 5:
		//render side 5;
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);			
			glVertex3f(1.0f, 1.0f, 1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(1.0f, 1.0f, -1.0f );
			glTexCoord2f(0,0);			
			glVertex3f(-1.0f, 1.0f, -1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(-1.0f, 1.0f, 1.0f );
		glEnd();
		break;

	case 6:
		//render side 6;
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);			
			glVertex3f(1.0f, 1.0f, -1.0f );
			glTexCoord2f(1,0);			
			glVertex3f(1.0f, -1.0f, -1.0f );
			glTexCoord2f(0,0);			
			glVertex3f(-1.0f, -1.0f, -1.0f );
			glTexCoord2f(0,1);			
			glVertex3f(-1.0f, 1.0f, -1.0f );
		glEnd();
		break;
	}
}



/***********************************
DiceDisplay Render2D

Author: Jamie Gault
***********************************/
void DiceDisplay::Render2D()
{
	if(!m_3D)
	{
		RenderCoordScreenImage( m_dice_textures[m_display_values[0] - 1], 
								screen_pos[0], screen_pos[1], screen_pos[0]+75.0f, screen_pos[1]+75.0f);

		RenderCoordScreenImage( m_dice_textures[m_display_values[1] - 1], 
								screen_pos[0] + 100.0f, screen_pos[1], screen_pos[0]+75.0f + 100.0f, screen_pos[1]+75.0f);

		//iterate through drawing the markers
		int i = 1;
		for( std::vector<unsigned int>::iterator marker_it = m_marker_vec.begin(); 
			marker_it != m_marker_vec.end(); ++marker_it, ++i )
		{
			if( i <= m_display_values[0] )
				RenderCoordScreenImage( *marker_it, screen_pos[0], screen_pos[1], screen_pos[0]+75.0f, screen_pos[1]+75.0f);
			else
				RenderCoordScreenImage( *marker_it, screen_pos[0] + 75.0f, screen_pos[1], screen_pos[0]+75.0f + 150.0f, screen_pos[1]+75.0f);
		}
	}

}


/***********************************
DiceDisplay RollDice

Author: Jamie Gault
***********************************/
void DiceDisplay::RollDice()
{
	if( m_roll_state != ROLL_IDLE )
		return;
	
	PickNewNumbers();

	m_total_steps = m_display_values[0] + m_display_values[1];
	m_cur_step = 0.0f;

	if( m_3D )
	{
		m_roll_state = ROLL_OUT;
		m_updatedelay = 0.0f;
	}
	else
		m_rolling = false;


}



/***********************************
DiceDisplay SetDisplayValues

Author: Jamie Gault
***********************************/
void DiceDisplay::GetDisplayValues( int &first, int &second )
{
	first = m_display_values[0];
	second = m_display_values[1];
}

/***********************************
DiceDisplay SetDie3DOrient

Author: Jamie Gault
***********************************/
void DiceDisplay::SetDie3DOrient( DIE_SIDE side )
{
	m_delt_ang[side][0] = 0.0f;
	m_delt_ang[side][1] = 0.0f;
	m_delt_ang[side][2] = 0.0f;

	int s = (side == LEFT_DIE)? 0:1;

	//orient the die according to the result value
	switch(m_display_values[s])
	{
	default:
	case 1:
		m_orig_ang[s][0] = 0.0f;
		m_orig_ang[s][1] = 0.0f;
		m_orig_ang[s][2] = 0.0f;
		break;
	case 2:
		m_orig_ang[s][0] = 0.0f;
		m_orig_ang[s][1] = -90.0f;
		m_orig_ang[s][2] = 0.0f;
		break;
	case 3:
		m_orig_ang[s][0] = 0.0f;
		m_orig_ang[s][1] = 90.0f;
		m_orig_ang[s][2] = 0.0f;
		break;
	case 4:
		m_orig_ang[s][0] = -90.0f;
		m_orig_ang[s][1] = 0.0f;
		m_orig_ang[s][2] = 0.0f;
		break;
	case 5:
		m_orig_ang[s][0] = 90.0f;
		m_orig_ang[s][1] = 0.0f;
		m_orig_ang[s][2] = 0.0f;
		break;
	case 6:
		m_orig_ang[s][0] = 180.0f;
		m_orig_ang[s][1] = 0.0f;
		m_orig_ang[s][2] = 0.0f;
		break;

	}

}



/***********************************
DiceDisplay PickNewNumbers

Author: Jamie Gault
***********************************/
void DiceDisplay::PickNewNumbers()
{
	int value = random(1, 5);

	//prevent a number from appearing twice
	m_display_values[0] = ( value < m_display_values[0] )? value: value + 1;

	value = random(1, 5);
	m_display_values[1] = ( value < m_display_values[1] )? value: value + 1;
}


/***********************************
DiceDisplay IncrementStep

Author: Jamie Gault
***********************************/
void DiceDisplay::IncrementStep()
{
	++m_cur_step;

	if( m_cur_step <= m_display_values[0] )
		m_marker_vec.push_back(GetMarkerValue(m_display_values[0], m_cur_step ));
	else
		m_marker_vec.push_back(GetMarkerValue(m_display_values[1], m_cur_step - m_display_values[0] ));

}



/***********************************
DiceDisplay AddMarker

Author: Jamie Gault
***********************************/
GLuint DiceDisplay::GetMarkerValue(int dieside, int cur_count )
{

	switch( dieside )
	{
	default:
	case 1:
		return pTexture_man->GetTexture("DIE_SPOT_CM");

		break;
	case 2:
		switch( cur_count )
		{
		case 1:
			return pTexture_man->GetTexture("DIE_SPOT_LT");
		case 2:
			return pTexture_man->GetTexture("DIE_SPOT_RB");
		}
	case 3:
		switch( cur_count )
		{
		case 1:
			return pTexture_man->GetTexture("DIE_SPOT_LT");
		case 2:
			return pTexture_man->GetTexture("DIE_SPOT_CM");
		case 3:
			return pTexture_man->GetTexture("DIE_SPOT_RB");
		}
	case 4:
		switch( cur_count )
		{
		case 1:
			return pTexture_man->GetTexture("DIE_SPOT_LT");
		case 2:
			return pTexture_man->GetTexture("DIE_SPOT_LB");
		case 3:
			return pTexture_man->GetTexture("DIE_SPOT_RT");
		case 4:
			return pTexture_man->GetTexture("DIE_SPOT_RB");
		}
	case 5:
		switch( cur_count )
		{
		case 1:
			return pTexture_man->GetTexture("DIE_SPOT_LT");
		case 2:
			return pTexture_man->GetTexture("DIE_SPOT_LB");
		case 3:
			return pTexture_man->GetTexture("DIE_SPOT_CM");
		case 4:
			return pTexture_man->GetTexture("DIE_SPOT_RT");
		case 5:
			return pTexture_man->GetTexture("DIE_SPOT_RB");
		}
	case 6:
		switch( cur_count )
		{
		case 1:
			return pTexture_man->GetTexture("DIE_SPOT_LT");
		case 2:
			return pTexture_man->GetTexture("DIE_SPOT_LM");
		case 3:
			return pTexture_man->GetTexture("DIE_SPOT_LB");
		case 4:
			return pTexture_man->GetTexture("DIE_SPOT_RT");
		case 5:
			return pTexture_man->GetTexture("DIE_SPOT_RM");
		case 6:
			return pTexture_man->GetTexture("DIE_SPOT_RB");
		}
	}

	return pTexture_man->GetTexture("DIE_SPOT_CM");
}


/***********************************
DiceDisplay AddMarker

Author: Jamie Gault
***********************************/
void DiceDisplay::SetToRoll() 
{ 
	m_rolling = true; 
	m_marker_vec.clear();
	m_roll_state = ROLL_IDLE;

	m_delt_ang[0][0] += 82.0f; //keeps the two dice from syncing up and turning in unison
}





/***********************************
PlayOrderRoll Constructor

Author: Jamie Gault
***********************************/
PlayOrderRoll::PlayOrderRoll(): IsComplete(false),
								m_roll_count(0),
								ol_loaded(false),
								results_shown(false),
								interp_cntr(0.0f),
								INTERPMAX(100.0f)
{
	m_title = TextBox(pText_man, std::string("Roll to see who goes first!")
			, DATA("MENU_TITLE_POSX"), DATA("MENU_TITLE_POSY")
			//, 80.0f*SCREEN_X_RATIO, 100.0f*SCREEN_Y_RATIO
			, 1000.0f, 15.0f*SCREEN_X_RATIO, 40*SCREEN_X_RATIO, 40*SCREEN_Y_RATIO );

	m_footer = TextBox(pText_man, std::string("Click the mouse to continue")
			, 300.0f, 700.0f
			, 1000.0f, 10.0f, 30.0f, 30.0f );

}

/***********************************
PlayOrderRoll Destructor

Author: Jamie Gault
***********************************/
PlayOrderRoll::~PlayOrderRoll()
{
	//clean up all the on screen icons
	for (size_t i = 0; i < m_players.size(); ++i) {
		delete m_titlebox[i];
		delete m_orderbox[i];
		delete m_stopbut_vec[i];
		delete m_dicerolls[i];
		delete m_tok_roll_icons[i];
		delete m_tok_results_icons[i];
	}
}

/***********************************
PlayOrderRoll Update

Author: Jamie Gault
***********************************/
void PlayOrderRoll::Update(float t)
{
	for( std::vector<DiceDisplay*>::iterator dice_it = m_dicerolls.begin();
	dice_it != m_dicerolls.end(); ++dice_it )
	{
		(*dice_it)->Update(t);
	}


	for( std::vector<Answer*>::iterator stop_it = m_stopbut_vec.begin(); 
			stop_it != m_stopbut_vec.end(); ++stop_it )
	{
		(*stop_it)->Update(t);
	}

	//poll for button presses
	if( pInput->PollMouseBut(MLEFT) )
	{
		//check for if all dice have been rolled
		if( m_roll_count == m_players.size() )
		{

			//the person is done using the dice roll system
			IsComplete = true;	
		}
		else
		{
			int userid = 0;
			int stopmarker = -1;
			for( std::vector<Answer*>::iterator stop_it = m_stopbut_vec.begin(); 
			stop_it != m_stopbut_vec.end(); ++stop_it, ++userid )
			{
				if( (*stop_it)->IsMouseOver() )
				{
					stopmarker = userid;
					break;
				}
			}

			//roll the dice
			if( stopmarker != -1 )
			{
				pMusic_man->play("BTN_C");


				//if dice haven't been rolled yet
				if( !m_dicerolls[stopmarker]->ResultsShown() )
				{
					bool rollagain;

					//while the number being rolled already exists
					do
					{
						rollagain = false;
						m_dicerolls[stopmarker]->RollDice();

						//see if the number already exists
						for( std::vector<int>::iterator i = m_values_taken.begin(); i != m_values_taken.end(); ++i )
						{
							if( *i == m_dicerolls[stopmarker]->GetTotalValue() )
							{	rollagain = true; break;	}
						}

					}while ( rollagain );
					
					m_values_taken.push_back(m_dicerolls[stopmarker]->GetTotalValue());

					++m_roll_count;
				}
			}
		}
	}


	//check for if all dice have been rolled
	if( m_roll_count == m_players.size() )
	{

		//if determining the play order was successful, move on
		if( ! results_shown)
		{
			results_shown = DeterminePlayerOrder();
			if (results_shown) {
				m_title.SetString("The order is...");
				
				// reorder player names
				float base_x = 300.0f;
				float base_y = 75.0f;
				float delta_x = 0.0f;
				float delta_y = -120.0f;
				m_animation_speed.resize(m_players.size());
				for (int i = 0; i < m_players.size(); ++i) {

				}
			}
		}

		//interpolate value over time
		if( interp_cntr < INTERPMAX )
			//have the delay counter increment
			interp_cntr += 4.0f*t;
		else 
			interp_cntr = INTERPMAX;



		////render the icons for the play order once it's decided
		//for( std::vector<Box_2D*>::iterator tokres_it = m_tok_results_icons.begin(); 
		//tokres_it != m_tok_results_icons.end(); ++tokres_it )
		//{
		//	(*tokres_it)->SetPosX(230.0f - 300.0f *(1.0f - GetInterp()) );
		//}
	}

}

/***********************************
PlayOrderRoll Render

Author: Jamie Gault
***********************************/
void PlayOrderRoll::Render()
{
	float base_y = 100.0f;
	float base_x = 500.0f;
	float delta_y = 100.0f;

	//drop a gel on the screen
	RenderScreenGel( 1.0f, 0.0f, 1.0f,0.5f);

	glColor3f(1.0f, 1.0f, 1.0f);
	//render the frame background
	RenderCoordScreenImage( pTexture_man->GetTexture(std::string("MENU_BACKGROUND")), 
					50.0f*SCREEN_X_RATIO, 
					50.0f*SCREEN_Y_RATIO, 
					(DATA("SCREEN_WIDTH") -50.0f), 
					(DATA("SCREEN_HEIGHT") -50.0f));

	m_title.Render();

	float anim_factor = GetInterp();
	if (anim_factor > 1) {
		anim_factor = 1;
	}

	//render the icons next to the rolling dice
	for (size_t i = 0; i < m_tok_roll_icons.size(); ++i) {
		Box_2D * box = m_tok_roll_icons[i];
		if (results_shown) {
			float y = base_y + i * delta_y + m_animation_speed[i] * anim_factor;
			box->SetPosY(y);

		}
		box->Render();
	}

	for (size_t i = 0; i < m_titlebox.size(); ++i) {
		TextBox * tb = m_titlebox[i];
		if (results_shown) {
			float y = base_y + i * delta_y + 25.0f + m_animation_speed[i] * anim_factor;
			tb->SetPosY(y);
			// player order will slide from the left
			TextBox * ob = m_orderbox[i];
			float goal_x = tb->GetPosX() - ob->GetBoxWidth() - 20.0f;
			float x = (goal_x + 100.0f) * anim_factor - 100.0f;
			ob->SetPosX(x);
			ob->SetPosY(y);
			ob->Render();
		}
		tb->Render();
	}

	//render the dice and the buttons
	for (size_t i = 0; i < m_players.size(); ++i) {
		DiceDisplay * dd = m_dicerolls[i];
		if (results_shown) {
			float y = base_y + i * delta_y + 10.0f + m_animation_speed[i] * anim_factor;
			dd->SetPosY(y);
		}
		m_dicerolls[i]->Render2D();

		//render the button if the dice are still rolling
		if( ! m_dicerolls[i]->ResultsShown() ) {
			m_stopbut_vec[i]->Render();
		}
	}

	//render the results
	if( results_shown ) {
		m_footer.Render();
	}
	
	//render the icons for the play order once it's decided
	for( std::vector<Box_2D*>::iterator tokres_it = m_tok_results_icons.begin()
		; tokres_it != m_tok_results_icons.end()
		; ++tokres_it )
	{
		(*tokres_it)->Render();
	}
}



/***********************************
PlayOrderRoll Load

Author: Jamie Gault
***********************************/
void PlayOrderRoll::Load(PlayerSetup::list_t & players)
{
	m_players.swap(players);

	//iterate through copying each name
	float base_x = 500.0f;
	float base_y = 100.0f;
	float delta_x = 0.0f;
	float delta_y = 0.0f;
	for (size_t i = 0; i < m_players.size(); ++i, delta_y += 100.0f) {
		PlayerSetup & ps = m_players[i];

		//load title for the user.
		TextBox * tb = new TextBox( pText_man,  ps.m_name
			//, 610.0f * SCREEN_X_RATIO, (170.0f + delta_y) * SCREEN_Y_RATIO
			, 0.0f, base_y + delta_y + 25.0f
			, 1000.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO );
		tb->SetPosX(base_x - tb->GetBoxWidth() - 20.0f);
		tb->SetBackground(pTexture_man->GetTexture("TID_TEXTBG"));
		m_titlebox.push_back( tb );

		TextBox * ob = new TextBox( pText_man, std::string()
			//, 610.0f * SCREEN_X_RATIO, (170.0f + delta_y) * SCREEN_Y_RATIO
			, 0.0f, base_y + delta_y + 25.0f
			, 1000.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO );
		m_orderbox.push_back(ob);

		//make icons corresponding to each player
		m_tok_roll_icons.push_back( new Box_2D( pTexture_man->GetTexture(std::string("icon_")+ps.m_token_name)
			//, 565.0f, (150.0f + delta_y)*SCREEN_Y_RATIO
			, base_x + delta_x, base_y + delta_y
			, 100.0f, 100.0f ));

		//load dice display
		DiceDisplay* dice = new DiceDisplay(false
			//, 600.0f*SCREEN_X_RATIO, (200.0f + delta_y)*SCREEN_Y_RATIO );
			, base_x + delta_x + 100.0f, base_y + delta_y + 10.0f);
		dice->SetToRoll();
		m_dicerolls.push_back(dice);

		//load button for rolling
		Answer* ans = new Answer( pText_man, std::string("Stop!") 
			//, 685.0f + 100.0f*SCREEN_X_RATIO, (200.0f + delta_y)*SCREEN_Y_RATIO
			, base_x + delta_x + 285.0f, base_y + delta_y + 25.0f
			, 1000.0f); 
		m_stopbut_vec.push_back(ans);
	}
}


/***********************************
PlayOrderRoll DeterminePlayerOrder

Author: Jamie Gault
***********************************/
bool PlayOrderRoll::DeterminePlayerOrder()
{
	//if the play order list hasn't been loaded yet
	if( ! ol_loaded )
	{
		typedef std::map<int, int, std::greater<int> > rollorder_map_t;
		rollorder_map_t rollorder;
		for(int i = 0; i < m_players.size() ; ++i) {
			m_players[i].m_rollvalue = m_dicerolls[i]->GetTotalValue();
			rollorder.insert(std::make_pair(m_dicerolls[i]->GetTotalValue(), i));
		}

		m_animation_speed.resize(m_players.size());
		rollorder_map_t::iterator roll_it = rollorder.begin();
		for (int new_index = 0; new_index < m_players.size(); ++new_index, ++roll_it) {
			int old_index = roll_it->second;
			m_animation_speed[old_index] = 100.0f * (new_index - old_index);
			// update the title box with just player name
			std::ostringstream oss;
			oss << (new_index + 1);
			m_orderbox[old_index]->SetString(oss.str(), true);
		}

		std::sort(m_players.begin(), m_players.end());
			
		ol_loaded = true;
	}

	//if all players have a unque play order
	if( m_roll_count == m_players.size())
		return true;
	else
		return false;
}





/***********************************
ScoreDisplay Constructor

Author: Jamie Gault
***********************************/
ScoreDisplay::ScoreDisplay(std::vector<player*> &playerlist ): active_player(0), Anim_on(false)
{
	
	//render the highlight over the person who's turn it is
	turnHL = Box_2D( pTexture_man->GetTexture( "TurnHighLigh" ),  
			(DATA("ICON_POS_X") + DATA("ICON_SPACE_X")*(float)active_player - 60.0f), 
			DATA("ICON_POS_Y")-40.0f, 
			128.0f*SCREEN_X_RATIO, 128.0f*SCREEN_Y_RATIO);


	float xpos = DATA("ICON_POS_X");
	float ypos = DATA("ICON_POS_Y");
	float space = DATA("ICON_SPACE_X");
	float count = 0.0f;
	//iterate through all the player setups and load in the icons
	for( std::vector<player*>::iterator psi = playerlist.begin(); psi != playerlist.end(); ++psi, ++count)
	{
			
		score.push_back(0);

	
		//setup the box that holds the score
		TextBox* scoreb = new TextBox(pText_man, (xpos + space*count), ypos, 
								60.0f, 60.0f, 
								10.0f, 35.0f, 35.0f);

		scoreb->SetString( "00" );

		//center the text is centered within the background box
		scoreb->SetBGOffset( -0.0f, 1, 0);
		scoreb->SetBGOffset( -5.0f, 0, 0);
		scoreb->SetBGOffset( -20.0f, 0, 1);
		scoreb->SetBGOffset( -10.0f, 1, 1);

		scoreb->SetBackground( pTexture_man->GetTexture(std::string("score_frame")));
		score_boxes.push_back( scoreb );


		//determine the name of the texture id
		std::string iconname("icon_");
		iconname.append((*psi)->getName());

		icon_boxes.push_back( new Box_2D( pTexture_man->GetTexture( iconname ),  
							(xpos + space*count - 100.0f)*SCREEN_X_RATIO, ypos-50.0f, 
							128.0f*SCREEN_X_RATIO, 128.0f*SCREEN_Y_RATIO) );
	}

}



/***********************************
ScoreDisplay Destructor

Author: Jamie Gault
***********************************/
ScoreDisplay::~ScoreDisplay()
{
	//delete all the score boxes
	for(std::vector<TextBox*>::iterator sb = score_boxes.begin(); sb != score_boxes.end(); ++sb )
	{
		delete (*sb);
	}

	//delete all the icons
	for(std::vector<Box_2D*>::iterator box = icon_boxes.begin(); box != icon_boxes.end(); ++box )
	{
		delete (*box);
	}
}


/***********************************
ScoreDisplay Update

Author: Jamie Gault
***********************************/
void ScoreDisplay::Update(float t)
{

	if( Anim_on )
	{
		accum_time += t;

		//if the time has over run
		if(accum_time > DATA("SCORE_TIME_LIMIT"))
		{
			Anim_on = false;

			score[anim_pl_id-1] += anim_score;
			std::stringstream s;

			//make it so there are always double digits for the score board
			if( score[anim_pl_id-1] >= 0 && score[anim_pl_id-1] < 10 )
				s<<'0';

			s<< score[anim_pl_id-1];
			(score_boxes[anim_pl_id-1])->SetString( s.str() );
		}
		else
		{
			anim_box.SetPosY( DATA("ICON_POS_Y")- 100.0f*accum_time/DATA("SCORE_TIME_LIMIT") );	
		}
	}

	turnHL.angle += t;

	turnHL.SetPosX((DATA("ICON_POS_X") + DATA("ICON_SPACE_X")*(float)active_player - 60.0f)*SCREEN_X_RATIO);

}



/***********************************
ScoreDisplay Render

Author: Jamie Gault
***********************************/
void ScoreDisplay::Render()
{
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f);

	turnHL.Render();

	//render the all the score boxes
	for(std::vector<TextBox*>::iterator sb = score_boxes.begin(); sb != score_boxes.end(); ++sb )
	{
		(*sb)->Render();
	}

	//render all of the icons
	for(std::vector<Box_2D*>::iterator box = icon_boxes.begin(); box != icon_boxes.end(); ++box )
	{
		(*box)->Render();
	}

	if( Anim_on) 
		anim_box.Render();
}


/***********************************
ScoreDisplay AddPoints

Author: Jamie Gault
***********************************/
void ScoreDisplay::AddPoints(int player_id, int points)
{

	if( points == 0 )
		return;
	

	accum_time = 0.0f;
	anim_pl_id = player_id;
	anim_score = points;
	Anim_on = true;

	std::stringstream scorestring;


	//generate the string for the texture
	if( points >= 0 )
		scorestring << "score_+" << points;
	else
		scorestring << "score_-" << -points;

	int i= pTexture_man->GetTexture(scorestring.str());

	anim_box = Box_2D( i, (DATA("ICON_POS_X") + DATA("ICON_SPACE_X")*(float)active_player)-50.0f, 
								DATA("ICON_POS_Y") , 100.0f, 100.0f);


	
}



/***********************************
ScoreDisplay AddPoints

Author: Jamie Gault
***********************************/
void ScoreDisplay::Reset()
{
	int id = 0;
	for( std::vector<int>::iterator sc = score.begin(); sc!= score.end() ; ++sc, ++id)
	{		
		*sc = 0;
		std::stringstream s;
		s<< "0"<<*sc;
		(score_boxes[id])->SetString( s.str() );
	}

}



/***********************************
EndDisplay Constructor

Author: Jamie Gault
***********************************/
EndDisplay::EndDisplay( WinData & win_data )
{

		std::stringstream title;

		if( win_data.turn == win_data.turn_limit )
			title << "The " << win_data.turn_limit << " rounds are up!" ;
		else
			title << "We have a winner!   ";

	

		//add to the title to the list of things to be rendered
		txtbox_vec.push_back(new TextBox(pText_man, title.str() , 
							100.0f*SCREEN_X_RATIO, 200.0f*SCREEN_Y_RATIO, 
							500.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO ));
	
		

		int size = (int)win_data.winner.size();

		//if there's more than one winner
		if( size > 1 )
		{
			std::stringstream win; 

			win << "  Tie! ";

			TextBox* t = new TextBox(pText_man, win.str() , 
								300.0f, 300.0f, 
								700.0f, 10.0f, 30, 30 );
			txtbox_vec.push_back(t);
			win.flush();

			player* end = win_data.winner.back();


			float x = 30.0f + t->GetBoxWidth();
			
			//list out all winners
			for( std::vector<player*>::iterator p = win_data.winner.begin(); p != win_data.winner.end() ; ++p )
			{
				std::stringstream str;

				//if this is the last element in the list
				if( (*p)->getID() == end->getID() )
				{
					str << " win with a score of " << (*p)->getScore() << "!";
				}
				else
				{					
					str << "and";
				}

				tok_ics.push_back(new Box_2D( pTexture_man->GetTexture(std::string("icon_") + (*p)->getName()), 
										DATA("END_TIELIST_POSX") + x, 
										DATA("END_TIELIST_POSY") + DATA("END_ICON_OFFSETY"), 
										DATA("END_TIEICON_WD"),DATA("END_TIEICON_HT")));
				x += 80.0f;


				TextBox* box = new TextBox(pText_man, str.str() , 
												DATA("END_TIELIST_POSX") + x, DATA("END_TIELIST_POSY"), 
												700.0f, 10.0f, 30, 30 );

				txtbox_vec.push_back(box);

				//increment by the width of the box
				x +=  box->GetBoxWidth(); 
			}
		}
		else
		{
			float x = 0.0f;

			tok_ics.push_back(new Box_2D( pTexture_man->GetTexture(std::string("icon_") + (*win_data.winner.begin())->getName()), 
										DATA("END_ONEWIN_POSX") + DATA("END_ICON_OFFSETX") + x, 
										DATA("END_ONEWIN_POSY") + DATA("END_ICON_OFFSETY"), 
										DATA("END_WINICON_WD"),DATA("END_WINICON_HT") ));
			x += 120.0f;

			std::stringstream win; 
			win << " wins with a score of " << (*win_data.winner.begin())->getScore() << "!";

			TextBox* txt = new TextBox(pText_man, win.str() , 
								DATA("END_ONEWIN_POSX") + x, 
								DATA("END_ONEWIN_POSY"), 
								700.0f, 13.0f, 40, 40 );
			//add text box for the end of the sentence
			txtbox_vec.push_back(txt);

			txt->SetBGOffset(-70.0f, 0, 0);

			
		}
	

}




/***********************************
EndDisplay Destructor

Author: Jamie Gault
***********************************/
EndDisplay::~EndDisplay()
{

	for( std::vector<TextBox*>::iterator txt = txtbox_vec.begin(); txt != txtbox_vec.end() ; ++txt )
	{
		delete(*txt);
	}

	for( std::vector<Box_2D*>::iterator tok = tok_ics.begin(); tok != tok_ics.end() ; ++tok )
	{
		delete(*tok);
	}

}


/***********************************
EndDisplay Update

Author: Jamie Gault
***********************************/
void EndDisplay::Update(float t)
{
	//don't really need it to do much

}

/***********************************
EndDisplay Render

Author: Jamie Gault
***********************************/
void EndDisplay::Render()
{

	


	for( std::vector<TextBox*>::iterator txt = txtbox_vec.begin(); txt != txtbox_vec.end() ; ++txt )
	{
		(*txt)->Render();
	}


	for( std::vector<Box_2D*>::iterator tok = tok_ics.begin(); tok != tok_ics.end() ; ++tok )
	{
		(*tok)->Render();
	}

}