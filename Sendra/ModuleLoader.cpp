#include "Application.h"
#include "ModuleLoader.h"


ModuleLoader::ModuleLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name.assign("loader");
}

ModuleLoader::~ModuleLoader() {}

bool ModuleLoader::Start()
{
	bool ret = true;

	return ret;
}

update_status ModuleLoader::PreUpdate(float dt)
{
	return(UPDATE_CONTINUE);
}

update_status ModuleLoader::Update(float dt)
{
	return (UPDATE_CONTINUE);
}

bool ModuleLoader::CleanUp()
{
	LOG("Cleaning loader");

	return true;
}

bool ModuleLoader::Save()
{
	LOG("Saving module %s", name.data());
	return(true);
}

bool ModuleLoader::Load()
{
	LOG("Loading module %s", name.data());
	return(true);
}