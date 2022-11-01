#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "parson.h"
#include <vector>


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
	void ShowConsoleWindow();
	void ShowHierarchyWindow();
	void RequestBrowser(const char* website_url);

	bool Save();
	bool Load();

	void ConsoleClearLog();
	void ConsoleLog(const char* console_log);

private:

	bool show_demo_window = false;
	bool show_properties_window = false;
	bool show_about_window = false;
	bool show_console_window = false;
	bool show_hierarchy_window = true;
	JSON_Object* editor_object;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	int fps = 0;
	bool console_scroll = true;
	ImGuiTextBuffer console_buffer;
};
#endif