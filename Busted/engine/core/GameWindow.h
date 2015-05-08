#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

#include "all.h"

//extern bool CamControl;

//! basic window class for making a window
class GameWindow
{
public:
	GameWindow( const std::string &title, bool fullscreen, int iconfileICO = 0);
	~GameWindow();

	void Open();
	void Close();
	HWND GetHandle() const;

	bool IsOpen();

	bool isFullScreen;

private:
	HINSTANCE			m_hInstance;
	HWND				m_hWnd;
	DWORD				m_style;
	int					m_ICO;
	const std::string	m_title;

	LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

//! Class for setuping up and releasing OpenGL
class GraphicsEngine
{
public:
	GraphicsEngine();

	void Init(GameWindow& window);
	void Release();

	void RenderScreen();

private:

	HWND		m_hWnd;
	HDC			m_hDC;  //handle to device in context
	HGLRC		m_glrc;	//used for creating OpenGL render Device Context
};

#endif