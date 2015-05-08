#include "Menu.h"
#include "busted.all.h"
#include "player/player.h"

extern Input *pInput;
extern Mentor_Manager *pMentor_man;
extern Texture_Manager* pTexture_man;

//m_state = SMS_GAME_SETUP - Game Setup Screen
//
//m_state = SMS_PLAYER_TOKEN - Select Token
//
//m_state = SMS_PLAYER_INFO - Choose Mentor, Sound Track and Gender
//
//- Select Token
//
//- Ready to begin?
//
//SMS_Mentor already exists
//
//
//SetupMenu::SetupMenu - sets the positions of the menu items, but does not display them
//SetupMenu::Update - Checks when a mouse is clicked
//SetupMenu::Render - Renders the items - positions set in setupmenu

PlayerSetup::PlayerSetup( int player_id, pina* piece, MENTOR_ID men_id, int soundset, GENDER gender)
	: m_ID( player_id )
	, m_boardpiece(piece)
	, m_mentor(men_id)
	, m_soundset(soundset)
	, m_gender(gender)
	, m_rollvalue(0)
{}

/***********************************
IncDecArrows Constructor

Author: Jamie Gault
***********************************/
IncDecArrows::IncDecArrows():	m_value(0), m_pressed(AA_NONE), m_max(0), m_min(0), m_inc(0),m_defvalue(0)
{}

/***********************************
IncDecArrows Constructor

Author: Jamie Gault
***********************************/
IncDecArrows::IncDecArrows( unsigned int defvalue, float xL, float yL, float xR, float yR,
						   unsigned min, unsigned int max, unsigned int inc):
								m_value(defvalue), m_pressed(AA_NONE), m_min(min), m_max(max), m_inc( inc), m_defvalue(defvalue)
{
	m_Left_but = Box_2D( BT_SQUARE_MIRROR, xL, yL, DATA("ARROW_WID"), DATA("ARROW_HT"));

	m_Right_but = Box_2D( BT_SQUARE, xR, yR, DATA("ARROW_WID"), DATA("ARROW_HT"));
	m_Right_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW") );
}

/***********************************
IncDecArrows Constructor

Author: Jamie Gault
***********************************/
IncDecArrows::IncDecArrows( IncDecArrows &arrow )
{
	(*this) = arrow;
}

/***********************************
IncDecArrows operator=

Author: Jamie Gault
***********************************/
IncDecArrows& IncDecArrows::operator=( IncDecArrows &arrows)
{
	m_value = arrows.m_value;
	m_max = arrows.m_max;
	m_min = arrows.m_min;
	m_inc = arrows.m_inc;
	m_pressed = arrows.m_pressed;

	m_Left_but = arrows.m_Left_but;
	m_Right_but = arrows.m_Right_but;
	m_defvalue = arrows.m_defvalue;

	return *this;
}

/***********************************
IncDecArrows Update

Author: Jamie Gault
***********************************/
void IncDecArrows::Update()
{
	m_pressed = AA_NONE;

	//change backgrounds
	if( m_Left_but.IsMouseOver() && m_value != m_min )
	{
		//rollover
		////pMusic_man->play("ARW_R");
		m_Left_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_OVER") );
	}
	else
		m_Left_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_IDLE") );

	//change backgrounds
	if( m_Right_but.IsMouseOver() && m_value != m_max )
	{
		//rollover
		////pMusic_man->play("ARW_R");
		m_Right_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_OVER") );
	}
	else
		m_Right_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_IDLE") );

	if( pInput->PollMouseBut(MLEFT) )
	{
		if( m_Left_but.IsMouseOver() )
		{
			if( m_value > m_min )
			{
				m_value -= m_inc;
				m_pressed = AA_DEC;
				m_Left_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_TRIG") );

				//click
				pMusic_man->play("ARW_C");
			}
		}
		else if( m_Right_but.IsMouseOver() )
		{
			if( m_value < m_max )
			{
				m_value += m_inc;
				m_pressed = AA_INC;
				m_Right_but.SetBackGround( pTexture_man->GetTexture("TID_ARROW_TRIG") );

				//click
				pMusic_man->play("ARW_C");
			}
		}
	}
}

