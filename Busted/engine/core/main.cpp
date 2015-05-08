//Base header that has as many engine-globals as possible, start here to learn the engine:
#include "all.h"

//The same, only globals for the game (separation between game and engine)
#include "../../busted/busted.all.h"

#include "../tools/importer/pinaImporter.h"            //Model importer
#include "../tools/importer/modelLoader.h"            //Model importer
#include "../../busted/Game_Manager/Game_Manager.h"
#include "GameWindow.h"

#pragma comment(lib, "User32.lib")

extern Game_Manager *Game_Man;
extern GraphicsEngine *Graph_En;
extern Texture_Manager *pTexture_man; //manages images
extern Mentor_Manager *pMentor_man;
extern Text_Manager *pText_man; //manages fonts and text

//GLUT interface headers...
#include "glut.h"
#include "glut/keys.h"
#include "glut/display.h"
#include "glut/specialKeys.h"

void init(void)
{
    display->LoadGLDefaults();

    pTexture_man = new Texture_Manager();
    pMentor_man = new Mentor_Manager();

    pMusic_man = new sound_manager("../Busted/assets/music/default_sounds.xtx");

    //load game structures
    Game_Man = new Game_Manager();

    //Situation database
    sitDB = new sqlDB("../Busted/assets/busted_db.db");
}

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

DEVMODE old_res;
DEVMODE new_res;
GameWindow * displayWindow;

std::string GetLastErrorText()
{
    char* msg;
	DWORD nErrorCode = GetLastError();
    // Ask Windows to prepare a standard message for a GetLastError() code:
    DWORD lcMessage = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
    // Return the message
    if (!lcMessage)
        return("Unknown error");
    else
        return(msg);
}

void clean()
{
	try {
		if (Graph_En) {
			Graph_En->Release();
			delete Graph_En;
			Graph_En = NULL;
		}
		for (size_t i = 0; i < objStack.size(); ++i) {
			delete objStack[i];
		}
		objStack.clear();
		if (Camera) {
			delete Camera;
			Camera = NULL;
		}
	} catch (...) {
	}
}

void showErrorMessage(const char * msg)
{
	std::string oss;
	oss = msg;
	oss += "\n" + GetLastErrorText();

	clean();
	ChangeDisplaySettings(NULL, 0);

	MessageBox(displayWindow ? displayWindow->GetHandle() : NULL
		, oss.c_str()
		, "Fatal Error"
		, MB_OK | MB_ICONERROR);
}

void start()
{
	try {
		DATA_MAN = new DataDrive();
		DATA_MAN->RefreshData();
		Camera = new camera;
		Camera->init();
		display = new Display();
	    pInput = new Input();
	} catch (const std::exception & e) {
		showErrorMessage(e.what());
		return;
	} catch (...) {
		showErrorMessage("Exception in data manager");
		return;
	}

	try {
	    displayWindow = new GameWindow("Busted!",DATA("FULL_SCREEN")==1.0f,0);
		displayWindow->Open();

		Graph_En = new GraphicsEngine();
		Graph_En->Init(*displayWindow);

		init();

		//game loop
		MSG  msg;
		while(displayWindow->IsOpen())
		{
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage (&msg);
			}
			else
			{
				//general update
				display->display();
			}
		}
		clean();
	} catch (const std::exception & e) {
		showErrorMessage(e.what());
	} catch (...) {
		showErrorMessage("Exception in main");
	}
}

int main(int argc, char** argv)
{
	start();
	if (displayWindow) {
		displayWindow->Close();
		delete displayWindow;
		displayWindow = NULL;
	}
	if (display) {
		delete display;
		display = NULL;
	}
	if (DATA_MAN) {
		delete DATA_MAN;
		DATA_MAN = NULL;
	}
	return 0;
}