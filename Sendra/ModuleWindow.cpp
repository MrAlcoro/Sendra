#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name.assign("window");
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		if (App->config != NULL)
		{
			window_object = json_object_dotget_object(App->modules_object, "window");

			if (window_object != NULL)
			{
				window_title = json_object_dotget_string(window_object, "title");
				SetTitle(window_title);
				LOG("Title: %s\n", window_title);

				wwidth = json_object_dotget_number(window_object, "width");
				wheight = json_object_dotget_number(window_object, "height");
				wscale = json_object_dotget_number(window_object, "scale");
				wfullscreen = json_object_dotget_boolean(window_object, "fullscreen");
				wresizable = json_object_dotget_boolean(window_object, "resizable");
				wborderless = json_object_dotget_boolean(window_object, "borderless");
				wdesktop = json_object_dotget_boolean(window_object, "windowed_fullscreen");
				wvsync = json_object_dotget_boolean(window_object, "vsync");
			}
			else
			{
				window_title = "Sendra";
				wwidth = 1280;
				wheight = 1024;
				wscale = 1;
				wfullscreen = false;
				wresizable = false;
				wborderless = false;
				wdesktop = false;
				wvsync = true;
			}
		}
		else
		{
			window_title = "Sendra";
			wwidth = 1280;
			wheight = 1024;
			wscale = 1;
			wfullscreen = false;
			wresizable = false;
			wborderless = false;
			wdesktop = false;
			wvsync = true;
		}

		//Create window
		int width = wwidth * wscale;
		int height = wheight * wscale;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if (wfullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (wresizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (wborderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (wdesktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

const char* ModuleWindow::GetTitle()
{
	return(window_title);
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetFullscreen(bool fullscreen)
{
	if (fullscreen == true)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	wfullscreen = fullscreen;
}

void ModuleWindow::SetBorderless(bool borderless)
{
	if (borderless == true)
		SDL_WINDOW_BORDERLESS;

	wborderless = borderless;
}

void ModuleWindow::SetResizable(bool resizable)
{
	if (resizable == true)
		SDL_SetWindowResizable(window, SDL_TRUE);
	else
		SDL_SetWindowResizable(window, SDL_FALSE);

	wresizable = resizable;
}

void ModuleWindow::SetFullDesktop(bool full_desktop)
{
	if (full_desktop == true)
		SDL_WINDOW_FULLSCREEN_DESKTOP;

	wdesktop = full_desktop;
}

bool ModuleWindow::Save()
{
	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) == false)
		{
			json_object_dotset_string(App->modules_object, "window.title", window_title);
			json_object_dotset_number(App->modules_object, "window.width", wwidth);
			json_object_dotset_number(App->modules_object, "window.height", wheight);
			json_object_dotset_number(App->modules_object, "window.scale", wscale);
			json_object_dotset_boolean(App->modules_object, "window.fullscreen", wfullscreen);
			json_object_dotset_boolean(App->modules_object, "window.resizable", wresizable);
			json_object_dotset_boolean(App->modules_object, "window.borderless", wborderless);
			json_object_dotset_boolean(App->modules_object, "window.windowed_fullscreen", wdesktop);
			json_object_dotset_boolean(App->modules_object, "window.vsync", wvsync);

			json_serialize_to_file_pretty(App->config, "config.json");
		}
		else
		{
			json_object_dotset_string(App->modules_object, "window.title", window_title);
			json_object_dotset_number(App->modules_object, "window.width", wwidth);
			json_object_dotset_number(App->modules_object, "window.height", wheight);
			json_object_dotset_number(App->modules_object, "window.scale", wscale);
			json_object_dotset_boolean(App->modules_object, "window.fullscreen", wfullscreen);
			json_object_dotset_boolean(App->modules_object, "window.resizable", wresizable);
			json_object_dotset_boolean(App->modules_object, "window.borderless", wborderless);
			json_object_dotset_boolean(App->modules_object, "window.windowed_fullscreen", wdesktop);
			json_object_dotset_boolean(App->modules_object, "window.vsync", wvsync);

			json_serialize_to_file_pretty(App->config, "config.json");
		}
	}
	else
	{
		json_object_dotset_string(App->modules_object, "window.title", window_title);
		json_object_dotset_number(App->modules_object, "window.width", wwidth);
		json_object_dotset_number(App->modules_object, "window.height", wheight);
		json_object_dotset_number(App->modules_object, "window.scale", wscale);
		json_object_dotset_boolean(App->modules_object, "window.fullscreen", wfullscreen);
		json_object_dotset_boolean(App->modules_object, "window.resizable", wresizable);
		json_object_dotset_boolean(App->modules_object, "window.borderless", wborderless);
		json_object_dotset_boolean(App->modules_object, "window.windowed_fullscreen", wdesktop);
		json_object_dotset_boolean(App->modules_object, "window.vsync", wvsync);

		json_serialize_to_file_pretty(App->config, "config.json");
	}

	LOG("Saving module %s", name.data());
	return(true);
}

bool ModuleWindow::Load()
{
	bool ret = false;

	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) != false)
		{
			window_title = json_object_dotget_string(window_object, "title");

			wwidth = json_object_dotget_number(window_object, "width");
			wheight = json_object_dotget_number(window_object, "height");
			wscale = json_object_dotget_number(window_object, "scale");
			wfullscreen = json_object_dotget_boolean(window_object, "fullscreen");
			wresizable = json_object_dotget_boolean(window_object, "resizable");
			wborderless = json_object_dotget_boolean(window_object, "borderless");
			wdesktop = json_object_dotget_boolean(window_object, "windowed_fullscreen");
			wvsync = json_object_dotget_boolean(window_object, "vsync");

			LOG("Loading module %s", name.data());
			ret = true;
		}
		else
		{
			LOG("Could not find the node named %s inside the file config.json", name.data());
			ret = false;
		}

	}
	else
	{
		LOG("Document config.json not found.");
		ret = false;
	}

	return ret;
}