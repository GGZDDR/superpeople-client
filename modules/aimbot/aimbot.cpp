#include <map>

#include "aimbot.hpp"

#include "../../engine/engine.hpp"

#include "../../renderer/renderer.hpp"

#include "../../renderer/menu/config/config.hpp"

auto bone_map = std::map< int, engine::structs::bones >
{
	{ 0, engine::structs::bones::head },
	{ 1, engine::structs::bones::neck },
	{ 2, engine::structs::bones::chest }
};

auto distance_from_cursor( const engine::structs::vector_t& target )
{
	const auto [ w, h ] = engine::functions::get_screen_size( );

	return std::sqrt( std::pow( target.x - w / 2, 2 ) + std::pow( target.y - h / 2, 2 ) );
}

auto modules::aimbot::draw_fov( ) -> void
{
	if ( std::get< bool >( menu::config::config_map[ "aimbot_show_fov" ] ) && std::get< int >( menu::config::config_map[ "aimbot_fov" ] ) > 0 )
    {
        const auto [ w, h ] = engine::functions::get_screen_size( );

        const auto draw_list = ImGui::GetBackgroundDrawList( );

        draw_list->AddCircle( { w / 2.f, h / 2.f }, std::get< int >( menu::config::config_map[ "aimbot_fov" ] ) * 10, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "fov_color" ] ).color[0], std::get< engine::structs::vector4_t >( menu::config::config_map[ "fov_color" ] ).color[1], std::get< engine::structs::vector4_t >( menu::config::config_map[ "fov_color" ] ).color[2], std::get< engine::structs::vector4_t >( menu::config::config_map[ "fov_color" ] ).color[3] }, std::get< int >( menu::config::config_map[ "aimbot_fov_segments" ] ), 2.f );
    }
}

void modules::aimbot::callback( )
{
	if ( !std::get< bool >( menu::config::config_map[ "aimbot_enable" ] ) )
		return;

	auto u_level = engine::values::gworld.get_ulevel( );

	if ( !u_level.address )
		return;

	auto owning_game_instance = engine::values::gworld.get_owning_game_instance( );

	if ( !owning_game_instance.address )
		return;

	std::unique_lock locked_table( engine::functions::aim_actor_lock );

	for ( auto player : engine::functions::aim_actors )
	{
		if ( !player.address )
			continue;

		if ( !player.get_pawn( ).address )
			continue;

		if ( player.get_pawn( ).get_player_controller( ).address )
			continue;

		if ( player.get_pawn( ).get_health( ) <= 0.f )
			continue;

		auto camera_manager = owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_camera_manager( );

		if ( !camera_manager.address )
			continue;

		if ( std::get< bool >( menu::config::config_map[ "aimbot_ignore_team" ] ) && owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_player_state( ).get_team_id( ) == player.get_pawn( ).get_player_state( ).get_team_id( ) || player.get_pawn( ).get_player_state( ).get_team_id( ) <= 0 )
			continue;

		auto bone_world_position = engine::functions::get_bone_position( player, bone_map[ std::get< int >( menu::config::config_map[ "aimbot_bone" ] ) ] );

		const auto distance = static_cast< std::uint32_t >( camera_manager.get_position( ).distance( bone_world_position ) / 100.f);

		if ( distance > std::get< int >( menu::config::config_map[ "aimbot_max_distance" ] ) )
			continue;

		auto relative_angle = engine::functions::calculate_angle( camera_manager.get_position( ), bone_world_position );

		if ( std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] ) > 0 )
		{
			auto original_angle = driver.read< engine::structs::rotator_t >( owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).address + engine::offsets::player_controller::control_rotation );

			auto difference = relative_angle - original_angle;

			difference.clamp( );

			relative_angle.pitch = original_angle.pitch + difference.pitch / std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] );
			relative_angle.yaw = original_angle.yaw + difference.yaw / std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] );
			relative_angle.roll = original_angle.roll + difference.roll / std::get< int >( menu::config::config_map[ "aimbot_smoothness" ] );
		}

		engine::structs::vector_t head_screen_position;

		if ( engine::functions::world_to_screen( bone_world_position, head_screen_position ) )
		{
			if ( distance_from_cursor( head_screen_position ) > std::get< int >( menu::config::config_map[ "aimbot_fov" ] ) * 10 )
				continue;

			if ( !GetAsyncKeyState( std::get< int >( menu::config::config_map[ "aimbot_hotkey" ] ) ) )
				continue;

		    driver.write< engine::structs::rotator_t >( owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).address + engine::offsets::player_controller::control_rotation, { relative_angle.pitch, relative_angle.yaw, relative_angle.roll } );
		}
	}
}