/***********************************
IncDecArrows Render

Author: Jamie Gault
***********************************/
void IncDecArrows::Render()
{
	if( m_value == m_min )
		glColor4f(0.50f, 0.50f, 0.50f, 1.0f);

	m_Left_but.Render();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if( m_value == m_max )
		glColor4f(0.50f, 0.50f, 0.50f, 1.0f);

	m_Right_but.Render();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/***********************************
IncDecArrows IsPressed

Author: Jamie Gault
***********************************/
ARROW_ACTION IncDecArrows::IsPressed()
{
	return m_pressed;
}

/***********************************
IncDecArrows SetValue

Author: Jamie Gault
***********************************/
void IncDecArrows::SetValue(int i )
{
	//set value as long as within range
	if( (unsigned)i >= m_min && (unsigned)i <= m_max )
	{
		m_value = i;
	}
}

/***********************************
IncDecArrows Reset

Author: Jamie Gault
***********************************/
void IncDecArrows::Reset()
{
	m_value = m_defvalue;
	Update();
}

/***********************************
NumberMenu Constructor

Author: Jamie Gault
***********************************/
NumberMenu::NumberMenu()
{}

/***********************************
NumberMenu Constructor

Author: Jamie Gault
***********************************/
NumberMenu::NumberMenu( Text_Manager* tm, std::string identifier, unsigned defvalue,
					   unsigned min, unsigned max, unsigned int inc, float x, float y, bool displaymax )
{
	m_displaymax = displaymax;

	m_identifier = TextBox(tm, identifier, x, y,
							500.0f, 10.0f, 30, 30 );

	m_identifier.SetBackground(0);

	//determine string length to use for the number box
	std::stringstream s;
	if( m_displaymax )
		s<< max<< " out of " << max;
	else
		s<< max;

	m_value_display = TextBox( tm, s.str(), x*SCREEN_X_RATIO + m_identifier.GetBoxWidth()+20.0f,
								y*SCREEN_Y_RATIO, 500.0f, 15*SCREEN_X_RATIO, 40*SCREEN_X_RATIO, 40*SCREEN_Y_RATIO );
	m_value_display.SetBGOffset(0.0f,1,1);

	//determine the back ground to use for the display value
	if( m_value_display.GetBoxHeight() > 128.0f || m_value_display.GetBoxWidth() > 256.0f )
		m_value_display.SetBackground(pTexture_man->GetTexture("TID_TEXTBG_L"));
	else
		m_value_display.SetBackground(pTexture_man->GetTexture("TID_TEXTBG"));

	m_arrows = IncDecArrows( defvalue, m_value_display.GetPosX() ,
									m_value_display.GetPosY() + m_value_display.GetBoxHeight(),
									m_value_display.GetPosX() + DATA("ARROW_WID"),
									m_value_display.GetPosY() + m_value_display.GetBoxHeight(), min, max, inc );

	UpdateValueDisplay();
}

/***********************************
NumberMenu Constructor

Author: Jamie Gault
***********************************/
NumberMenu::NumberMenu( NumberMenu& menu )
{
	(*this) = menu;
}

/***********************************
NumberMenu operator=

Author: Jamie Gault
***********************************/
NumberMenu& NumberMenu::operator=( NumberMenu& menu )
{
	m_identifier =  menu.m_identifier;

	m_value_display =  menu.m_value_display;

	m_arrows = menu.m_arrows;

	m_displaymax = menu.m_displaymax;

	return *this;
}

/***********************************
NumberMenu Update

Author: Jamie Gault
***********************************/
void NumberMenu::Update()
{
	m_arrows.Update();

	if( m_arrows.IsPressed() != AA_NONE )
	{
		UpdateValueDisplay();
	}
}

/***********************************
NumberMenu Render

Author: Jamie Gault
***********************************/
void NumberMenu::Render()
{
	if (isVisible()) {
		m_arrows.Render();
		m_identifier.Render();
		m_value_display.Render();
	}
}

/***********************************
NumberMenu UpdateValueDisplay

Author: Jamie Gault
***********************************/
void NumberMenu::UpdateValueDisplay()
{
	std::stringstream s;
	if( m_displaymax )
		s<< m_arrows.m_value << " out of " << m_arrows.m_max;
	else
		s<< m_arrows.m_value;

	m_value_display.SetString(s.str());
}

/***********************************
EditMenu Constructor

Author: Vigen Issahhanjan
***********************************/
EditMenu::EditMenu()
{}

EditMenu::EditMenu(Text_Manager * tm, std::string title, float x, float y, int max_length)
{
	m_title_box = TextBox( tm, title, x*SCREEN_X_RATIO, y*SCREEN_Y_RATIO,
				500.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO);
	m_title_box.SetBackground(0);

	m_input_box = TextBox( tm,
			std::string(max_length, 'm'),
			m_title_box.GetPosX() + m_title_box.GetBoxWidth() + 20,
			m_title_box.GetPosY(),
			500.0f,
			10.0f*SCREEN_X_RATIO,
			30*SCREEN_X_RATIO,
			30*SCREEN_Y_RATIO);
	m_input_box.SetBackground(pTexture_man->GetTexture("TID_TEXTBG_L"));
	m_input_box.SetString("");
	m_input_box.EnableCursor(true);

	m_max_text_length = max_length;
}

void EditMenu::Reset()
{
	m_input_box.SetString("");
}

void EditMenu::SetText(const std::string & text)
{
	return m_input_box.SetString(text);
}

std::string EditMenu::GetText()
{
	return m_input_box.GetString();
}

void EditMenu::Update()
{
	std::string text = m_input_box.GetString();
	while (char c = pInput->NextKey()) {
		if (c == 0x08) { // backspace
			if (!text.empty()) {
				text.resize(text.size() - 1);
			}
		} else if (c >= 32 && c < 127) {
			if (text.size() < m_max_text_length) {
				text += c;
			}
		}
	}
	m_input_box.SetString(text);
}

void EditMenu::Render()
{
	if (isVisible()) {
		m_title_box.Render();
		m_input_box.Render();
	}
}

/***********************************
WordMenu Constructor

Author: Jamie Gault
***********************************/
WordMenu::WordMenu()
{}

/***********************************
WordMenu Constructor

Author: Jamie Gault
***********************************/
WordMenu::WordMenu( Text_Manager* tm, std::string identifier, std::list<std::string> &options, float x, float y  )
{
	m_identifier = TextBox(tm, identifier, x*SCREEN_X_RATIO, y*SCREEN_Y_RATIO,
							500.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO);

	/*if( m_identifier.GetBoxHeight() > 128.0f || m_identifier.GetBoxWidth() > 256.0f )
		m_identifier.SetBackground(pTexture_man->GetTexture("TID_TEXTBG_L"));
	else
		m_identifier.SetBackground(pTexture_man->GetTexture("TID_TEXTBG"));
	*/

	m_identifier.SetBackground(0);

	m_options.assign( options.begin(),  options.end());
	m_cur_option = m_options.begin();

	//find the longest string to enter into the box
	std::string* s = &(*m_options.begin());
	for( std::list<std::string>::iterator strit = m_options.begin(); strit != m_options.end(); ++strit )
	{
		if( s->length() < (*strit).length() )
			s = &(*strit);
	}

	m_option_display = TextBox( tm, *s , x + m_identifier.GetBoxWidth()+20.0f, y,
								600.0f, 10.0f, 30, 30 );

	m_option_display.SetBGOffset(0.0f,1,1);

	if( m_option_display.GetBoxHeight() > 128.0f || m_option_display.GetBoxWidth() > 256.0f )
		m_option_display.SetBackground(pTexture_man->GetTexture("TID_TEXTBG_L"));
	else
		m_option_display.SetBackground(pTexture_man->GetTexture("TID_TEXTBG"));

	m_arrows = IncDecArrows( 0, m_option_display.GetPosX() /*- m_option_display.GetBoxWidth()/4*/,
									m_option_display.GetPosY() + m_option_display.GetBoxHeight(),
									m_option_display.GetPosX() + DATA("ARROW_WID"),
									m_option_display.GetPosY() + m_option_display.GetBoxHeight(),
									0, (int)m_options.size()-1);

	UpdateDisplay();
}

/***********************************
WordMenu Constructor

Author: Jamie Gault
***********************************/
WordMenu::WordMenu( WordMenu& menu )
{
	(*this) = menu;
}

/***********************************
WordMenu operator=

Author: Jamie Gault
***********************************/
WordMenu& WordMenu::operator=( WordMenu& menu )
{
	m_identifier =  menu.m_identifier;

	m_option_display =  menu.m_option_display;

	m_options.assign( menu.m_options.begin(),  menu.m_options.end());
	m_cur_option = m_options.begin();

	m_arrows = menu.m_arrows;

	return *this;
}

/***********************************
WordMenu Update

Author: Jamie Gault
***********************************/
void WordMenu::Update()
{
	m_arrows.Update();

	if( m_arrows.IsPressed() == AA_DEC )
	{
		--m_cur_option;

		UpdateDisplay();
	}
	else if( m_arrows.IsPressed() == AA_INC )
	{
		++m_cur_option;

		UpdateDisplay();
	}
}

/***********************************
WordMenu Render

Author: Jamie Gault
***********************************/
void WordMenu::Render()
{
	if (isVisible()) {
		m_arrows.Render();
		m_identifier.Render();
		m_option_display.Render();
	}
}

/***********************************
WordMenu UpdateValueDisplay

Author: Jamie Gault
***********************************/
void WordMenu::UpdateDisplay()
{
	m_option_display.SetString(*m_cur_option);
}

/***********************************
WordMenu SetToValue

Author: Jamie Gault
***********************************/
void WordMenu::SetOption(int i)
{
	int delta = i - m_arrows.m_value;
	if( delta > 0 ) {
		for( int dinc = 0; dinc < delta; ++dinc ) ++m_cur_option;
	}
	else {
		for( int dinc = 0; dinc < -delta; ++dinc ) --m_cur_option;
	}

	m_arrows.SetValue( i );
	UpdateDisplay();
}

/***********************************
PieceOption Constructor

Author: Jamie Gault
***********************************/
PieceOption::PieceOption(  pina* obj, std::string name ):  m_taken(false), m_token_name(name),m_piece(false)
{
	m_piece.setPos( 0.0f, 0.0f, 0.0f );
	m_piece.setPina(obj);
	/*m_piece.scale.x = 2.0f;
	m_piece.scale.y = 2.0f;
	m_piece.scale.z = 2.0f;*/

	m_piece.scale.x = 0.7f;
	m_piece.scale.y = 0.7f;
	m_piece.scale.z = 0.7f;
	m_piece.rot.y = 180.0f;
}

/***********************************
PieceOption Destructor

Author: Jamie Gault
***********************************/
PieceOption::~PieceOption()
{
	//RemoveObject( &m_piece );
}

/***********************************
PieceMenu Constructor

Author: Jamie Gault
***********************************/
PieceMenu::PieceMenu(): m_action( PA_PAUSE )
{
	base_pos[0] = 0.0f;
	base_pos[1] = 3.0f;
	base_pos[2] = 0.0f;

	goal_pos[0] = 0.0f;
	goal_pos[1] = 3.0f;
	goal_pos[2] = 0.0f;

	m_arrows = IncDecArrows( 0, DATA("PIECE_ARROW_POSX"), DATA("PIECE_ARROW_POSY"),
							DATA("PIECE_ARROW_POSX")+ DATA("PIECE_ARROW_GAP"), DATA("PIECE_ARROW_POSY"),
							0, (int)pModel_man->getTokens()->size() - 1 );

	LoadDisplayPieces();
}

/***********************************
PieceMenu Deconstructor

Author: Jamie Gault
***********************************/
PieceMenu::~PieceMenu()
{
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		delete (*poi);
	}
}

