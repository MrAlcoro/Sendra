#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"


class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Draw();
	void ShowPropertiesWindow();
	void ShowAboutWindow();
	void RequestBrowser(const char* website_url);

private:

	bool showDemoWindow = false;
	bool showPropertiesWindow = false;
	bool showAboutWindow = false;
};