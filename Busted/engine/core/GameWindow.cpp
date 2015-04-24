
#include "all.h"
#include "GameWindow.h"
#include "glut/Input.h"
//#include <windows.h>
#include <stdio.h>


#pragma comment(lib, "gdi32.lib")

extern class DataDrive* DATA_MAN;
extern class Input* pInput;

bool CamControl = false;



#pragma warning( disable : 4244 )
#pragma warning( disable : 4312 )


GameWindow::GameWindow( const std::string &title, bool fullscreen, int iconfile): 
                m_title(title), isFullScreen(fullscreen), m_ICO(iconfile)
{}
    

GameWindow::~GameWindow()
{
    UnregisterClass((LPCSTR)m_title.c_str(), m_hInstance);
}

void GameWindow::Open()
{
    int width = DATA("SCREEN_WIDTH");
    int height = DATA("SCREEN_HEIGHT");
    int bits = 32;

    WNDCLASS    wc;                        // Windows Class Structure
    DWORD        dwExStyle;                // Window Extended Style
    DWORD        dwStyle;                // Window Style

    m_hInstance            = GetModuleHandle(NULL);                // Grab An Instance For Our Window
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;    // Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc        = (WNDPROC) StaticWindowProc;            // WndProc Handles Messages
    wc.cbClsExtra        = 0;                                    // No Extra Window Data
    wc.cbWndExtra        = 0;                                    // No Extra Window Data
    wc.hInstance        = m_hInstance;                            // Set The Instance
    wc.hIcon            = LoadIcon(NULL, IDI_WINLOGO);            // Load The Default Icon
    wc.hCursor            = LoadCursor(NULL, IDC_ARROW);            // Load The Arrow Pointer
    wc.hbrBackground    = NULL;                                    // No Background Required For GL
    wc.lpszMenuName        = NULL;                                    // We Don't Want A Menu
    wc.lpszClassName    = (LPCSTR)m_title.c_str();                // Set The Class Name

    if (!RegisterClass(&wc))                                    // Attempt To Register The Window Class
    {
        throw std::runtime_error("Failed To Register The Window Class.");
    }
    

	if (isFullScreen)                                                // Attempt Fullscreen Mode?
    {
        DEVMODE dmScreenSettings;                                // Device Mode
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));    // Makes Sure Memory's Cleared
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);        // Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth    = width;                // Selected Screen Width
        dmScreenSettings.dmPelsHeight    = height;                // Selected Screen Height
        dmScreenSettings.dmBitsPerPel    = bits;                    // Selected Bits Per Pixel
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			SetLastError(ERROR_BAD_ARGUMENTS);
            throw std::runtime_error("Failed to change display resolution");
        }
    }

    if (isFullScreen)                                            // Are We Still In Fullscreen Mode?
    {
        dwExStyle=WS_EX_APPWINDOW;                                // Window Extended Style
        dwStyle=WS_POPUP;                                        // Windows Style
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;            // Window Extended Style
        dwStyle=WS_OVERLAPPEDWINDOW;                            // Windows Style
    }

    // Create The Window
    if (!(m_hWnd=CreateWindowEx(dwExStyle,                           // Extended Style For The Window
                                (LPCSTR)m_title.c_str(),             // Class Name
                                m_title.c_str(),                     // Window Title
                                dwStyle |                            // Defined Window Style
                                WS_CLIPSIBLINGS |                    // Required Window Style
                                WS_CLIPCHILDREN,                     // Required Window Style
                                0, 0,                                // Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
                                NULL,                                // No Parent Window
                                NULL,                                // No Menu
                                m_hInstance,                            // Instance
                                NULL)))                                // Dont Pass Anything To WM_CREATE
    {
		throw std::runtime_error("Window Creation Error");
    }

    ShowWindow(m_hWnd,SW_SHOW);                        // Show The Window
    SetForegroundWindow(m_hWnd);                        // Slightly Higher Priority
    SetFocus(m_hWnd);                                    // Sets Keyboard Focus To The Window
}

void GameWindow::Close()
{
	if (isFullScreen) {
		ChangeDisplaySettings(NULL, 0);
	}
	if (m_hWnd && IsWindow(m_hWnd)) {
		DestroyWindow(m_hWnd);
	}
	m_hWnd = NULL;	
	if (m_hInstance) {
		UnregisterClass(m_title.c_str(), m_hInstance);
	}
	m_hInstance=NULL;
}

HWND GameWindow::GetHandle() const
{
    return m_hWnd;
}

bool GameWindow::IsOpen()
{
    return ::IsWindow(m_hWnd) == TRUE;
}

LRESULT GameWindow::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(GetHandle(), msg, wParam, lParam);
}

