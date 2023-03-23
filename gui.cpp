#include "gui.h"
#include <stdexcept>
#include "src/hacks/misc.cpp"
#include <format>
#include <iostream>
#include "ragebot.h"
#include "ext/imgui/imgui_stdlib.h"
#include "ext/imgui/imgui_internal.h"
#include "src/util/small_pixel.h"
#include "src/util/whitney.h"

#pragma warning(disable : 4996)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

// window process
LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

bool gui::SetupWindowClass(const char* windowClassName) noexcept
{
	// populate window class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	// register
	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

void gui::DestroyWindowClass() noexcept
{
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept
{
	// create temp window
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		100,
		100,
		0,
		0,
		windowClass.hInstance,
		0
	);

	if (!window)
		return false;

	return true;
}

void gui::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

bool gui::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
		return false;

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(
		handle,
		"Direct3DCreate9"
	));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&device
	) < 0) return false;

	

	return true;
}

void gui::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}
void gui::Setup()
{

	if (!SetupWindowClass("hackClass001"))
		throw std::runtime_error("Failed to create window class.");

	if (!SetupWindow("Hack Window"))
		throw std::runtime_error("Failed to create window.");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create device.");

	DestroyWindow();
	DestroyWindowClass();
}

ImFont* smallest_pixel;

void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
		);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromMemoryCompressedTTF(whitney_compressed_data, whitney_compressed_size, 16.5, NULL, io.Fonts->GetGlyphRangesDefault());
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\comic.ttf", 18, NULL, io.Fonts->GetGlyphRangesDefault());
	smallest_pixel = io.Fonts->AddFontFromMemoryCompressedTTF(smallest_pixel_compressed_data, smallest_pixel_compressed_size, 16, NULL, io.Fonts->GetGlyphRangesDefault());
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(20,20);
	style->WindowRounding = 5.0f;
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;
	style->ChildRounding = 1.0f;
	style->ItemInnerSpacing = ImVec2(1, 1);
	style->ItemSpacing = ImVec2(1, 1);
	style->TabRounding = 1.f;
	style->Colors[ImGuiCol_Text] = ImVec4(0.8, 0.8, 0.8, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.86f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.65f); //17,25,37,255 0,223,255,255
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.65f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.192f, 0.192f, 0.192f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.721f, 0.321f, 1.f, 0.9f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.65f, 0.f, 1.00f);
	style->WindowBorderSize = 0.f;

	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.117f, 0.117f, 0.117f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);

	style->ChildBorderSize = 1.0f;
	

	style->WindowTitleAlign = ImVec2(0.50f, 0.50f);
	style->WindowMenuButtonPosition = ImGuiDir_None;
	style->Colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.01568f, 0.23921f, 0.36470f, 1.00f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.01568f, 0.23921f, 0.36470f, 1.00f);
	style->FramePadding = ImVec2(20, 3);
	style->Colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.8f);

	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.f);

	style->Colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.3f, 0.4f, 1.f);



	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void gui::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// retore wnd proc
	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

