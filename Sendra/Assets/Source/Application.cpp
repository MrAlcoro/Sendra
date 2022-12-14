#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	loader = new ModuleLoader(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes
	AddModule(scene);
	AddModule(loader);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);
}

Application::~Application()
{
	// release modules
	list<Module*>::reverse_iterator item;
	item = list_modules.rbegin();

	while (item != list_modules.rend())
	{
		delete* item;
		item++;
	}
	list_modules.clear();

}

bool Application::Init()
{
	bool ret = true;

	config = json_parse_file("config.json");

	if (config != NULL)
	{
		modules_object = json_value_get_object(config);

		LOG("Configuration loaded successfully.");
	}
	else
	{
		config = json_value_init_object();
		modules_object = json_value_get_object(config);
		json_serialize_to_file(config, "config.json");

		LOG("Configuration not found. Initializing config file...");
		LOG("You're using Sendra's default settings.");
	}	

	// Call Init() in all modules

	list<Module*>::iterator item_list;
	Module* it;

	for (item_list = list_modules.begin(); item_list != list_modules.end() && ret == true; ++item_list) {

			it = *item_list;
			ret = it->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (item_list = list_modules.begin(); item_list != list_modules.end() && ret == true; ++item_list) {

			it = *item_list;
			ret = it->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	list<Module*>::iterator item_list;
	Module* it;

	for (item_list = list_modules.begin(); item_list != list_modules.end() && ret == UPDATE_CONTINUE; ++item_list) {

			it = *item_list;
			ret = it->PreUpdate(dt);
	}

	for (item_list = list_modules.begin(); item_list != list_modules.end() && ret == UPDATE_CONTINUE; ++item_list) {

			it = *item_list;
			ret = it->Update(dt);
	}

	for (item_list = list_modules.begin(); item_list != list_modules.end() && ret == UPDATE_CONTINUE; ++item_list) {

			it = *item_list;
			ret = it->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	list<Module*>::reverse_iterator item;
	item = list_modules.rbegin();

	while (item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}

	return ret;
}

void Application::AddConsoleLog(const char* console_log)
{
	editor->ConsoleLog(console_log);
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::CallSave()
{
	LOG("Saving engine's configuration...");

	std::list<Module*>::iterator iterator = list_modules.begin();

	while (iterator != list_modules.end())
	{
		iterator._Ptr->_Myval->Save();
		iterator++;
	}	
}

void Application::CallLoad()
{
	LOG("Loading engine's configuration...");

	std::list<Module*>::iterator iterator = list_modules.begin();

	while (iterator != list_modules.end())
	{
		iterator._Ptr->_Myval->Load();
		iterator++;
	}
}