#include "engine/core/all.h"
#include "busted/Game_Manager/Game_Manager.h"

Display * display = NULL;
extern Game_Manager *Game_Man;
extern GraphicsEngine *Graph_En;
extern Texture_Manager* pTexture_man;

Display::Display()
	: FADE_S(FADE_IN)
	, SPLASH_S(SPLASH_COPY)
	, splashFade(DATA("SPLASH_IN"))
	, splashWait(DATA("SPLASH_WAIT"))
	, splashFadeRem(1.0f)
	, splashWaitRem(1.0f)
	, width(DATA("SCREEN_WIDTH"))
	, height(DATA("SCREEN_HEIGHT"))
{}

//Make a grid (for orientation)
void Display::drawGrid(void)
{
	glColor3f(1.0, 1.0, 1.0);
	// draw grid
	glBegin(GL_LINES);
	for(int i=-10;i<=10;++i)
	{
		glVertex3f(i,0,-10);
		glVertex3f(i,0,10);

		glVertex3f(10,0,i);
		glVertex3f(-10,0,i);
	}
	glEnd();
}

void Display::drawScreenGrid()
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);

	for( float i = 0; i < 25; ++i)
	{
		for( float j = 0; j < 20 ; ++j)
		{
			glVertex2f(i*50.0f*SCREEN_X_RATIO,0.0f);
			glVertex2f(i*50.0f*SCREEN_X_RATIO,768.0f*SCREEN_Y_RATIO);

			glVertex2f(0.0f,i*50.0f*SCREEN_Y_RATIO);
			glVertex2f(1024.0f*SCREEN_X_RATIO,i*50.0f*SCREEN_Y_RATIO);
		}
	}
	glEnd();

	TextBox gridpos(Game_Man->textboxdata.text_m,  0.0f, 0.0f, 30.0f, 3.0f, 15.0f, 15.0f);

	for( float i = 0; i < 25; ++i)
	{
		for( float j = 0; j < 20 ; ++j)
		{
			//render the x value
			std::stringstream xpos;
			xpos<< i*50.0f;
			gridpos.SetString(xpos.str());

			gridpos.SetPosX( i*50.0f*SCREEN_X_RATIO );
			gridpos.SetPosY( 0.0f );
			gridpos.Render();

			gridpos.SetPosY( i*50.0f*SCREEN_Y_RATIO );
			gridpos.SetPosX( 0.0f );
			gridpos.Render();
		}
	}
}

//Default glEnables and other GL settings
void Display::LoadGLDefaults()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);						// Enable Blending
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, .1f);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); //set depth test to be less than or equal to

    glEnable(GL_CULL_FACE);
    glPolygonMode (GL_FRONT_AND_BACK , GL_FILL);

	if(DATA("light_on"))
	{
		glEnable(GL_LIGHTING);

		if(DATA("light_0"))
			glEnable(GL_LIGHT0);
		GLfloat specular[] = {0.1, 0.1, 0.1, 0.1};
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		GLfloat ambient[] = { 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glShadeModel(GL_SMOOTH);

		if(DATA("light_1"))
			glEnable(GL_LIGHT1);
		GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT1,GL_DIFFUSE, diffuse);
		glShadeModel(GL_SMOOTH);

		if(DATA("light_2"))
			glEnable(GL_LIGHT2);
		/*ambient[1] = 0.0f;
		ambient[2] = 0.0f;*/
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT2,GL_DIFFUSE, diffuse);
		glShadeModel(GL_SMOOTH);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	reshape(width, height);
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

void Display::UpdateLighting()
{
	if(DATA("light_on"))
	{
		glEnable(GL_LIGHTING);

		if(DATA("light_0"))
			glEnable(GL_LIGHT0);
		else
			glDisable(GL_LIGHT0);

		if(DATA("light_1"))
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);

		if(DATA("light_2"))
			glEnable(GL_LIGHT2);
		else
			glDisable(GL_LIGHT2);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}

	GLfloat pos[] = { 0.0f, DATA("lz"), 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	GLfloat pos1[] = { DATA("lx"), 10.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);

	GLfloat pos2[] = { 0.0f, 10.0f, DATA("lz"), 0.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, pos2);
}

