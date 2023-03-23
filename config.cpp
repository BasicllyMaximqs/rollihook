#include "config.h"


namespace config {

	void apply_aimbot_circle(float r, float g, float b)
	{

	}
	void esp_box_apply_clr(float r, float g, float b)
	{
		esp_box[0] = r;
		esp_box[1] = g;
		esp_box[2] = b;
	}
	void esp_snap_apply_clr(float r, float g, float b)
	{
		esp_snap[0] = r;
		esp_snap[1] = g;
		esp_snap[2] = b;
	}
	void map_color_apply_clr(float r, float g, float b) 
	{
		map_coloring[0] = r * 5;
		map_coloring[1] = g * 5;
		map_coloring[2] = b * 5;
	}


	void apply_clr(bool enemy, bool visible, float r, float g, float b)
	{
		if (!enemy)
		{
			if (visible) 
			{
				chams_friendly_visible[0] = r;
				chams_friendly_visible[1] = g;
				chams_friendly_visible[2] = b;
			}
			else {
				chams_friendly_occluded[0] = r;
				chams_friendly_occluded[1] = g;
				chams_friendly_occluded[2] = b;
			}
		}
		else {
			if (visible)
			{
				chams_visible[0] = r;
				chams_visible[1] = g;
				chams_visible[2] = b;
			}
			else

			{
				chams_occluded[0] = r;
				chams_occluded[1] = g;
				chams_occluded[2] = b;
			}
		}

	}
}