/***********************************
PieceMenu Update

Author: Jamie Gault
***********************************/
void PieceMenu::Update(float t)
{
	//spin current piece
	(*m_selected_piece)->m_piece.rot.y += t;

	//change the scale of all pieces
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		//if the piece is unselected
		if( (*poi) == (*m_selected_piece) )
		{
			if( (*poi)->m_piece.scale.x < 4.0f )
			{
				(*poi)->m_piece.scale.x += t/10.0f;
				(*poi)->m_piece.scale.y += t/10.0f;
				(*poi)->m_piece.scale.z += t/10.0f;

				if((*poi)->m_piece.scale.x > 4.0f )
				{
					(*poi)->m_piece.scale.x = 4.0f;
					(*poi)->m_piece.scale.y = 4.0f;
					(*poi)->m_piece.scale.z = 4.0f;
				}
			}
		}
		else
		{
			if( (*poi)->m_piece.scale.x > 1.5f )
			{
				(*poi)->m_piece.scale.x -= t/10.0f;
				(*poi)->m_piece.scale.y -= t/10.0f;
				(*poi)->m_piece.scale.z -= t/10.0f;

				if((*poi)->m_piece.scale.x < 1.5f )
				{
					(*poi)->m_piece.scale.x = 1.5f;
					(*poi)->m_piece.scale.y = 1.5f;
					(*poi)->m_piece.scale.z = 1.5f;
				}
			}
		}
	}

	//if the menu is shifting
	if( m_action != PA_PAUSE )
	{
		//increment positions
		if(  m_action == PA_MOVERIGHT )
		{
			base_pos[0] -= DATA("PIECE_POS_INC")*t;

			if( base_pos[0] < goal_pos[0] )
			{
				//pMusic_man->play("SEL_TOK");
				base_pos[0] = goal_pos[0];
				m_action = PA_PAUSE;
			}
		}
		else if(  m_action == PA_MOVELEFT )
		{
			base_pos[0] += DATA("PIECE_POS_INC")*t;

			if( base_pos[0] > goal_pos[0] )
			{
			//pMusic_man->play("SEL_TOK");
				base_pos[0] = goal_pos[0];
				m_action = PA_PAUSE;
			}
		}

		UpdatePiecePos();
	}
	else
	{
		m_arrows.Update();

		//if the left button was pressed
		if( pInput->PollMouseBut(MLEFT) )
		{
			ARROW_ACTION a = m_arrows.IsPressed();

			if( a == AA_INC )
				MoveRight();
			else if( a == AA_DEC )
				MoveLeft();
		}
	}
}

/***********************************
PieceMenu Render

Author: Jamie Gault
***********************************/
void PieceMenu::Render()
{
	m_arrows.Render();
}

/***********************************
PieceMenu Reset

Author: Jamie Gault
***********************************/
void PieceMenu::Reset()
{
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		(*poi)->m_piece.setVisible(true);
		(*poi)->m_taken = false;
	}

	UpdatePiecePos();
}

/***********************************
PieceMenu Reset

Author: Jamie Gault
***********************************/
void PieceMenu::OpenPiece( pina* model )
{
	float pos_step = DATA("PIECE_POS_STEP");
	//iterate through until you find the model that matches and set it to visible
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		PieceOption * pieceOption = *poi;
		if(pieceOption->m_piece.getPina() == model) {
			pieceOption->m_piece.setVisible(true);
			pieceOption->m_taken = false;

			m_arrows.m_value = 0;
			m_action = PA_MOVERIGHT;
			m_selected_piece = poi;
			break;
		}
		if (pieceOption->m_taken) {
			// skip taken piece - it won't be shown, and we can
			// properly rotate the carusel
			continue;
		}
		if (model != NULL) {
			// rotate the carusel until we find the selected piece
			goal_pos[0] -= pos_step;
		}
	}
	// set goal position to the base one to eliminate the rotation action
	base_pos[0] = goal_pos[0];
	UpdatePiecePos();
}

