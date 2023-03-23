#pragma once
#include <cstdint>
#include <string>

namespace config
{

	void apply_aimbot_circle(float r, float g, float b);
	void apply_clr(bool enemy, bool visible, float r, float g, float b);
	void esp_box_apply_clr(float r, float g, float b);
	void esp_snap_apply_clr(float r, float g, float b);
	void map_color_apply_clr(float r, float g, float b);

	inline bool RemoveAimPunch = false;

	inline bool map_color = false;

	inline bool RemoveViewPunch = false;
	
	inline bool bunnyhop = false;

	inline bool auto_strafe = false;

	inline bool legit_magnet = false;

	inline float legitbot_scaling = 0.f;

	inline bool aspectRatio = false;

	inline bool aimbot = false;
	inline bool aimbot_quiet = false;
	inline bool aimbot_aimjunkies = false;

	inline bool esp = false;

	inline bool esp_name = false;
	inline bool esp_weapon = false;

	inline bool esp_defuser = false;
	inline bool esp_bombcarrier = false;

	inline bool hitsound = false;
	inline bool esp_show_if_scoped = false;
	inline bool esp_healthbar = false;
	inline bool esp_snapline = false;

	inline bool visualize_aimbot_fov = false;

	inline bool no_flash = false;

	inline float aimbot_fov = 128.f;

	inline float aimbot_smooth = 10.f;

	inline float bestFov = { 5.5f };

	inline float aspectRatioAngle = 1.3f;

	inline float view_fov = 90.f;

	inline bool ragebot = false;

	inline bool autostop = false;

	inline bool scope_fov_override = false;

	inline bool anti_aim = false;
	inline bool anti_aim_moonwalk = false;

	inline bool anti_aim_fastduck = false;
	inline bool disable_post_processing = false;
	inline bool override_viewmodel = false;

	inline float anti_aim_pitch = 0.5f;

	inline bool fov_changer = false;

	inline bool thirdperson = false;
	
	inline bool trashtalk = false;

	inline bool remove_shadows = false;

	inline bool remove_blackscope = false;

	inline bool remove_3dsky = false;

	inline bool reveal_plant = false;

	inline bool clanTag = false;

	inline bool customHitSound = false;

	inline float view_x, view_y, view_z, view_roll;

	inline bool esp_display_hp_text = false;

	inline bool esp_flashed = false;

	inline std::string customSoundName;

	struct Color {
		std::uint8_t r{ }, g{}, b{ };
	};

	inline bool chams = false;


	inline bool esp_skeleton = false;
	
	inline bool buylogs = false;
	inline bool ignore_buylogs_teammates = false;

	inline bool map_cst_colors = false;

	inline int esp_skel[3] = { 1,1,1 };

	inline float map_coloring[3] = { 1.f, 1.f, 1.f };

	inline bool chams_drawenemies = true;

	inline bool chams_drawfriendlies = true;

	inline float chams_friendly_occluded[3] = { 1.f,1.f,1.f };
	inline float chams_friendly_visible[3] = { 1.f,1.f,1.f };

    inline float chams_occluded[3] = { 1.f, 1.f, 1.f };
	inline float chams_visible[3] = { 1.f, 1.f, 1.f };

	inline float esp_box[] = { 1.f, 1.f, 1.f };
	inline float esp_snap[] = { 1.f, 1.f, 1.f };

};