void Display::UpdateCompanyScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ViewOrtho();

	if(DATA("debug"))
		if(pInput->PollMouseBut(MLEFT))
			FADE_S = FADE_OUT;

	//load up the main engine
	if( (FADE_S == FADE_NONE && SPLASH_S == SPLASH_NONE) )
		Game_Man->LoadStart();

	//Draw one of the splash screens
	if(SPLASH_S == SPLASH_COPY)
		RenderFullScreenImage( pTexture_man->GetTexture("copyright") );
	else if(SPLASH_S == SPLASH_LOGO)
		RenderFullScreenImage( pTexture_man->GetTexture("splash") );

	//Draw the fade amount
	if(FADE_S == FADE_IN)
	{
		if(splashFadeRem <= 0.0f)
		{
			splashFadeRem = 0.0f;
			FADE_S = FADE_WAIT;
		}
		//RenderScreenGel( 0.0f, 0.0f, 0.0f, splashFadeRem );
		splashFadeRem -= 1.0f/(splashFade*60.0f);
	}
	else if( FADE_S == FADE_WAIT )
	{
		if(SPLASH_S == SPLASH_COPY)
		{
			if(splashWaitRem <= 0.0f)
			{
				splashWaitRem = 0.0f;
				FADE_S = FADE_OUT;
			}
			splashWaitRem -= 1.0f/(splashWait*60.0f);
		}
		else
		{
			if(pInput->PollMouseBut(MLEFT))
				FADE_S = FADE_OUT;
		}
	}
	else if( FADE_S == FADE_OUT )
	{
		if(splashFadeRem >= 1.0f)
		{
			splashFadeRem = 1.0f;
			FADE_S = FADE_NONE;
		}
		//RenderScreenGel( 0.0f, 0.0f, 0.0f, splashFadeRem );
		splashFadeRem += 1.0f/(splashFade*60.0f);
	}
	else
	{
		if(SPLASH_S == SPLASH_COPY)
		{
			FADE_S = FADE_IN;
			SPLASH_S = SPLASH_LOGO;
		}
		else
			SPLASH_S = SPLASH_NONE;
	}

	//Finalize GL
	Graph_En->RenderScreen();

	ViewPerspective();

		//clear input data
	pInput->ClearKeyboardData();
	pInput->ClearMouseData();
}

void Display::display(void)
{
	//Cap the frame limit
	if(!Game_Man->UpdateFramerate())
		return;

	//render the company screen until the user clicks
	if( Game_Man->GetExternalGS() == ES_PREGAME_LOAD)
	{
		UpdateCompanyScreen();
		return;
	}

	//Update game-settings
	//Players are told to move here
	Game_Man->Update(Game_Man->framescale);

	//Clear buffers, set GLUT things
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Modify the view based on the camera
	Camera->render();

	UpdateLighting();

	//render nothing while displaying a question
	if( (Game_Man->GetInGameState() != IGS_QUESTION ||
		(Game_Man->m_cam_man.GetTransInterp() != 1.0f && Game_Man->m_cam_man.GetCamFollowType() ==CFT_Q_TRANS_IN )) &&
		Game_Man->GetInGameState() != IGS_RESULT)
	{
		//!!!!!!!!!!!!!!!!!!!!
		//MOST IMPORTANT STEP:
		//Main draw-loop
		for(std::vector<obj*>::iterator oi = objStack.begin(); oi < objStack.end(); ++oi)
		{
			if( (*oi)->isLightOn() )
			(*oi)->render();
		}// for objStack

		glDisable(GL_LIGHTING);

		//render stuff without lights
		for(std::vector<obj*>::iterator oi = objStack.begin(); oi < objStack.end(); ++oi)
		{
			if( !(*oi)->isLightOn() )
				(*oi)->render();
		}// for objStack

		//if it's time to roll the dice
		//if(  Game_Man->GetInGameState() == IGS_DICE || Game_Man->GetInGameState() == IGS_MOVE )
		{
			Sprite roll( pTexture_man->GetTexture("ROLL_AGAIN" ), 40.0f, 7.0f, -40.0f, 9.0f, 9.0f);
			Sprite lose( pTexture_man->GetTexture("LOSEATURN" ), -40.0f, 7.0f, -40.0f, 9.0f, 9.0f);
			//Sprite sprite( pTexture_man->GetTexture("LOSEATURN" ), 0.0f, 30.0f, 0.0f, 20.0f, 20.0f);

			//render roll again signs
			roll.SetOffset( 0.0f, roll.GetHeight()/2.0f);
			roll.Render();
			roll.pos.x *= -1.0f;
			roll.pos.z *= -1.0f;
			roll.Render();

			//render lose a turn signs
			lose.SetOffset( 0.0f, lose.GetHeight()/2.0f);
			lose.Render();
			lose.pos.x *= -1.0f;
			lose.pos.z *= -1.0f;
			lose.Render();
		}

		glColor3f(1.0f,1.0f,1.0f);	//reset the color just in-case the next object doesn't have one
	}
	else
		glDisable(GL_LIGHTING);

	//Render the game updates
	Game_Man->Render();

	if( DATA("screengrid") )
	{
		//draw screen grid
		ViewOrtho();
			drawScreenGrid();
		ViewPerspective();
	}

	glEnable(GL_DEPTH_TEST);

	Game_Man->ChooseTokenRender();

	//Finalize GL
	Graph_En->RenderScreen();

	//clear input data
	pInput->ClearKeyboardData();
	pInput->ClearMouseData();
}

void Display::reshape(int w, int h)
{
	if (h == 0) {
		// avoid division by zero
		h = 1;
	}
	width = w;
	height = h;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				// Reset The Current Viewport
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	gluPerspective(45, (float)width/(float)height, 10.0f, 1100.0f);
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix

	pInput->SetViewportRatio(w / (float)DATA("SCREEN_WIDTH"), h / (float)DATA("SCREEN_HEIGHT"));
}