void gui::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	static float map_c[3] = { 0.f, 0.f, 0.f };
	static float c_v[3] = { 0.02f, 0.5f, 0.76f };
	static float c_o[3] = { 0.36f, 0.01f, 1.f };
	static float circle_c[3] = { 52.f, 52.f, 52.f };
	static float esp_b[3] = { 52.f, 52.f, 52.f };
	static float esp_snap[3] = { 52.f, 52.f, 52.f };
	static float esp_skel[3] = { 52.f, 52.f, 52.f };

	static float fr_c_v[3] = { 0.5f, 0.5f, 0.76f };
	static float fr_c_o[3] = { 0.1f, 0.3f, 0.76f };

	drawList = ImGui::GetBackgroundDrawList();
	
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);

	char buffer[9]; // allocate space for the formatted string
	strftime(buffer, 9, "%T", timeinfo);

	char init[64] = "RolliHook | User-Mode Release | ";
	strcat_s(init, buffer);

	ImGui::PushFont(smallest_pixel);
	drawList->AddRectFilled(ImVec2(2, 2), ImVec2(320, 30), IM_COL32(42, 42, 42, 255), 0.8f, ImDrawListFlags_AntiAliasedLines);
	drawList->AddText(ImVec2(10, 7.5), IM_COL32(153, 90, 173, 255), init);
	ImGui::PopFont();
	if (config::visualize_aimbot_fov && globals::localPlayer && globals::localPlayer->IsAlive())
	{
		drawList->AddCircle(ImVec2(1920 / 2, 1080 / 2), config::bestFov * 12.5f, IM_COL32(circle_c[0] * 255, circle_c[1] * 255, circle_c[2] * 255, 255), 0, 1.f);
	}
	

	const char* materialItems[] = { "Normal", "Glass" };
	const char* hitboxItems[] = { "Head", "Neck", "Chest", "Stomach", "Pelvis", "Legs" };
	static bool selected[5];
	static std::string prv;
	std::vector<std::string> vec;

	if (gui::render_menu)
	{
		//ImGui::SetNextWindowSize(ImVec2(675, 570));
		
		ImGui::Begin("rollihook", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		
		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_FittingPolicyResizeDown))
		{
			ImGui::PushStyleVar(11, ImVec2(60.f, 3.0f));
			if (ImGui::BeginTabItem("Visuals"))
			{
				
				ImGui::BeginChild("Visual Settings", ImVec2(300, 230), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
				{
					if (ImGui::BeginTabBar("Visual Settings2", ImGuiTabBarFlags_None))
					{
						ImGui::PushStyleVar(11, ImVec2(20.f, 3.0f));
						if (ImGui::BeginTabItem("ESPs"))
						{
							ImGui::Checkbox(" [esp] - 2d box ", &config::esp);
							ImGui::SameLine();
							ImGui::ColorEdit3("   ", esp_b, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

							ImGui::Checkbox(" [esp] - snapline   ", &config::esp_snapline);
							ImGui::SameLine();
							ImGui::ColorEdit3("  ", esp_snap, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

							ImGui::Checkbox(" [esp] - draw skeleton   ", &config::esp_skeleton);
							ImGui::SameLine();
							ImGui::ColorEdit3(" ", esp_skel, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);


							ImGui::Checkbox(" [esp] - healthbar ", &config::esp_healthbar);
							ImGui::Checkbox(" [esp] - health counter ", &config::esp_display_hp_text);


							ImGui::EndTabItem();

							config::esp_box_apply_clr(esp_b[0], esp_b[1], esp_b[2]);
							config::esp_snap_apply_clr(esp_snap[0], esp_snap[1], esp_snap[2]);
						}
						if (ImGui::BeginTabItem("Flags"))
						{
							ImGui::Checkbox(" [flags] - show name", &config::esp_name);
							
							ImGui::Checkbox(" [flags] - show weapon", &config::esp_weapon);
							ImGui::Checkbox(" [flags] - show scoped", &config::esp_show_if_scoped);
							ImGui::Checkbox(" [flags] - show flashed", &config::esp_flashed);
							ImGui::Checkbox(" [flags] - show defuser", &config::esp_defuser);
							ImGui::EndTabItem();
						}

						ImGui::PopStyleVar();
						
					}
					ImGui::EndTabBar();

				}
				ImGui::EndChild();
				ImGui::SameLine();

				ImGui::BeginChild("Chams/Etc", ImVec2(350, 230), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
				{

					ImGui::Checkbox("Enable Chams", &config::chams);
					ImGui::Text("");
					if (config::chams) {
						ImGui::Checkbox("Draw Enemies", &config::chams_drawenemies);
						if (config::chams_drawenemies)
						{
							ImGui::ColorEdit3("[enemies] visible", c_v, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit3("[enemies] occluded", c_o, ImGuiColorEditFlags_NoInputs);
							ImGui::Text("");
						}
						ImGui::Checkbox("Draw Friendlies", &config::chams_drawfriendlies);

						if (config::chams_drawfriendlies) {
							ImGui::ColorEdit3("[friendly] visible", fr_c_v, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit3("[friendly] occluded", fr_c_o, ImGuiColorEditFlags_NoInputs);
						}

						//ImGui::ListBox("Material", 1, materialItems, 2);

						// bool enemy, bool visible, float r,g,b

						config::apply_clr(true, true, c_v[0], c_v[1], c_v[2]);
						config::apply_clr(true, false, c_o[0], c_o[1], c_o[2]);

						config::apply_clr(false, true, fr_c_v[0], fr_c_v[1], fr_c_v[2]);
						config::apply_clr(false, false, fr_c_o[0], fr_c_o[1], fr_c_o[2]);

					}

					ImGui::Text("");
					ImGui::Text("Global Settings");

					ImGui::Checkbox(" [map] nightmode", &config::map_color);
					ImGui::Checkbox(" [map] custom color", &config::map_cst_colors);
					ImGui::SameLine();
					ImGui::ColorEdit3(" ", map_c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);


					config::map_color_apply_clr(map_c[0], map_c[1], map_c[2]);
				}

				ImGui::EndChild();

				//ImGui::SetNextItemWidth(95.0f);
				ImGui::BeginChild("###Remove", ImVec2(370, 230), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
				{
					ImGui::Checkbox(" [vs] remove shadows", &config::remove_shadows);

					ImGui::Checkbox(" [vs] remove 3d skybox", &config::remove_3dsky);

					ImGui::Checkbox(" [vs] disable post-processing", &config::disable_post_processing);

					ImGui::Checkbox(" [vs] remove flashbangs", &config::no_flash);

					ImGui::Checkbox(" [vs] remove scope fov", &config::scope_fov_override);

					ImGui::Checkbox(" [vs] remove black scope", &config::remove_blackscope);

					ImGui::Checkbox(" [vs] remove aim punch", &config::RemoveAimPunch);
					ImGui::Checkbox(" [vs] remove view punch", &config::RemoveViewPunch);

				}
				ImGui::EndChild();
				ImGui::SameLine();



				ImGui::BeginChild("###LocalVisuals", ImVec2(280, 230), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
				{

					ImGui::Checkbox("Override Viewmodel", &config::override_viewmodel);
					if (config::override_viewmodel)
					{
						ImGui::SetNextItemWidth(100.f);
						ImGui::SliderFloat("X Offset", &config::view_x, 0.f, 200.f, "X - %.1f");
						ImGui::SetNextItemWidth(100.f);
						ImGui::SliderFloat("Y Offset", &config::view_y, 0.f, 200.f, "Y - %.1f");
						ImGui::SetNextItemWidth(100.f);
						ImGui::SliderFloat("Z Offset", &config::view_z, 0.f, 200.f, "Z - %.1f");
						ImGui::SetNextItemWidth(100.f);
						ImGui::SliderFloat("Roll Offset", &config::view_roll, 0.f, 200.f, "Z - %.1f");


					}



					ImGui::Checkbox(" Thirdperson", &config::thirdperson);



					ImGui::Checkbox(" Override FOV", &config::fov_changer);
					ImGui::SameLine();
					ImGui::SetNextItemWidth(95.0f);
					ImGui::SliderFloat("", &config::view_fov, 90.5f, 180.f, "%.1f");

					ImGui::Checkbox(" Aspect Ratio", &config::aspectRatio);
					ImGui::SameLine();
					ImGui::SetNextItemWidth(95.0f);
					ImGui::SliderFloat(" ", &config::aspectRatioAngle, 0.1f, 1.7f, "%.1f");

				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			ImGui::PopStyleVar();

			ImGui::PushStyleVar(11, ImVec2(60.f, 3.0f));
			if (ImGui::BeginTabItem("Misc"))
			{
				ImGui::BeginChild("##MiscStuff", ImVec2(650, 450), true, ImGuiWindowFlags_AlwaysAutoResize);
				{
					ImGui::Checkbox(" [misc] buylog", &config::buylogs);
					if (config::buylogs) {
						ImGui::Checkbox(" [bl] ignore teammates", &config::ignore_buylogs_teammates);
					}
					ImGui::Checkbox(" [misc] bunnyhop", &config::bunnyhop);
					ImGui::Checkbox(" [misc] autostrafer", &config::auto_strafe);
					ImGui::Checkbox(" [misc] reveal plant", &config::reveal_plant);
					ImGui::Checkbox(" [misc] clantag", &config::clanTag);
					ImGui::Checkbox(" [misc] hitsound", &config::hitsound);
					if (config::hitsound)
					{
						ImGui::Checkbox(" Custom Hitsound", &config::customHitSound);
						ImGui::SetNextItemWidth(95.0f);
						ImGui::InputText("Sound File", &config::customSoundName);
					}
					ImGui::Checkbox(" Trashtalk", &config::trashtalk);
				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			ImGui::PopStyleVar();

			ImGui::PushStyleVar(11, ImVec2(60.f, 3.0f));
			if (ImGui::BeginTabItem("Ragebot"))
			{
				ImGui::BeginChild("##RagebotStuff", ImVec2(650, 450), true, ImGuiWindowFlags_AlwaysAutoResize);
				{
					ImGui::Checkbox("Ragebot (Barebone)", &config::ragebot);

					ImGui::Checkbox("Auto Stop", &config::autostop);

					ImGui::Text("Hitboxes");
					ImGui::SetNextItemWidth(100.0f);
					if (ImGui::BeginCombo("Hitboxes", prv.c_str()))
					{
						prv = "";

						for (size_t i = 0; i < IM_ARRAYSIZE(hitboxItems); i++)
						{
							ImGui::Selectable(hitboxItems[i], &selected[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							if (selected[i])
								vec.push_back(hitboxItems[i]);
						}

						for (size_t i = 0; i < vec.size(); i++)
						{
							if (vec.size() == 1)
								prv += vec.at(i);
							else if (!(i == vec.size()))
								prv += vec.at(i) + ", ";
							else {
								prv += vec.at(i);
							}
						}

						ImGui::EndCombo();
					}

					ImGui::SetNextItemWidth(95.0f);
					if (ImGui::Button("Apply Changes"))
					{
						hacks::hitboxes.clear();


						std::string delimiter = ", ";
						std::vector<std::string> v = split(prv, delimiter);

						// its so fucking late and i cannot come up with
						// a better working solution, im going to fucking
						// kill myself soon i swear


						// i - string
						// v - entire list

						//0,1,2,3,4             5 total


						for (auto i : v) {
							//std::cout << i << std::endl;

							if (i == "Head")
							{
								hacks::hitboxes.push_back(8);
								continue;
							}
							if (i == "Neck")
							{
								hacks::hitboxes.push_back(7);
								continue;
							}
							if (i == "Chest")
							{
								hacks::hitboxes.push_back(6);
								continue;
							}
							if (i == "Stomach")
							{
								hacks::hitboxes.push_back(4);
								continue;
							}
							if (i == "Pelvis")
							{
								hacks::hitboxes.push_back(3);
								continue;
							}
							if (i == "Legs")
							{
								hacks::hitboxes.push_back(77);
								hacks::hitboxes.push_back(70);
								hacks::hitboxes.push_back(78);
								hacks::hitboxes.push_back(79);
								continue;
							}
						}




						for (size_t i = 0; i < hacks::hitboxes.size(); i++)
						{
							std::cout << hacks::hitboxes[i] << std::endl;
						}
					}
					ImGui::Text("");
					ImGui::Text("Anti-Aim");

					ImGui::Checkbox("Enable", &config::anti_aim);
					ImGui::SliderFloat("Pitch", &config::anti_aim_pitch, -89.f, 89.f, "%.1f");
					ImGui::Checkbox("Force Moonwalk", &config::anti_aim_moonwalk);
				}
				ImGui::EndChild();
				ImGui::EndTabItem();

			}
			ImGui::PopStyleVar();

			ImGui::PushStyleVar(11, ImVec2(60.f, 3.0f));
			if (ImGui::BeginTabItem("Legit"))
			{
				ImGui::BeginChild("##Legits", ImVec2(650,450), true, ImGuiWindowFlags_AlwaysAutoResize);
				{
					ImGui::Checkbox(" [legit] aimbot", &config::aimbot);
					ImGui::Checkbox(" [legit] silent aim", &config::aimbot_quiet);
					ImGui::Checkbox(" [legit] smoothing/magnetize", &config::legit_magnet);
					if (config::legit_magnet) {
						ImGui::SetNextItemWidth(95.0f);
						ImGui::SliderFloat(" [smoothing] scale", &config::legitbot_scaling, 0.f, 1.f, "%.1f");
					}
					ImGui::Checkbox(" [legit] display aimbot fov", &config::visualize_aimbot_fov);
					ImGui::SameLine();

					ImGui::ColorEdit3("", circle_c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::SetNextItemWidth(120.0f);
					ImGui::SliderFloat(" [fov] scale", &config::bestFov, 1.5f, 255.f, "%.1f");
					ImGui::EndTabItem();
				}
				ImGui::EndChild();
			}
			ImGui::PopStyleVar();
		}

		ImGui::EndTabBar();
	}
	
	
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
)
{
	// toogle menu
	if (GetAsyncKeyState(VK_F10) & 1)
		gui::open = !gui::open;

	if (GetAsyncKeyState(VK_INSERT) & 1)
		gui::render_menu = !gui::render_menu;

	// pass messages to imgui
	if (gui::open && ImGui_ImplWin32_WndProcHandler(
		window,
		message,
		wideParam,
		longParam
	)) return 1L;

	return CallWindowProc(
		gui::originalWindowProcess,
		window,
		message,
		wideParam,
		longParam
	);
}