/***********************************
PieceMenu ChoosePiece

Author: Jamie Gault
***********************************/
PieceOption* PieceMenu::ChoosePiece()
{
	(*m_selected_piece)->m_piece.setVisible(false);
	(*m_selected_piece)->m_taken = true;

	PieceOption* op = (*m_selected_piece);

	FindUnselected(); //get a new piece that hasn't been selected

	UpdatePiecePos();

	goal_pos[0] = 0.0f;
	m_arrows.m_value = 0;
	m_action = PA_MOVERIGHT;

	return op;
}

/***********************************
PieceMenu SetVisible

Author: Jamie Gault
***********************************/
void PieceMenu::SetVisible( bool v, bool all)
{
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		if( !(*poi)->m_taken || all )
		(*poi)->m_piece.setVisible( v );
	}
}

/***********************************
PieceMenu MoveLeft

Author: Jamie Gault
***********************************/
void PieceMenu::MoveLeft()
{
	bool atend = false;
	std::list<PieceOption*>::iterator poi =  m_selected_piece ;

	if( poi == m_pie_opt_list.begin()  )
		return;

	do
	{ --poi; }
	while( poi != m_pie_opt_list.begin() && (*poi)->m_taken );

	//the beginning of the list was found
	if( poi == m_pie_opt_list.begin() && (*m_pie_opt_list.begin())->m_taken )
		return;

	vec3 v = vec3( 0.5f, 0.5f, 0.5f );
	vec3 u = vec3( 1.0f, 1.0f, 1.0f );

	(*m_selected_piece)->m_piece.setColor(v);

	m_selected_piece = poi;

	(*m_selected_piece)->m_piece.setColor(u);

	goal_pos[0] += DATA("PIECE_POS_STEP");

	m_action = PA_MOVELEFT;
}

/***********************************
PieceMenu MoveRight

Author: Jamie Gault
***********************************/
void PieceMenu::MoveRight()
{
	bool atend = false;
	std::list<PieceOption*>::iterator poi =  m_selected_piece ;

	do
	{ ++poi; }
	while( poi != m_pie_opt_list.end() && (*poi)->m_taken );

	//the end was reached without finding a free piece
	if( poi == m_pie_opt_list.end() )
		return;

	vec3 v = vec3( 0.5f, 0.5f, 0.5f );
	vec3 u = vec3( 1.0f, 1.0f, 1.0f );

	(*m_selected_piece)->m_piece.setColor(v);

	m_selected_piece = poi;

	(*m_selected_piece)->m_piece.setColor(u);

	goal_pos[0] -= DATA("PIECE_POS_STEP");

	m_action = PA_MOVERIGHT;
}

/***********************************
PieceMenu LoadDisplayPieces

Author: Jamie Gault
***********************************/
void PieceMenu::LoadDisplayPieces()
{
	PieceOption* option;

	for(std::map<std::string, Model*>::iterator ti = pModel_man->getTokens()->begin(); ti != pModel_man->getTokens()->end(); ++ti)
	{
		(*ti).second->mesh->SetTextureID( pTexture_man->GetTexture((*ti).second->texture) );
		option = new PieceOption( (*ti).second->mesh, (*ti).first);
		m_pie_opt_list.push_back( option );
	}

	m_selected_piece = m_pie_opt_list.begin();

	//set all the pieces into position
	UpdatePiecePos();
}

/***********************************
PieceMenu UpdatePiecePos

Author: Jamie Gault
***********************************/
void PieceMenu::UpdatePiecePos()
{
	float deltax = 0.0f;
	float space_x = DATA("PIECE_SPACE_X");
	//loop through setting the positoins
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		if( (*poi)->m_taken ) {
			continue;
		}

		//position the pieces about a wheel with an open slot at the end (pModel_man->getTokens()->size() + 1 )
		float angle = (base_pos[0] + deltax) / (space_x * (pModel_man->getTokens()->size() + 1)) * 2.0f * 3.14159f;
		float x = 40.0f * sin(angle);
		float z = 40.0f * cos(angle);
		(*poi)->m_piece.setPos( x, base_pos[1], z);

		deltax += space_x;
	}
}

/***********************************
PieceMenu FindUnselected

Author: Jamie Gault
***********************************/
void PieceMenu::FindUnselected()
{
	for( std::list<PieceOption*>::iterator poi =  m_pie_opt_list.begin() ;
		poi != m_pie_opt_list.end(); ++poi )
	{
		//if the piece is unselected
		if( !(*poi)->m_taken )
		{
			m_selected_piece = poi;
			return;
		}
	}
}

/***********************************
MentorOption Constructor

Author: Jamie Gault
***********************************/
MentorOption::MentorOption( Box_2D box, Mentor* men_id ): m_mentor( men_id ), m_box(box), m_selected(false),
															playingPickMe(false), playingThanks(false)
{}

void MentorOption::pickMe()
{
	playingThanks = false;
	if(pMusic_man->isPlaying(m_mentor->thanks))
		pMusic_man->stop(m_mentor->thanks);

	if(!playingPickMe)
		pMusic_man->play(m_mentor->pickMe);
	playingPickMe = true;
}

void MentorOption::thanks()
{
	playingPickMe = false;
	if(pMusic_man->isPlaying(m_mentor->pickMe))
		pMusic_man->stop(m_mentor->pickMe);

	if(!playingThanks)
		pMusic_man->play(m_mentor->thanks);
	playingThanks = true;
}

/***********************************
MentorOption GetID

Author: Jamie Gault
***********************************/
MENTOR_ID MentorOption::GetID()
{
	return m_mentor->GetMentorID();
}

/***********************************
MentorOption Render

Author: Jamie Gault
***********************************/
void MentorOption::Render()
{
	int pic = ( m_selected ) ? m_mentor->GetEmoPic(HAPPY) : m_mentor->GetEmoPic(NORMAL);

	RenderCoordScreenImage( pic,
							m_box.GetPosX(), m_box.GetPosY(), m_box.GetPosX() + m_box.GetWidth(),
							m_box.GetPosY() + m_box.GetHeight(), 1.0f);

	if (m_box.IsMouseOver() || m_selected) {
		m_box.Render();
	}
}

/***********************************
MentorMenu Constructor

Author: Jamie Gault
***********************************/
MentorMenu::MentorMenu()
	: m_selected_mentor(NULL)
{
	for(int i = 0; i < 10; ++i) {
		float x = (150.0f) + 150.0f * (i % 5);
		float y = ( 180.0f ) + 210.0f * (i / 5);
		MentorOption* pmenop = new MentorOption(
			Box_2D(BT_LINES, x, y, DATA("OPT_MENTOR_WIDTH"), DATA("OPT_MENTOR_HEIGHT")),
			pMentor_man->GetMentor((MENTOR_ID)i)
			);
		m_men_opt_vec.push_back(pmenop);
	}

	m_selected_mentor = *m_men_opt_vec.begin();
}

