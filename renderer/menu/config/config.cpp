#include "config.hpp"

#include "../menu.hpp"

std::unordered_map< const char*, std::variant<bool, int, float, engine::structs::vector4_t> > menu::config::config_map
{
	{ "aimbot_enable",			false },
	{ "aimbot_ignore_team",     false },
	{ "aimbot_smoothness",		5 },
	{ "aimbot_bone",		    0 },
	{ "aimbot_max_distance",	80 },
	{ "aimbot_show_fov",		false },
	{ "aimbot_fov",	            50 },
	{ "aimbot_fov_segments",	 8 },
	{ "aimbot_hotkey",			VK_RBUTTON },

	{ "triggerbot_enable",		false },
	{ "triggerbot_hotkey",		VK_LBUTTON },

	{ "enable_no_recoil",		false },
	{ "enable_fast_shoot",      false },
	{ "enable_fast_ads",	    false },

	{ "visuals_max_distance",	80 },
	{ "visuals_ignore_team",	false },
	{ "visuals_bounding_box",	false },
	{ "visuals_filled_box",	    false },
	{ "visuals_corner_box",	    false },
	{ "visuals_head_hitbox",	false },
	{ "visuals_skeleton",	    false },
	{ "visuals_distance",	    false },
	{ "visuals_health",	        false },

	{ "bounding_box_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "filled_box_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "corner_box_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "hitbox_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "skeleton_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "distance_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
	{ "health_color", engine::structs::vector4_t{ 0.f, 1.f, 0.f, 1.f } },
	{ "fov_color", engine::structs::vector4_t{ 1.f, 1.f, 1.f, 1.f } },
};