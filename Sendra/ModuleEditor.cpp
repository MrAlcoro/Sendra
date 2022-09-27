#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "glew.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name.assign("editor");
}

// Destructor
ModuleEditor::~ModuleEditor()
{}

// Called before render is available
bool ModuleEditor::Init()
{
	LOG("Creating Editor");
	bool ret = true;

	if (App->config != NULL)
	{
		editor_object = json_object_dotget_object(App->modules_object, "editor");

		if (editor_object != NULL)
		{
			showDemoWindow = json_object_dotget_boolean(editor_object, "show_demo");
			showPropertiesWindow = json_object_dotget_boolean(editor_object, "show_properties");
			showAboutWindow = json_object_dotget_boolean(editor_object, "show_about");
			showConsoleWindow = json_object_dotget_boolean(editor_object, "show_console");
		}
		else
		{
			showDemoWindow = false;
			showPropertiesWindow = false;
			showAboutWindow = false;
			showConsoleWindow = false;
		}
	}
	else
	{
		showDemoWindow = false;
		showPropertiesWindow = false;
		showAboutWindow = false;
		showConsoleWindow = false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleEditor::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleEditor::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Destroying Editor");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::RequestBrowser(const char* website_url)
{
	ShellExecuteA(NULL, "open", website_url, NULL, NULL, 0);
}

void ModuleEditor::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (showDemoWindow)
		ImGui::ShowDemoWindow();

	if (showPropertiesWindow)
		ShowPropertiesWindow();

	if (showAboutWindow)
		ShowAboutWindow();

	if (showConsoleWindow)
		ShowConsoleWindow();

	// Main menu bar ------------------------------------------------------------
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				App->input->quit = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Save", "CTRL + S"))
				App->CallSave();

			if (ImGui::MenuItem("Load", "CTRL + L"))
				App->CallLoad();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Properties"))
				showPropertiesWindow = !showPropertiesWindow;

			if (ImGui::MenuItem("Console"))
				showConsoleWindow = !showConsoleWindow;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("ImGui Demo"))
				showDemoWindow = !showDemoWindow;

			if (ImGui::MenuItem("About"))
				showAboutWindow = !showAboutWindow;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::ShowPropertiesWindow()
{
	ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_Appearing, ImVec2(0.5f, 0.4f));
	ImGui::SetNextWindowSize(ImVec2(500, 650));
	ImGui::Begin("Properties");
	
	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::TextWrapped("Engine name: Sendra");
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::TextWrapped("Mouse position:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "X: %i ", App->input->GetMouseX());
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "Y: %i", App->input->GetMouseY());
	}

	if (ImGui::CollapsingHeader("User's information"))
	{
		ImGui::BulletText("CPU cache line size: %d", SDL_GetCPUCacheLineSize());
		ImGui::BulletText("CPU cores available: %d", SDL_GetCPUCount());
		ImGui::BulletText("RAM configured: %d", SDL_GetSystemRAM());
		ImGui::BulletText("RAM configured: %d", SDL_GetSystemRAM());
		ImGui::BulletText("Has 3DNow!: %d", SDL_Has3DNow());
		ImGui::BulletText("Has AltiVec: %d", SDL_HasAltiVec());
		ImGui::BulletText("Has AVX: %d", SDL_HasAVX());
		ImGui::BulletText("Has AVX2: %d", SDL_HasAVX2());
		ImGui::BulletText("Has MMX: %d", SDL_HasMMX());
		ImGui::BulletText("Has RDTSC: %d", SDL_HasRDTSC());
		ImGui::BulletText("Has SSE: %d", SDL_HasSSE());
		ImGui::BulletText("Has SSE2: %d", SDL_HasSSE2());
		ImGui::BulletText("Has SSE3: %d", SDL_HasSSE3());
		ImGui::BulletText("Has SSE41: %d", SDL_HasSSE41());
		ImGui::BulletText("Has SSE42: %d", SDL_HasSSE42());
	}

	ImGui::End();
}

