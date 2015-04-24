/*
	Input.cpp
	Created by: Jamie Gault
	Created on 4/26/07
*/

#include "Input.h"
#include <iostream>

Input *pInput;

/***********************************
Input Constructor
	
brief: 
Author: Jamie Gault
***********************************/
Input::Input()
	: m_xRatio(1)
	, m_yRatio(1)
{
	m_mouse.x = 0;
	m_mouse.y = 0;
	ClearKeyboardData();
	ClearMouseData();
}

/***********************************
Input PollKey
	
brief: returns next key from the pressed_key stack
Author: Vigen Issahhanjan
***********************************/
char Input::NextKey()
{
	if (m_keyboard.pressed_keys.empty()) {
		return 0;
	}
	char c = m_keyboard.pressed_keys.top();
	m_keyboard.pressed_keys.pop();
	return c;
}

/***********************************
Input PollKey
	
brief: returns true if the key was pressed
Author: Jamie Gault
***********************************/
char Input::PollKey( char c )
{
	//if the key is within a certain range
	if( c >= 'a' && c <= 'z' )
		return m_keyboard.Trig[c-'a'];
	else if ( c == ESCAPE )
		return m_keyboard.t_escape;
	else if ( c == ENTER )
		return m_keyboard.t_enter;
	else if ( c == ' ' )
		return m_keyboard.t_space;

	return 0;
}


/***********************************
Input PollMouseBut
	
Author: Jamie Gault
***********************************/
char Input::PollMouseBut( MouseDown but )
{
	return m_mouse.Trig[but];
}


/***********************************
Input GetMousePosX
	
Author: Jamie Gault
***********************************/
float Input::GetMousePosX()
{
	float x = m_mouse.x / m_xRatio;
	return x;
}


/***********************************
Input GetMousePosY
	
Author: Jamie Gault
***********************************/
float Input::GetMousePosY()
{
	float y = m_mouse.y / m_yRatio;
	return y;
}



/***********************************
Input SetMousePosX
	

Author: Jamie Gault
***********************************/
void Input::SetMousePosX(float x)
{
	m_mouse.x = x;
}


/***********************************
Input SetMousePosY
	
Author: Jamie Gault
***********************************/
void Input::SetMousePosY(float y)
{
	m_mouse.y = y;
}



/***********************************
Input SetMouseBut
	
Author: Jamie Gault
***********************************/
void Input::SetMouseButTrig( int i)
{
	if( i == (int) MRIGHT || i == (int) MLEFT )
		m_mouse.Trig[i] = 1;
}


/***********************************
Input SetMouseBut
	
Author: Jamie Gault
***********************************/
void Input::SetMouseButHeld( int i)
{
	if( i == (int) MRIGHT || i == (int) MLEFT )
		m_mouse.Held[i] = 1;
}



/***********************************
Input ClearMouseButTrig
	
Author: Jamie Gault
***********************************/
void Input::ClearMouseButTrig( int i)
{
	if( i == (int) MRIGHT || i == (int) MLEFT )
		m_mouse.Trig[i] = 0;
}


/***********************************
Input ClearMouseButHeld
	
Author: Jamie Gault
***********************************/
void Input::ClearMouseButHeld( int i)
{
	if( i == (int) MRIGHT || i == (int) MLEFT )
		m_mouse.Held[i] = 0;
}



/***********************************
Input SetInput
	
Author: Jamie Gault
***********************************/
void Input::SetKeyboard( char c)
{
	if (c < 127) {
		m_keyboard.pressed_keys.push(c);
	}
	if( (c >= 'a' && c <= 'z' ) )
	{
		m_keyboard.Trig[c-'a'] = 1;
	}
	else if( c == ESCAPE )
	{
		m_keyboard.t_escape = 1;
	}
	else if( c == ENTER )
	{
		m_keyboard.t_enter = 1;

	}
	else if( c == ' ' )
	{
		m_keyboard.t_space = 1;

	}


}


/***********************************
Input ClearKeyboardData
	
Author: Jamie Gault
***********************************/
void Input::ClearKeyboardData()
{
	for( int eachkey = 0; eachkey <= 'z'-'a' ; ++eachkey)
	{
		m_keyboard.Trig[eachkey] = 0;
		m_keyboard.Held[eachkey] = 0;
	}

	m_keyboard.t_enter = 0;
	m_keyboard.t_escape = 0;
	m_keyboard.t_space = 0;
	m_keyboard.pressed_keys.swap(std::stack<char>());
}


/***********************************
Input ClearMouseData
	
Author: Jamie Gault
***********************************/
void Input::ClearMouseData()
{
	m_mouse.Held[0] = 0;
	m_mouse.Held[1] = 0;
	m_mouse.Trig[0] = 0;
	m_mouse.Trig[1] = 0;
}

void Input::SetViewportRatio(float xRatio, float yRatio)
{
	m_xRatio = xRatio;
	m_yRatio = yRatio;
}
