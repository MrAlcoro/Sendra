#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "imgui_impl_sdl.h"


#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name.assign("input");
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		if (App->config != NULL)
		{
			input_object = json_object_dotget_object(App->modules_object, "input");
		}
	}

	return ret;
}

FILE_TYPE ModuleInput::GetFileType(std::string file_name)
{
	std::string type = file_name.substr(file_name.find_last_of("."));

	if (type == ".fbx" || type == ".obj" ||
		type == ".FBX" || type == ".OBJ")
	{
		return MODEL;
	}

	else if (type == ".png" || type == ".jpg" || type == ".bmp" || type == ".dds" ||
		type == ".PNG" || type == ".JPG" || type == ".BMP" || type == ".DDS")
	{
		return TEXTURE;
	}
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch(e.type)
		{
			FILE_TYPE type;

			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_DROPFILE:
				file_path = e.drop.file;
				type = GetFileType(file_path.c_str());
				if (type == MODEL)
				{
					if (App->loader->meshes.size() != 0)
					{
						App->loader->meshes.clear();
						App->loader->LoadFile(file_path.c_str());
					}
					else
						App->loader->LoadFile(file_path.c_str());

					App->loader->Focus();
				}
				else if (type == TEXTURE)
				{
					App->loader->SetTexture(file_path.c_str());
				}

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	if (keyboard[SDL_SCANCODE_LCTRL] == KEY_REPEAT && keyboard[SDL_SCANCODE_S] == KEY_UP)
		App->CallSave();

	if (keyboard[SDL_SCANCODE_LCTRL] == KEY_REPEAT && keyboard[SDL_SCANCODE_L] == KEY_UP)
		App->CallLoad();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool ModuleInput::Save()
{
	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) == false) 
		{
			json_object_set_null(App->modules_object, name.data());
			json_serialize_to_file_pretty(App->config, "config.json");
		}
	}
	else
	{
		json_object_set_null(App->modules_object, name.data());
		json_serialize_to_file_pretty(App->config, "config.json");
	}

	LOG("Saving module %s", name.data());
	return(true);
}

bool ModuleInput::Load()
{
	bool ret = false;

	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) != false)
		{
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