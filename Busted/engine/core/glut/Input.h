/*
	Input.h
	Created by: Jamie Gault
	Created on 4/26/07
*/

#pragma once

#define MAX_KEY_RANGE  'z'-'a' + 1
//all keys a through z

const char ESCAPE = 27;
const char ENTER = 10;

#include <stack>

//struct for managing mouse data
struct MOUSE
{
	float x;
	float y;
	char Trig[2];
	char Held[2];
};

enum MouseDown{MLEFT, MRIGHT};

//struct for managing keyboard data
struct KEYBOARD
{
	char Trig[MAX_KEY_RANGE]; //letter keys
	char Held[MAX_KEY_RANGE];
	char t_enter; // flag for enter being triggered
	char t_escape; //flag for espace being triggered
	char t_space; //flag from pressing space bar
	std::stack<char> pressed_keys;	// stack of pressed keys
};

//input class responsible for managing input
class Input
{
public:
	Input();

	//getters
	char NextKey();
	char PollKey( char c );
	char PollMouseBut( MouseDown but );
	float GetMousePosX();
	float GetMousePosY();
	void SetMousePosX(float x);
	void SetMousePosY(float y);

	void SetMouseButTrig( int i);
	void SetMouseButHeld( int i);
	void ClearMouseButTrig( int i);
	void ClearMouseButHeld( int i);

	void SetKeyboard( char c);
	void ClearKeyboardData();
	void ClearMouseData();

	// sets the ratio of physical screen resolution
	// to the fixed game one
	void SetViewportRatio(float xRatio, float yRatio);

private:

	//structures for holding data
	KEYBOARD m_keyboard;
	MOUSE m_mouse;

	float m_xRatio;
	float m_yRatio;
};
