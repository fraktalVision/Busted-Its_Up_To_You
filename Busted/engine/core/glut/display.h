#ifndef _GLUT_DISPLAY_FILE_H_
#define _GLUT_DISPLAY_FILE_H_

#include "engine/tools/textdisplay/TextManager.h"

class Display {
public:
	Display();
	//Make a grid (for orientation)
	void drawGrid(void);
	void drawScreenGrid();
	//Default glEnables and other GL settings
	void LoadGLDefaults();
	void UpdateLighting();
	void UpdateCompanyScreen();
	void display(void);
	void reshape(int w, int h);

private:
	enum SPLASH_FADE { FADE_IN, FADE_WAIT, FADE_OUT, FADE_NONE };
	enum SPLASH_STATE { SPLASH_COPY, SPLASH_LOGO, SPLASH_NONE };
	SPLASH_FADE FADE_S;
	SPLASH_STATE SPLASH_S;

	float splashFade;
	float splashWait;

	float splashFadeRem;
	float splashWaitRem;

	// physical monitor dimensions
	int width;
	int height;
};

extern Display * display;

#endif