void ModuleEditor::ShowAboutWindow()
{
	ImGui::SetNextWindowPos(ImVec2(1000, 400), ImGuiCond_Appearing, ImVec2(0.5f, 0.4f));
	ImGui::SetNextWindowSize(ImVec2(500, 450));
	ImGui::Begin("About");

	ImGui::Text("Sendra\n3D game engine made by Alexis Cosano Rodriguez.\n\n");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("External resources used:\n");
	ImGui::BulletText("ImGui v%s", ImGui::GetVersion());
	ImGui::SameLine();
	if (ImGui::Button("Download"))
		RequestBrowser("https://github.com/ocornut/imgui/tree/docking");

	ImGui::BulletText("SDL v%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
	ImGui::SameLine();
	if (ImGui::Button("Download"))
		RequestBrowser("https://libsdl.org/");

	ImGui::BulletText("Glew v%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
	ImGui::SameLine();
	if (ImGui::Button("Download"))
		RequestBrowser("http://glew.sourceforge.net/");

	ImGui::BulletText("OpenGL v%s", glGetString(GL_VERSION));
	ImGui::SameLine();
	if (ImGui::Button("Download"))
		RequestBrowser("https://www.opengl.org/");

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::TextWrapped("MIT License"

		"Copyright(c) 2022 Alexis Cosano\n\n"

		"Permission is hereby granted, free of charge, to any person obtaining a copy"
		"of this software and associated documentation files (the 'Software'), to deal"
		"in the Software without restriction, including without limitation the rights"
		"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell"
		"copies of the Software, and to permit persons to whom the Software is"
		"furnished to do so, subject to the following conditions:\n\n"

		"The above copyright notice and this permission notice shall be included in all"
		"copies or substantial portions of the Software.\n\n"

		"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE"
		"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"
		"SOFTWARE.\n\n\n");

	ImGui::End();
}

void ModuleEditor::ShowConsoleWindow()
{
	ImGui::SetNextWindowPos(ImVec2(600, 850), ImGuiCond_Appearing, ImVec2(0.5f, 0.4f));
	ImGui::SetNextWindowSize(ImVec2(800, 200));
	ImGui::Begin("Console");


	ImGui::End();
}

bool ModuleEditor::Save()
{
	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) == false)
		{
			json_object_dotset_boolean(App->modules_object, "editor.show_demo", showDemoWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_properties", showPropertiesWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_about", showAboutWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_console", showConsoleWindow);
			json_serialize_to_file_pretty(App->config, "config.json");
		}
		else
		{
			json_object_dotset_boolean(App->modules_object, "editor.show_demo", showDemoWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_properties", showPropertiesWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_about", showAboutWindow);
			json_object_dotset_boolean(App->modules_object, "editor.show_console", showConsoleWindow);
			json_serialize_to_file_pretty(App->config, "config.json");
		}

		LOG("Saving module %s", name.data());
	}
	else
	{
		json_object_dotset_boolean(App->modules_object, "editor.show_demo", showDemoWindow);
		json_object_dotset_boolean(App->modules_object, "editor.show_properties", showPropertiesWindow);
		json_object_dotset_boolean(App->modules_object, "editor.show_about", showAboutWindow);
		json_object_dotset_boolean(App->modules_object, "editor.show_console", showConsoleWindow);
		json_serialize_to_file_pretty(App->config, "config.json");

		LOG("Saving module %s", name.data());
	}


	return(true);
}

bool ModuleEditor::Load()
{
	bool ret = false;

	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) != false)
		{
			LOG("Loading module %s", name.data());

			showDemoWindow = json_object_dotget_boolean(editor_object, "show_demo");
			showPropertiesWindow = json_object_dotget_boolean(editor_object, "show_properties");
			showAboutWindow = json_object_dotget_boolean(editor_object, "show_about");
			showConsoleWindow = json_object_dotget_boolean(editor_object, "show_console");

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