#ifndef __MODULELOADER_H__
#define __MODULELOADER_H__

#include "Module.h"
#include "Globals.h"


class ModuleLoader : public Module
{
private:
	int random_number = 0;

public:
	ModuleLoader(Application* app, bool start_enabled = true);
	~ModuleLoader();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	// Save & load
	bool Save();
	bool Load();

};
#endif