/***********************************
MentorMenu Destructor

Author: Jamie Gault
***********************************/
MentorMenu::~MentorMenu()
{
	for(std::vector<MentorOption*>::iterator i = m_men_opt_vec.begin();
		i != m_men_opt_vec.end(); ++i)
	{
		delete(*i);
	}
}

/***********************************
MentorMenu PollOptions

Author: Jamie Gault
***********************************/
MentorOption* MentorMenu::PollOptions()
{
	//Iterate through all the mentor options
	for (size_t i = 0; i < m_men_opt_vec.size(); ++i) {
		MentorOption * menopt = m_men_opt_vec[i];
		//if the mouse is over an option
		if(menopt->m_box.IsMouseOver() ) {
			//if left mouse was pressed
			if( pInput->PollMouseBut(MLEFT) ) {
				//Play thanks audio for selecting this mentor
				menopt->thanks();

				//Deselect the selected mentor
				if( m_selected_mentor ) {
					m_selected_mentor->m_selected = false;
				}

				//And select this one
				m_selected_mentor = menopt;
				m_selected_mentor->m_selected = true;
				return m_selected_mentor;
			}
			else if(!menopt->playingThanks) {
				menopt->pickMe();
			}
		} else {
			//Stop the pick me audio if the user mouses out (prevents multiple mentors talking)
			if(menopt->playingPickMe && pMusic_man->isPlaying(menopt->m_mentor->pickMe)) {
				pMusic_man->stop(menopt->m_mentor->pickMe);
			}
			menopt->playingPickMe = false;
			menopt->playingThanks = false;
		}
	}
	return m_selected_mentor; //returns if there was one already selected or not
}

/***********************************
MentorMenu Render

Author: Jamie Gault
***********************************/
void MentorMenu::Render()
{
	for (size_t i = 0; i < m_men_opt_vec.size(); ++i) {
		m_men_opt_vec[i]->Render();
	}
}

void MentorMenu::SetSelectedMentor(int id)
{
	for (int i = 0; i < m_men_opt_vec.size(); ++i) {
		if (i == id) {
			m_selected_mentor = m_men_opt_vec[i];
			m_selected_mentor->m_selected = true;
		} else {
			m_men_opt_vec[i]->m_selected = false;
		}
	}
	if (!m_selected_mentor) {
		m_selected_mentor = m_men_opt_vec[0];
	}
}

MentorOption* MentorMenu::GetSelectedMentor()
{
	return m_selected_mentor;
}

/***********************************
ConfigConfirm Constructor

Author: Jamie Gault
***********************************/
ConfigConfirm::ConfigConfirm(PlayerSetup::list_t & setups)
{
	float inc_y = 0.0f;

	for(PlayerSetup::list_t::iterator psi = setups.begin()
		; psi != setups.end()
		; ++psi, inc_y += 100.0f )
	{
		TextBox * name = new TextBox( pText_man, psi->m_name, 0.0f, 125.0f + inc_y, 500.0f );
		name->SetPosX(500.0f - name->GetBoxWidth() - 20);
		name->SetBackground(pTexture_man->GetTexture("TID_TEXTBG"));
		name_vec.push_back(name);

		//load in a new packground
		token_pics.push_back( new Box_2D( pTexture_man->GetTexture( std::string("icon_") + psi->m_token_name ),
										  500.0f, 100.0f+ inc_y, 100.0f, 100.0f ) );

		mentor_pics.push_back( new Box_2D( pMentor_man->GetMentor(psi->m_mentor )->GetEmoPic(HAPPY),
										  600.0f, 100.0f+ inc_y, 100.0f, 100.0f ) );

		std::string music;
		switch(psi->m_soundset) {
		default:
		case 0:
			music = std::string("MsI_Dis");
			break;
		case 1:
			music = std::string("MsI_Reg");
			break;
		case 2:
			music = std::string("MsI_Rock");
			break;
		case 3:
			music = std::string("MsI_Tec");
			break;
		case 4:
			music = std::string("MsI_Folk");
			break;
		}

		token_pics.push_back( new Box_2D( pTexture_man->GetTexture( music ),
										  700.0f, 100.0f+ inc_y, 100.0f, 100.0f ) );

		if( psi->m_gender == GEN_MALE)
			gender_vec.push_back( new TextBox( pText_man, std::string("Boy"),  810.0f, 120.0f + inc_y, 500.0f ));
		else
			gender_vec.push_back( new TextBox( pText_man, std::string("Girl"),  810.0f, 120.0f + inc_y, 500.0f ));
	}
}

/***********************************
ConfigConfirm Deconstructor

Author: Jamie Gault
***********************************/
ConfigConfirm::~ConfigConfirm()
{
	for( std::vector<TextBox*>::iterator name = name_vec.begin(); name != name_vec.end(); ++name )
		delete (*name);

	for( std::vector<Box_2D*>::iterator token = token_pics.begin(); token != token_pics.end(); ++token )
		delete (*token);

	for( std::vector<Box_2D*>::iterator mentor = mentor_pics.begin(); mentor != mentor_pics.end(); ++mentor )
		delete (*mentor);

	for( std::vector<Box_2D*>::iterator sound = sound_pics.begin(); sound != sound_pics.end(); ++sound )
		delete (*sound);

	for( std::vector<TextBox*>::iterator gender = gender_vec.begin(); gender != gender_vec.end(); ++gender )
		delete (*gender);
}

/***********************************
ConfigConfirm Render

Author: Jamie Gault
***********************************/
void ConfigConfirm::Render()
{
	for( std::vector<TextBox*>::iterator name = name_vec.begin(); name != name_vec.end(); ++name )
		(*name)->Render();

	for( std::vector<Box_2D*>::iterator token = token_pics.begin(); token != token_pics.end(); ++token )
		(*token)->Render();

	for( std::vector<Box_2D*>::iterator mentor = mentor_pics.begin(); mentor != mentor_pics.end(); ++mentor )
		(*mentor)->Render();

	for( std::vector<Box_2D*>::iterator sound = sound_pics.begin(); sound != sound_pics.end(); ++sound )
		(*sound)->Render();

	for( std::vector<TextBox*>::iterator gender = gender_vec.begin(); gender != gender_vec.end(); ++gender )
		(*gender)->Render();
}

