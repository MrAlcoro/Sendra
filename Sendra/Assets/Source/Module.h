#ifndef __MODULEMODULE_H__
#define __MODULEMODULE_H__

#include <string>

class Application;

class Module
{
private :
	bool enabled;

public:
	Application* App;
	std::string name;

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool Save()
	{
		return(true);
	}

	virtual bool Load()
	{
		return(true);
	}
};
#endif