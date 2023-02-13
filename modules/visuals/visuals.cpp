#include <string>

#include "visuals.hpp"

#include "../../engine/engine.hpp"

#include "../../renderer/renderer.hpp"

#include "../../renderer/menu/config/config.hpp"

void modules::visuals::callback( )
{
	auto ulevel = engine::values::gworld.get_ulevel( );

	if ( !ulevel.address )
		return;

	auto owning_game_instance = engine::values::gworld.get_owning_game_instance( );

	if ( !owning_game_instance.address )
		return;

	std::unique_lock locked_table( engine::functions::visual_actor_lock );

	for ( auto player : engine::functions::visual_actors )
	{
		if ( !player.address )
			continue;

		const auto player_team = player.get_pawn( ).get_player_state( ).get_team_id( );

		const auto local_player_team = owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_player_state( ).get_team_id( );

		if ( player.get_pawn( ).get_player_controller( ).address )
			continue;

		auto camera_manager = owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_camera_manager( );

		if ( !camera_manager.address )
			continue;

		auto head_world_position = engine::functions::get_bone_position( player, engine::structs::bones::head );
		auto root_world_position = engine::functions::get_bone_position( player, engine::structs::bones::root );

		const auto distance = static_cast< std::uint32_t >( camera_manager.get_position( ).distance( root_world_position ) / 100.f );

		engine::structs::vector_t head_screen_position, root_screen_position;

		if ( engine::functions::world_to_screen( head_world_position, head_screen_position ) && engine::functions::world_to_screen( root_world_position, root_screen_position ) )
		{
			auto player_pawn = player.get_pawn( );

			if ( !player_pawn.address )
				continue;

			const auto health = player_pawn.get_health( );

			if ( health <= 0.f || distance > std::get< int >( menu::config::config_map[ "visuals_max_distance" ] ) )
				continue;

			if ( std::get< bool >( menu::config::config_map[ "visuals_ignore_team" ] ) && local_player_team == player_team || player_team <= 0 )
			    continue;

			const auto draw_list = ImGui::GetBackgroundDrawList( );

			const float width = std::fabs( head_screen_position.y - root_screen_position.y ) / 4.0f;
			const float height = std::fabs( head_screen_position.y - root_screen_position.y ) * 1.2f;

			if ( !width || !height )
				continue;

			const auto line_len_width = width / 2.5f;

			if ( !root_screen_position.x || !root_screen_position.y )
				continue;

			if ( std::get< bool >( menu::config::config_map[ "visuals_bounding_box" ] ) && !std::get< bool >( menu::config::config_map[ "visuals_corner_box" ] ) )
			{
				{
					draw_list->AddRect( { root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - height }, ImColor{ std::get< engine::structs::vector4_t >(menu::config::config_map["bounding_box_color"]).color[0], std::get< engine::structs::vector4_t >(menu::config::config_map["bounding_box_color"]).color[1], std::get< engine::structs::vector4_t >(menu::config::config_map["bounding_box_color"]).color[2] } );
				}

				{
					draw_list->AddRect( { root_screen_position.x - width + 1, root_screen_position.y + 1 }, { root_screen_position.x + width + 1, root_screen_position.y - height + 1 }, ImColor{ 0, 0, 0 } );
				}
			}

			if ( std::get< bool >( menu::config::config_map[ "visuals_filled_box" ] ) )
			{
				{
					draw_list->AddRectFilled( { root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - height }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "filled_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "filled_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "filled_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "filled_box_color" ] ).color[ 3 ] } );
				}
			}

			if ( std::get< bool >( menu::config::config_map[ "visuals_corner_box" ] ) && !std::get< bool >(menu::config::config_map["visuals_bounding_box"]) )
			{
				{
					draw_list->AddLine( { root_screen_position.x - width, root_screen_position.y - height }, { root_screen_position.x - width + line_len_width, root_screen_position.y - height }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );
					draw_list->AddLine( { root_screen_position.x - width, root_screen_position.y - height }, { root_screen_position.x - width, (root_screen_position.y - height) + line_len_width }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );

					draw_list->AddLine( { root_screen_position.x + width, root_screen_position.y - height }, { root_screen_position.x + width - line_len_width, root_screen_position.y - height }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );
					draw_list->AddLine( { root_screen_position.x + width, root_screen_position.y - height }, { root_screen_position.x + width, (root_screen_position.y - height) + line_len_width }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );

					draw_list->AddLine( { root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x - width + line_len_width, root_screen_position.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );
					draw_list->AddLine( { root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x - width, root_screen_position.y - line_len_width }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );

					draw_list->AddLine( { root_screen_position.x + width, root_screen_position.y }, { root_screen_position.x + width - line_len_width, root_screen_position.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );
					draw_list->AddLine( { root_screen_position.x + width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - line_len_width }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "corner_box_color" ] ).color[ 3 ] } );
				}

				{
					draw_list->AddLine( { root_screen_position.x - width + 1, root_screen_position.y - height + 1 }, { root_screen_position.x - width + line_len_width, root_screen_position.y - height + 1 }, ImColor{ 0,0,0 } );
					draw_list->AddLine( { root_screen_position.x - width + 1, root_screen_position.y - height + 1 }, { root_screen_position.x - width + 1, (root_screen_position.y - height) + line_len_width }, ImColor{ 0,0,0 } );

					draw_list->AddLine( { root_screen_position.x + width - 1, root_screen_position.y - height + 1 }, { root_screen_position.x + width - line_len_width, root_screen_position.y - height + 1 }, ImColor{ 0,0,0 } );
					draw_list->AddLine( { root_screen_position.x + width - 1, root_screen_position.y - height + 1 }, { root_screen_position.x + width - 1, (root_screen_position.y - height) + line_len_width }, ImColor{ 0,0,0 } );

					draw_list->AddLine( { root_screen_position.x - width + 1, root_screen_position.y - 1 }, { root_screen_position.x - width + line_len_width, root_screen_position.y - 1 }, ImColor{ 0,0,0 } );
					draw_list->AddLine( { root_screen_position.x - width + 1, root_screen_position.y - 1 }, { root_screen_position.x - width + 1, root_screen_position.y - line_len_width - 1 }, ImColor{ 0,0,0 } );

					draw_list->AddLine( { root_screen_position.x + width - 1, root_screen_position.y - 1 }, { root_screen_position.x + width - line_len_width, root_screen_position.y - 1 }, ImColor{ 0,0,0 } );
					draw_list->AddLine( { root_screen_position.x + width - 1, root_screen_position.y - 1 }, { root_screen_position.x + width - 1, root_screen_position.y - line_len_width - 1 }, ImColor{ 0,0,0 } );
				}
            }

			if ( std::get< bool >( menu::config::config_map[ "visuals_skeleton" ] ) )
			{
				engine::structs::vector_t screen_r_shoulder,
					screen_l_shoulder,
					screen_r_elbow, screen_l_elbow,
					screen_r_hand, screen_l_hand,
					screen_spine_mid_bottom,
					screen_spine_bottom, screen_r_knee,
					screen_l_knee, screen_r_ankle, screen_l_ankle;

				{
					engine::structs::vector_t r_shoulder = engine::functions::get_bone_position( player, engine::structs::bones::right_shoulder );
					engine::structs::vector_t l_shoulder = engine::functions::get_bone_position( player, engine::structs::bones::left_shoulder );

					engine::structs::vector_t r_elbow = engine::functions::get_bone_position( player, engine::structs::bones::right_elbow );
					engine::structs::vector_t l_elbow = engine::functions::get_bone_position( player, engine::structs::bones::left_elbow );

					engine::structs::vector_t r_hand = engine::functions::get_bone_position( player, engine::structs::bones::right_hand );
					engine::structs::vector_t l_hand = engine::functions::get_bone_position( player, engine::structs::bones::left_hand );

					engine::structs::vector_t spine_mid_bottom = engine::functions::get_bone_position( player, engine::structs::bones::chest );

					engine::structs::vector_t spine_bottom = engine::functions::get_bone_position( player, engine::structs::bones::pelvis );

					engine::structs::vector_t r_knee = engine::functions::get_bone_position( player, engine::structs::bones::right_knee );
					engine::structs::vector_t l_knee = engine::functions::get_bone_position( player, engine::structs::bones::left_knee );

					engine::structs::vector_t r_ankle = engine::functions::get_bone_position( player, engine::structs::bones::right_foot );
					engine::structs::vector_t l_ankle = engine::functions::get_bone_position( player, engine::structs::bones::left_foot );

					{
						if
							( engine::functions::world_to_screen( r_knee, screen_r_knee ) &&
								engine::functions::world_to_screen( l_knee, screen_l_knee ) &&

								engine::functions::world_to_screen( r_ankle, screen_r_ankle ) &&
								engine::functions::world_to_screen( l_ankle, screen_l_ankle ) &&

								engine::functions::world_to_screen( r_shoulder, screen_r_shoulder ) &&
								engine::functions::world_to_screen( l_shoulder, screen_l_shoulder ) &&

								engine::functions::world_to_screen( r_elbow, screen_r_elbow ) &&
								engine::functions::world_to_screen( l_elbow, screen_l_elbow ) &&

								engine::functions::world_to_screen( r_hand, screen_r_hand ) &&
								engine::functions::world_to_screen( l_hand, screen_l_hand ) &&

								engine::functions::world_to_screen( spine_bottom, screen_spine_bottom ) &&
								engine::functions::world_to_screen( spine_mid_bottom, screen_spine_mid_bottom )
							    )
						{
							draw_list->AddLine( { screen_r_shoulder.x, screen_r_shoulder.y }, { screen_r_elbow.x, screen_r_elbow.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
							draw_list->AddLine( { screen_l_shoulder.x, screen_l_shoulder.y }, { screen_l_elbow.x, screen_l_elbow.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );

							draw_list->AddLine( { screen_r_elbow.x, screen_r_elbow.y }, { screen_r_hand.x, screen_r_hand.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
							draw_list->AddLine( { screen_l_elbow.x, screen_l_elbow.y }, { screen_l_hand.x, screen_l_hand.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );

							draw_list->AddLine( { screen_r_shoulder.x, screen_r_shoulder.y }, { screen_spine_mid_bottom.x, screen_spine_mid_bottom.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
							draw_list->AddLine( { screen_l_shoulder.x, screen_l_shoulder.y }, { screen_spine_mid_bottom.x, screen_spine_mid_bottom.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );

							draw_list->AddLine( { screen_spine_bottom.x, screen_spine_bottom.y }, { screen_spine_mid_bottom.x, screen_spine_mid_bottom.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );

							draw_list->AddLine( { screen_r_knee.x, screen_r_knee.y }, { screen_r_ankle.x, screen_r_ankle.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
							draw_list->AddLine( { screen_l_knee.x, screen_l_knee.y }, { screen_l_ankle.x, screen_l_ankle.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );

							draw_list->AddLine( { screen_r_knee.x, screen_r_knee.y }, { screen_spine_bottom.x, screen_spine_bottom.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
							draw_list->AddLine( { screen_l_knee.x, screen_l_knee.y }, { screen_spine_bottom.x, screen_spine_bottom.y }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "skeleton_color" ] ).color[ 3 ] } );
						}
					}
				}
			}

			if ( std::get< bool >( menu::config::config_map[ "visuals_head_hitbox" ] ) )
			{
				{
				    draw_list->AddRect( { head_screen_position.x, head_screen_position.y }, { head_screen_position.x + 5.f, head_screen_position.y + 5.f }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "hitbox_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "hitbox_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "hitbox_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "hitbox_color" ] ).color[ 3 ] } );
				}

				{
					draw_list->AddRect( { head_screen_position.x - 1, head_screen_position.y - 1 }, { ( head_screen_position.x + 5.f ) - 1, ( head_screen_position.y + 5.f ) - 1 }, ImColor{ 0, 0, 0 }, 0, 0, 0.5f );
				}
			}

			if ( std::get< bool >( menu::config::config_map[ "visuals_distance" ] ) )
			{ 
				std::string to_draw; to_draw.append( std::to_string( distance ) ); to_draw.append( "M" );

				{
					draw_list->AddLine( { head_screen_position.x, head_screen_position.y }, { head_screen_position.x - 5.f, head_screen_position.y - 10.f }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 3 ] } );
				}

				{
					draw_list->AddLine( { head_screen_position.x - 5.f, head_screen_position.y - 10.f }, { head_screen_position.x - 15.f, head_screen_position.y - 10.f }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 3 ] } );
				}

				draw_list->AddText( ImGui::GetDefaultFont( ), 10.5f, { head_screen_position.x - 15.f, head_screen_position.y - 20.f }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "distance_color" ] ).color[ 3 ] }, to_draw.c_str( ) );
			}

			const auto new_width_health = ( width ) / player_pawn.get_max_health( ) * health;

			if ( std::get< bool >( menu::config::config_map[ "visuals_health" ] ) )
			{
				std::string to_draw; to_draw.append( std::to_string( static_cast< int >( health ) ) ); to_draw.append( "HP" );

				{					
					draw_list->AddRectFilled( { root_screen_position.x + new_width_health, ( root_screen_position.y + ( height / 10.f ) * 1.2f ) - .5f }, { root_screen_position.x - new_width_health, ( root_screen_position.y + ( height / 10.f ) ) - .5f }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 3 ] } );
				}

				{
					draw_list->AddRect( { root_screen_position.x + new_width_health, ( root_screen_position.y + ( height / 10.f ) * 1.2f ) - .5f }, { root_screen_position.x - new_width_health, ( root_screen_position.y + ( height / 10.f ) ) - .5f }, ImColor{ 1, 1, 1 } );
				}

				draw_list->AddText( ImGui::GetDefaultFont( ), 9.5f, { (root_screen_position.x + width / 2) - (new_width_health / 2 ) - 12.f, ( root_screen_position.y + ( height / 10.f ) * 1.2f ) }, ImColor{ std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 0 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 1 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 2 ], std::get< engine::structs::vector4_t >( menu::config::config_map[ "health_color" ] ).color[ 3 ] }, to_draw.c_str( ) );
			}
		}
	}
}