/***********************************
PictureMenu Constructor

Author: Jamie Gault
***********************************/
PictureMenu::PictureMenu(std::vector<unsigned int> &pic_ids, std::vector<int> &menu_ids, float x, float y, int max_row)
{
	x *= SCREEN_X_RATIO;
	y *= SCREEN_Y_RATIO;

	int size = (int)pic_ids.size();
	float xstep = x;

	//create all the buttons
	for( int i = 0; i < size; ++i, xstep += 150.0f )
	{
		PictureOption* p = new PictureOption( Box_2D(pic_ids[i], xstep, y, 100.0f, 100.0f ), menu_ids[i] );
		m_pic_opt_vec.push_back(p);
	}
}
PictureMenu::PictureMenu(std::vector<BtnTextureSet> &pic_ids, std::vector<int> &menu_ids, float x, float y, int max_row)
{
	x *= SCREEN_X_RATIO;
	y *= SCREEN_Y_RATIO;

	int size = (int)pic_ids.size();
	float xstep = x;

	//create all the buttons
	for( int i = 0; i < size; ++i, xstep += 150.0f )
	{
		PictureOption* p = new PictureOption( Box_2D(pic_ids[i].m_idle, xstep, y, 100.0f, 100.0f, pic_ids[i].m_mouse_over ), menu_ids[i] );
		m_pic_opt_vec.push_back(p);
	}
}

/***********************************
PictureMenu Destructor

Author: Jamie Gault
***********************************/
PictureMenu::~PictureMenu()
{
	for(std::vector<PictureOption*>::iterator i = m_pic_opt_vec.begin();
		i != m_pic_opt_vec.end(); ++i)
	{
		delete(*i);
	}
}

/***********************************
PictureMenu PollOptions

Author: Jamie Gault
***********************************/
int PictureMenu::PollOptions()
{
	for (size_t i = 0; i < m_pic_opt_vec.size(); ++i) {
		PictureOption * po = m_pic_opt_vec[i];
		//if the mouse is over an option
		if(po->m_box.IsMouseOver() )
		{
			po->m_box.UseAltBG();

			//if left mouse was pressed
			if( pInput->PollMouseBut(MLEFT) )
			{
				m_selected_option = po->value;
				return m_selected_option;
			}
		}
		else
			po->m_box.UseBG();
	}

	return -1;
}

/***********************************
PictureMenu Render

Author: Jamie Gault
***********************************/
void PictureMenu::Render()
{
	for (size_t i = 0; i < m_pic_opt_vec.size(); ++i) {
		m_pic_opt_vec[i]->m_box.Render();
	}
}

/***********************************
PictureMenu Reset

Author: Jamie Gault
***********************************/
void PictureMenu::Reset()
{
	if (!m_pic_opt_vec.empty()) {
		m_selected_option = m_pic_opt_vec[0]->value;
	}
}

Text_Manager* txt;

/***********************************
SetupMenu Constructor

Author: Jamie Gault
***********************************/
SetupMenu::SetupMenu(Text_Manager* tm, GLuint t_id )
	: m_player_cnt(0), m_state(SMS_GAME_SETUP), m_confirm(NULL)
{
	txt = tm;

	m_playercounter = NumberMenu( tm, "Number of Players:", 2, 2, 5, 1,  DATA("PLYCNT_MENU_POSX"), DATA("PLYCNT_MENU_POSY"));

	m_roundcount = NumberMenu( tm, "Rounds Limit:", DATA("DEFAULT_ROUND_LIMIT"),
								DATA("MIN_ROUND_LIMIT"), DATA("MAX_ROUND_LIMIT"),  DATA("ROUND_INC"),
								DATA("RNDCNT_MENU_POSX"), DATA("RNDCNT_MENU_POSY") );

	m_scorecount = NumberMenu( tm, "Goal Score:", DATA("DEFAULT_SCORE_LIMIT"),
								DATA("MIN_SCORE_LIMIT"), DATA("MAX_SCORE_LIMIT"), 5,
								DATA("SCRCNT_MENU_POSX"), DATA("SCRCNT_MENU_POSY"));
	m_scorecount.setVisible(false);

	//load in goal menu
	std::list<std::string> game_mode_op;
	game_mode_op.push_back( std::string("when selected number of rounds are completed."));
	game_mode_op.push_back( std::string("when one player reaches the selected high score."));
	game_mode_op.push_back( std::string("when one player reaches the selected high score or highest score after a set number of turns."));
	m_game_mode = WordMenu( tm, "Game Ends:", game_mode_op,
							DATA("GOAL_MENU_POSX"), DATA("GOAL_MENU_POSY") );

	//load the privacy on off selection
	// per Jen/Hy request privacy settings were removed. I'll just comment the code though :)
	//std::list<std::string> onoff_op;
	//onoff_op.push_back( std::string("Disabled"));
	//onoff_op.push_back( std::string("Enabled"));
	//m_priv_on = WordMenu( tm, "Privacy:", onoff_op,
	//						DATA("PRIVON_MENU_POSX"), DATA("PRIVON_MENU_POSY") );

	////determine the limitations on how many questions a user can skip
	//std::list<std::string> privacy_limit_op;
	//privacy_limit_op.push_back("Unlimited");
	//for (int i = (int)DATA("MIN_PRIV_LIMIT"); i <= DATA("MAX_PRIV_LIMIT"); ++i) {
	//	std::ostringstream oss;
	//	oss << i;
	//	privacy_limit_op.push_back(oss.str());
	//}
	//m_priv_lim_on = WordMenu( tm, "Requests for privacy:", privacy_limit_op,
	//							DATA("PRIVLIM_MENU_POSX"), DATA("PRIVLIM_MENU_POSY"));
	//m_priv_lim_on.setVisible(false);

	//setup the gender menu
	std::list<std::string> gender_op;
	gender_op.push_back( std::string("Boy"));
	gender_op.push_back( std::string("Girl"));
	m_gendersetter = WordMenu( tm, "Player Gender:", gender_op,
								DATA("GENSET_MENU_POSX"), DATA("GENSET_MENU_POSY") );

	m_playername = EditMenu( tm, "Player Name:  ", DATA("PLNAME_MENU_POSX"), DATA("PLNAME_MENU_POSY"), DATA("PLNAME_MAX_LENGTH"));

	//load in the sound menu
	if(pMusic_man->getNumSets() > 0)
	{
		std::vector<BtnTextureSet> imagelist;// list of images
		std::vector<int> menu_ids; //ids for the menu

		for( int i = 0; i < pMusic_man->getNumSets(); ++i )
		{
			std::string caption;
			std::string alt;
			switch(i)
			{
			default:
			case 0:
				caption = std::string("MsI_Dis");
				alt = std::string("MsI_Dis_A");
				break;
			case 1:
				caption = std::string("MsI_Reg");
				alt = std::string("MsI_Reg_A");
				break;
			case 2:
				caption = std::string("MsI_Rock");
				alt = std::string("MsI_Rock_A");
				break;
			case 3:
				caption = std::string("MsI_Tec");
				alt = std::string("MsI_Tec_A");
				break;
			case 4:
				caption = std::string("MsI_Folk");
				alt = std::string("MsI_Folk_A");
				break;
			}
			BtnTextureSet set = {pTexture_man->GetTexture( caption), pTexture_man->GetTexture( alt), 0};

			imagelist.push_back( set );
			menu_ids.push_back(i);
		}

		m_soundsetter = new PictureMenu(imagelist, menu_ids, DATA("SNDSET_MENU_POSX"), DATA("SNDSET_MENU_POSY"), 5);
	}
	else
		m_soundsetter = NULL;

	//init buttons for moving back and forth between menus
	m_Next_but = Answer( TextBox(tm,  DATA("NEXT_BUT_POS_X"), DATA("NEXT_BUT_POS_Y"), 128.0f*3.0f/4.0f, 64.0f*3.0f/4.0f ));
	m_Back_but = Answer( TextBox(tm,  DATA("BACK_BUT_POS_X"), DATA("NEXT_BUT_POS_Y"), 128.0f*3.0f/4.0f, 64.0f*3.0f/4.0f ) );

	m_Next_but.SetBGs(pTexture_man->GetTexture("NEXT_BUT_IDLE"),
					pTexture_man->GetTexture("NEXT_BUT_MO"),
					pTexture_man->GetTexture("NEXT_BUT_CLICK"));

	m_Back_but.SetBGs(pTexture_man->GetTexture("BACK_BUT_IDLE"),
					pTexture_man->GetTexture("BACK_BUT_MO"),
					pTexture_man->GetTexture("BACK_BUT_CLICK"));

	m_menu_title = TextBox(txt
		, std::string("")
		, DATA("MENU_TITLE_POSX") //20.0f*SCREEN_X_RATIO
		, DATA("MENU_TITLE_POSY") //20.0f*SCREEN_Y_RATIO,
		, 800.0f
		, 15.0f*SCREEN_X_RATIO
		, 40*SCREEN_X_RATIO
		, 40*SCREEN_Y_RATIO );

	m_menu_footer = TextBox(txt
		, std::string("Let's roll to see who goes first.")
		, 300.0f
		, 700.0f
		, 700.0f
		, 10.0f*SCREEN_X_RATIO
		, 30*SCREEN_X_RATIO
		, 30*SCREEN_Y_RATIO);
}

