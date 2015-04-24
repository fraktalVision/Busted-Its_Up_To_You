/***********
Zachary Whaley
gamemogul@gmail.com
--ALL.H--
* Throw as many globally needed things in here for the base engine as possible
* There is also a 'busted.all.h' which houses the globals necessary for the game,
  this has the things necessary for the engine (portability)
***********/

//System libraries
	#pragma warning(disable : 4996)	//Annoying warning
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include <glut.h>
	#include <iostream>
	#include <iterator>
	#include <sstream>
	#include <string>
	#include <vector>
	#include <map>
	#include <queue>
	#include <fstream>
	
	#include <ctime> 
	#include <cstdlib>
	#include <list>
	#include <math.h>
	#include <assert.h>


	//#include "../glut/display.h"
    #include "glut/display.h"
	//#include "../textureload/textureImporter.h"
	#include "../tools/textureload/textureImporter.h"
	#include <sqlite3.h>


	//#include "../../tools/sqlite/SQLiteDB.h"
	#include "../tools/sqlite/SQLiteDB.h"
	//#include "../../tools/datadrive/datadrive.h"
	#include "../tools/datadrive/datadrive.h"
	//#include "../tools/textdisplay/TextManager.h"
	//#include "../ogg/ogg_stream.h"
	#include "engine/audio/ogg/stream/ogg_stream.h"
	//#include "../sound_manager/sound_manager.h"
	#include "engine/audio/sound_manager/sound_manager.h"
	#include "GameWindow.h"

	class ModelLoader;

	#include "../tools/importer/modelLoader.h"

	extern ModelLoader *pModel_man;

//Base-classes: thing, locator, obj, camera
	#include "classes.h"

//Structs, typedefs, vertext lists, vectors, etc
	#include "structures.h"

//SDL
	#include "../include/SDL.h"
	#include "../include/SDL_Image.h"

//opengl
	#pragma	comment(lib, "opengl32.lib")
	#pragma comment(lib, "glu32.lib")
//	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib, "glut32.lib")


//#ifndef _ALL_H_
//#define _ALL_H_

#define MODEL_OFFSET 0//-50
#define CAM_OFFSET -50
#define MAX_OBJECTS 200
#define BREAKLINE "//------------------------------\n//------------------------------"

extern camera* Camera;				//Camera
extern sound_manager *pMusic_man;

extern std::vector<obj*> objStack;	//Render stack

extern class DataDrive* DATA_MAN;	//pointer to the data manager (manages INI files)
extern class Input* pInput;			//Poll user-input
extern class Text_Manager* pText_man;
extern class Display * display;

#define DATA DATA_MAN->GetData

extern int	btnBtn, btnMod, btnState;

extern float SCREEN_X_RATIO;
extern float SCREEN_Y_RATIO;

//#endif