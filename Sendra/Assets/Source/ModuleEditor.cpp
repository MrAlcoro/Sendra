#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "glew.h"
#include "version.h"


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

	ConsoleClearLog();

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
		static char title[64] = "Sendra"; 
		ImGui::InputText("Engine's name", title, 64);
		static char organization[64] = "CITM UPC";
		ImGui::InputText("Organization", organization, 64);
		ImGui::Separator();
		ImGui::SliderInt("Max FPS", &fps, 0, 347);
		ImGui::Text("Limit Framerate: %d", fps);
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			App->window->SetFullscreen(fullscreen);
			fullscreen = !fullscreen;
		}

		if (ImGui::Checkbox("Borderless", &borderless))
		{
			App->window->SetBorderless(borderless);
			borderless = !borderless;
		}

		if (ImGui::Checkbox("Resizable", &resizable))
		{
			App->window->SetResizable(resizable);
			resizable = !resizable;
		}

		if (ImGui::Checkbox("Full desktop", &full_desktop))
		{
			App->window->SetFullDesktop(full_desktop);
			full_desktop = !full_desktop;
		}
			
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::TextWrapped("Mouse position:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "X: %i ", App->input->GetMouseX());
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "Y: %i", App->input->GetMouseY());
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::BulletText("SDL version: v%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::BulletText("CPUs: %d", SDL_GetCPUCount());
		ImGui::SameLine();
		ImGui::Text("(Cache: %dkb)", SDL_GetCPUCacheLineSize());
		float mebibytes = SDL_GetSystemRAM();
		float gigabytes = mebibytes * 0.001048576;
		ImGui::BulletText("RAM configured: %.2fGb", gigabytes);
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
		ImGui::Separator();
		ImGui::Spacing();
		char* gl_renderer = (char*)glGetString(GL_RENDERER);
		char* gl_vendor = (char*)glGetString(GL_VENDOR);
		ImGui::BulletText("GPU: %s", gl_renderer);
		ImGui::BulletText("Brand: %s", gl_vendor);		
	}

	ImGui::End();
}

void ModuleEditor::ShowAboutWindow()
{
	ImGui::SetNextWindowPos(ImVec2(1000, 400), ImGuiCond_Appearing, ImVec2(0.5f, 0.4f));
	ImGui::SetNextWindowSize(ImVec2(500, 450));
	ImGui::Begin("About");

	ImGui::Text("Sendra\n");
	ImGui::Text("3D game engine made by");
	ImGui::SameLine();
	if (ImGui::MenuItem("Alexis Cosano Rodriguez.")) { RequestBrowser("https://github.com/MrAlcoro"); };
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("External resources used:\n");
	ImGui::BulletText("ImGui v%s", ImGui::GetVersion());
	ImGui::SameLine();
	if (ImGui::Button("ImGui"))
		RequestBrowser("https://github.com/ocornut/imgui/tree/docking");

	ImGui::BulletText("SDL v%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
	ImGui::SameLine();
	if (ImGui::Button("SDL"))
		RequestBrowser("https://libsdl.org");

	ImGui::BulletText("Glew v%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
	ImGui::SameLine();
	if (ImGui::Button("Glew"))
		RequestBrowser("http://glew.sourceforge.net/");

	ImGui::BulletText("OpenGL v%s", glGetString(GL_VERSION));
	ImGui::SameLine();
	if (ImGui::Button("OpenGL"))
		RequestBrowser("https://www.opengl.org/");

	ImGui::BulletText("Assimp v%d.%d", aiGetVersionMajor(), aiGetVersionMinor());
	ImGui::SameLine();
	if (ImGui::Button("Assimp"))
		RequestBrowser("https://github.com/assimp/assimp");

	ImGui::BulletText("DevIL");
	ImGui::SameLine();
	if (ImGui::Button("DevIL"))
		RequestBrowser("https://openil.sourceforge.net/download.php");

	ImGui::BulletText("Parson v%d.%d", PARSON_VERSION_MAJOR, PARSON_VERSION_MINOR);
	ImGui::SameLine();
	if (ImGui::Button("Parson"))
		RequestBrowser("https://github.com/kgabis/parson");

	ImGui::BulletText("MathGeoLib");
	ImGui::SameLine();
	if (ImGui::Button("MathGeoLib"))
		RequestBrowser("https://github.com/juj/MathGeoLib");

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

void ModuleEditor::ConsoleClearLog()
{
	console_buffer.clear();
	console_scroll = true;
}

void ModuleEditor::ConsoleLog(const char* console_log)
{
	console_buffer.append(console_log);
	console_scroll = true;
}

void ModuleEditor::ShowConsoleWindow()
{
	ImGui::SetNextWindowPos(ImVec2(600, 850), ImGuiCond_Appearing, ImVec2(0.5f, 0.4f));
	ImGui::SetNextWindowSize(ImVec2(800, 200));
	ImGui::Begin("Console");

	ImGui::TextUnformatted(console_buffer.begin());

	if (console_scroll)
		ImGui::SetScrollHereY(1.0f);

	console_scroll = false;

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