/***********************************
SetupMenu Deconstructor

Author: Jamie Gault
***********************************/
SetupMenu::~SetupMenu()
{
	ClearPlayerSetups();

	if( m_soundsetter)
		delete m_soundsetter;

	if( m_confirm )
		delete m_confirm;
}

/***********************************
SetupMenu Update

Author: Jamie Gault
***********************************/
void SetupMenu::Update(float t)
{
	m_Next_but.Update(t);
	m_Back_but.Update(t);

	switch( m_state )
	{
	case SMS_GAME_SETUP:
		m_playercounter.Update();
		m_game_mode.Update();

		// privacy request is disabled
		//m_priv_on.Update();
		//if( m_priv_on.GetOption() == 1 )
		//{
		//	m_priv_lim_on.Update();
		//	m_priv_lim_on.setVisible(true);
		//} else {
		//	m_priv_lim_on.setVisible(false);
		//}

		//determine which rules' options are in effect
		switch (m_game_mode.GetOption()) {
		case 0:
			m_roundcount.Update();
			m_roundcount.setVisible(true);
			m_scorecount.setVisible(false);
			break;
		case 1:
			m_scorecount.Update();
			m_scorecount.setVisible(true);
			m_roundcount.setVisible(false);
			break;
		case 2:
			m_roundcount.Update();
			m_roundcount.setVisible(true);
			m_scorecount.Update();
			m_scorecount.setVisible(true);
			break;
		}
		break;

	case SMS_PLAYER_NAME:
		m_playername.Update();
		break;

	case SMS_PLAYER_INFO:
		m_gendersetter.Update();

		{
			int oldselection = m_soundsetter->GetSelectedOpt();
			int result = m_soundsetter->PollOptions();

			//if the default sound was changed in the menu
			if( result != -1)
			{
				pMusic_man->fadeOut( pMusic_man->getSndSet( oldselection ) + "-happy");
				pMusic_man->fadeIn(pMusic_man->getSndSet((result)) + "-happy");
				pMusic_man->play(pMusic_man->getSndSet((result)) + "-happy");
			}
		}

		break;
	case SMS_PLAYER_TOKEN:
		m_piece_menu.Update(t);
		break;

	case SMS_MENTOR:
		m_ment_menu.PollOptions();
		break;

	case SMS_CONFIRM:
	case SMS_START:
	default:
		break;
	}

	//check for menu switching
	if(pInput->PollMouseBut(MLEFT))
	{
		if( m_Next_but.IsMouseOver() )
			MoveNext();
		else if(  m_Back_but.IsMouseOver() )
			MoveBack();
	}
}

/***********************************
SetupMenu StartGame

Author: Jamie Gault
***********************************/
bool SetupMenu::StartGame()
{
	return m_state == SMS_START;
}

/***********************************
SetupMenu BackToMain

Author: Jamie Gault
***********************************/
bool SetupMenu::BackToMain()
{
	return m_state == SMS_MAINBACK;
}

/***********************************
SetupMenu Render

Author: Jamie Gault
***********************************/
void SetupMenu::Render()
{
	glColor3f(1.0f, 1.0f, 1.0f );

	RenderCoordScreenImage( pTexture_man->GetTexture(std::string("MENU_BACKGROUND")),
						50.0f,
						50.0f,
						(DATA("SCREEN_WIDTH") -50.0f),
						(DATA("SCREEN_HEIGHT") -50.0f));

	m_menu_title.Render();

	switch( m_state )
	{
	case SMS_GAME_SETUP:
		m_playercounter.Render();
		m_game_mode.Render();

		// privacy request is disabled
		//m_priv_on.Render();
		//m_priv_lim_on.Render();

		m_roundcount.Render();
		m_scorecount.Render();
		break;

	case SMS_PLAYER_NAME:
		m_playername.Render();
		break;

	case SMS_PLAYER_INFO:
		m_soundsetter->Render();
		m_gendersetter.Render();
		break;

	case SMS_PLAYER_TOKEN:
		m_piece_menu.Render();
		break;

	case SMS_MENTOR:
		m_ment_menu.Render();
		break;

	case SMS_CONFIRM:
		m_confirm->Render();
		m_menu_footer.Render();
		break;

	case SMS_START:
	default:
		break;
	}

	////render what player is being active
	//if(m_state == SMS_MENTOR || m_state == SMS_PLAYER_TOKEN || m_state == SMS_PLAYER_INFO) {
	//	glBindTexture( GL_TEXTURE_2D, 0 );
	//	TextBox box( txt, m_cur_setup->m_name, 400.0f*SCREEN_X_RATIO, 100.0f*SCREEN_Y_RATIO,
	//				500.0f, 10.0f*SCREEN_X_RATIO, 30*SCREEN_X_RATIO, 30*SCREEN_Y_RATIO  );
	//	box.Render();
	//}

	//render buttons
	m_Next_but.Render();
	m_Back_but.Render();
}

