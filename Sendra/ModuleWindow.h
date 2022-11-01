#ifndef __MODULEWINDOW_H__
#define __MODULEWINDOW_H__

#include "Module.h"
#include "Assets/Libraries/SDL/include/SDL.h"
#include "Assets/Libraries/Parson/parson.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	const char* GetTitle();
	void SetFullscreen(bool fullscreen);
	void SetBorderless(bool borderless);
	void SetResizable(bool resizable);
	void SetFullDesktop(bool fullscreen);

	bool Save();
	bool Load();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	const char* window_title = "Config not found.";
	int wwidth;
	int wheight;
	int wscale;
	bool wfullscreen;
	bool wresizable;
	bool wborderless;
	bool wdesktop;
	bool wvsync;

private:

	JSON_Object* window_object;
};
#endif