LRESULT CALLBACK GameWindow::StaticWindowProc(HWND m_hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE: 
        display->reshape(LOWORD(lParam),HIWORD(lParam));
        break;

    case WM_DESTROY: 
		PostQuitMessage(0); 
		break;

    case WM_CHAR: 
		pInput->SetKeyboard( wParam ); 
        if(pInput->PollKey('c')&& DATA("debug") ) {
			CamControl = !CamControl; 
		}
		break;
    case WM_LBUTTONDOWN: 
		pInput->SetMouseButTrig( MLEFT ); 
		break;

    case WM_RBUTTONDOWN: 
		pInput->SetMouseButTrig( MRIGHT ); 
		break;

    case WM_MOUSEMOVE: 
        {
            int x=(short)LOWORD(lParam);
            int y=(short)HIWORD(lParam);
            //float x = (lParam); float y = GET_Y_LPARAM(lParam);
            pInput->SetMousePosX(x); pInput->SetMousePosY(y); 



            vec2 input = {x, y};
            vec2 orig = Camera->getOrigMouse();
            vec2 diffTrans = {input.y - orig.y, input.x - orig.x};
            GLfloat diff = DATA("SCREEN_WIDTH") - input.x;

            if( CamControl )//DATA("debug") )
            {
                switch(wParam)
                {
                    case MK_LBUTTON:
                    {
                        vec3 newRot = Camera->getRot();
                

                        newRot.x += diffTrans.x/5;
                        newRot.y += diffTrans.y/5;

                        //if(DATA("debug")) { std::cout << "Left button x:" << newRot.x << " y:" << newRot.y << std::endl; }
                        Camera->setRot(newRot);
                    }
                        break;
                    case MK_MBUTTON:
                    {
                        vec4 pos = Camera->getPos();

                        vec3 Rot = Camera->getRot();

                        float cos0 = (cos((180.0f-Rot.y )/180.0f*3.14159f));
                        float sin0 = (sin((180.0f-Rot.y )/180.0f*3.14159f));
                        float cos1 = cos((180.0f-Rot.y+90.0f)/180.0f*3.14159f);
                        float sin1 = sin((180.0f-Rot.y+90.0f)/180.0f*3.14159f);


                        pos.x -= cos1*diffTrans.x/7.5  - sin1*diffTrans.y/7.5 ;
                        pos.z -= -cos0*diffTrans.x/7.5  + sin0*diffTrans.y/7.5 ;;
                        Camera->setPos(pos);
                        //if(DATA("debug")) { std::cout << "Middle button" << std::endl; }
                    }
                        break;
                    case MK_RBUTTON:
                    {
                        //vec4 pos 
                        float d    = Camera->getCamDistance();

                        if(diff < 0)
                            diff = -1 * diff;

                        if(orig.x > input.x)
                            d += diff/100 + d/100;
                        else if (orig.x < input.x) // do nothing on =
                            d -= diff/100 + d/100;

                        Camera->setCamDistance(d);
                    }
                        //if(DATA("debug")) { std::cout << "Right button" << std::endl; }
                        break;
                    default:
                        break;
                }// switch

            }
            Camera->setOrigMouse(input);
        }
        break;
    }
    return (DefWindowProc(m_hWnd, msg, wParam, lParam));
}




GraphicsEngine::GraphicsEngine()
{
    m_glrc    = NULL;
    m_hDC    = NULL;
    m_hWnd    = NULL;

}


void GraphicsEngine::Init(GameWindow& window)
{
	int bits = 32;

    m_hWnd = window.GetHandle();
    
    static PIXELFORMATDESCRIPTOR pfd=                // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),                // Size Of This Pixel Format Descriptor
        1,                                            // Version Number
        PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
        PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                            // Must Support Double Buffering
        PFD_TYPE_RGBA,                                // Request An RGBA Format
        bits,                                        // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                            // Color Bits Ignored
        0,                                            // No Alpha Buffer
        0,                                            // Shift Bit Ignored
        0,                                            // No Accumulation Buffer
        0, 0, 0, 0,                                    // Accumulation Bits Ignored
        16,                                            // 16Bit Z-Buffer (Depth Buffer)  
        0,                                            // No Stencil Buffer
        0,                                            // No Auxiliary Buffer
        PFD_MAIN_PLANE,                                // Main Drawing Layer
        0,                                            // Reserved
        0, 0, 0                                        // Layer Masks Ignored
    };
    
    if (!(m_hDC=GetDC(m_hWnd)))                            // Did We Get A Device Context?
    {
		throw std::runtime_error("Can't Create A GL Device Context");
    }

    GLuint        PixelFormat;            // Holds The Results After Searching For A Match
    if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))    // Did Windows Find A Matching Pixel Format?
    {
		throw std::runtime_error("Can't Find A Suitable PixelFormat");
    }

    if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))        // Are We Able To Set The Pixel Format?
    {
		throw std::runtime_error("Can't Set The PixelFormat");
    }

    if (!(m_glrc=wglCreateContext(m_hDC)))                // Are We Able To Get A Rendering Context?
    {
		throw std::runtime_error("Can't Create A GL Rendering Context");
    }

    if(!wglMakeCurrent(m_hDC,m_glrc))                    // Try To Activate The Rendering Context
    {
		throw std::runtime_error("Can't Activate The GL Rendering Context");
    }

	display->LoadGLDefaults();
}

void GraphicsEngine::Release()
{
	if (m_glrc)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			throw std::runtime_error("Release Of DC And RC Failed");
		}

		if (!wglDeleteContext(m_glrc))						// Are We Able To Delete The RC?
		{
			throw std::runtime_error("Release Rendering Context Failed");
		}
		m_glrc = NULL;										// Set RC To NULL
	}

	if (m_hDC && !ReleaseDC(m_hWnd,m_hDC))					// Are We Able To Release The DC
	{
		throw std::runtime_error("Release Device Context Failed");
	}
	m_hDC = NULL;										// Set DC To NULL
}

void GraphicsEngine::RenderScreen()
{
    glFlush();
    glFinish();
    SwapBuffers( m_hDC);
}