void SetupMenu::ActivateGameSetupView()
{
	m_menu_title.SetString("Game Setup", true);
}

void SetupMenu::DeactivateGameSetupView()
{
	//get the number of players selected
	m_player_cnt = m_playercounter.GetValue();

	// intialize enough players; store the previous number
	// of players to make sure existing information is not overwritten
	size_t prev_sz = m_psetup_list.size();
	m_psetup_list.resize(m_player_cnt);
	for (size_t i = prev_sz; i < m_player_cnt; ++i ) {
		m_psetup_list[i] = PlayerSetup( i+1, NULL, M_NONE, -1, GEN_MALE );
	}
	m_cur_setup = m_psetup_list.begin();
}

void SetupMenu::ActivatePlayerNameView()
{
	m_state = SMS_PLAYER_NAME;
	m_playername.SetText(m_cur_setup->m_name);
	std::ostringstream oss;
	oss << "Player " << m_cur_setup->m_ID << ", Enter Your Name";
	m_menu_title.SetString(oss.str(), true);
}

void SetupMenu::DeactivatePlayerNameView()
{
	m_cur_setup->m_name = m_playername.GetText();
	if (m_cur_setup->m_name.empty()) {
		m_cur_setup->m_name = "Player ";
		m_cur_setup->m_name += '0' + m_cur_setup->m_ID;
	}
}

void SetupMenu::ActivatePlayerInfoView()
{
	m_state = SMS_PLAYER_INFO;
	if (m_cur_setup->m_soundset != -1) {
		m_soundsetter->SetSelectedOpt(m_cur_setup->m_soundset);
	} else {
		m_soundsetter->Reset();
	}
	m_gendersetter.SetOption(m_cur_setup->m_gender);
	m_menu_title.SetString(m_cur_setup->m_name + ", Select Your Gender and Music", true);
}

void SetupMenu::DeactivatePlayerInfoView()
{
	m_cur_setup->m_soundset = m_soundsetter->GetSelectedOpt();
	m_cur_setup->m_gender = (GENDER)m_gendersetter.GetOption();
	pMusic_man->fadeOut(pMusic_man->getSndSet(m_cur_setup->m_soundset) + "-happy");
}

void SetupMenu::ActivatePlayerTokenView()
{
	m_state = SMS_PLAYER_TOKEN;
	m_piece_menu.OpenPiece(m_cur_setup->m_boardpiece);
	m_piece_menu.Update(1.0f);
	m_menu_title.SetString(m_cur_setup->m_name + ", Select Your Token", true);
}

void SetupMenu::DeactivatePlayerTokenView()
{
	//trade over the selection values for the pieces and name
	PieceOption* piece = m_piece_menu.ChoosePiece();
	m_cur_setup->m_boardpiece = piece->m_piece.getPina();
	m_cur_setup->m_token_name = piece->m_token_name;
	m_cur_setup->m_soundset = m_soundsetter->GetSelectedOpt();

	//change the state to sound and mentor menu state
	m_soundsetter->Reset(); //put the sound menu back at the default value
}

void SetupMenu::ActivateMentorView()
{
	m_state = SMS_MENTOR;
	m_ment_menu.SetSelectedMentor(m_cur_setup->m_mentor);
	m_menu_title.SetString(m_cur_setup->m_name + ", Select Your Mentor", true);
}

void SetupMenu::DeactivateMentorView()
{
	if (m_ment_menu.GetSelectedMentor()) {
		m_cur_setup->m_mentor = m_ment_menu.GetSelectedMentor()->GetID();
	}
}

void SetupMenu::ActivateConfirmView()
{
	m_state = SMS_CONFIRM;
	m_confirm = new ConfigConfirm( m_psetup_list );
	m_menu_title.SetString("Ready to begin?", true);
	m_menu_footer.SetString("Let's roll to see who goes first", true);
}

void SetupMenu::DeactivateConfirmView()
{
	if (m_confirm) {
		delete m_confirm;
		m_confirm = NULL;
	}
}

/***********************************
SetupMenu Next

Author: Jamie Gault
***********************************/
void SetupMenu::MoveNext()
{
	pMusic_man->play("BTN_C");
	switch( m_state ) {
	case SMS_GAME_SETUP:
		DeactivateGameSetupView(); //intialize the number of players needed
		ActivatePlayerNameView();
		break;

	case SMS_PLAYER_NAME:
		DeactivatePlayerNameView();
		ActivatePlayerInfoView();
		break;

	case SMS_PLAYER_INFO:
		DeactivatePlayerInfoView();
		ActivatePlayerTokenView();
		break;

	case SMS_PLAYER_TOKEN:
		DeactivatePlayerTokenView();
		ActivateMentorView();
		break;

	case SMS_MENTOR:
		DeactivateMentorView();
		++m_cur_setup;
		if (m_cur_setup == m_psetup_list.end()) {
			ActivateConfirmView();
		} else {
			ActivatePlayerNameView();
		}
		break;

	case SMS_CONFIRM:
		DeactivateConfirmView();
		//m_menu_title.SetString("Ready to begin?", true);
		m_state = SMS_START;
		break;

	case SMS_START:
	default:
		break;
	}
}

/***********************************
SetupMenu Back

Author: Jamie Gault
***********************************/
void SetupMenu::MoveBack()
{
	pMusic_man->play("BTN_C");

	switch( m_state )
	{
	case SMS_GAME_SETUP:
		m_state = SMS_MAINBACK;
		break;

	case SMS_PLAYER_NAME:
		DeactivatePlayerNameView();
		if (m_cur_setup == m_psetup_list.begin()) {
			m_state = SMS_GAME_SETUP;
			m_menu_title.SetString("Game Setup", true);
		} else {
			--m_cur_setup;
			ActivateMentorView();
		}
		break;

	case SMS_PLAYER_INFO:
		DeactivatePlayerInfoView();
		ActivatePlayerNameView();
		break;

	case SMS_PLAYER_TOKEN:
		DeactivatePlayerTokenView();
		ActivatePlayerInfoView();
		break;

	case SMS_MENTOR:
		DeactivateMentorView();
		ActivatePlayerTokenView();
		break;

	case SMS_CONFIRM:
		DeactivateConfirmView();
		--m_cur_setup;
		ActivateMentorView();
		break;

	case SMS_START:
	default:
		break;
	}
}

/***********************************
SetupMenu ClearPlayerSetups

Author: Jamie Gault
***********************************/
void SetupMenu::ClearPlayerSetups()
{
	m_psetup_list.clear();
}