#pragma once

#include <mutex>
#include <vector>
#include <optional>

#include "../driver/driver.hpp"

#include "structures/structures.hpp"

namespace engine::offsets::core
{
	static struct g_objects
	{
		std::uintptr_t self = 0x67F3C00;

		std::uint8_t list_size = 0x14;
	} g_objects;

	static struct fname_array
	{
		std::uintptr_t self = 0x67DB3C0;

		std::uint8_t size = 0x10;
	} fname_array;

	static struct g_world
	{
		std::uintptr_t self = 0x693C640;

		std::uint8_t u_level = 0x48;

		std::uint32_t owning_game_instance = 0x1D8;
	} g_world;
}

namespace engine::offsets::u_level
{
	constexpr auto actors = 0x98;
}

namespace engine::offsets::u_game_instance
{
	constexpr auto local_player = 0xB8;
}

namespace engine::offsets::actor
{
	constexpr auto pawn = 0x428;

	constexpr auto root_component = 0x158;
}

namespace engine::offsets::u_player
{
	constexpr auto player_controller = 0x30;

	constexpr auto controller_player_state = 0x340;
}

namespace engine::offsets::player_controller
{
	constexpr auto character = 0x288;

	constexpr auto is_local_player_controller = 0x57C;

	constexpr auto acknowledged_pawn = 0x2C8;

	constexpr auto control_rotation = 0x2B0;
}

namespace engine::offsets::character
{
	constexpr auto camera_manager = 0x438;
}

namespace engine::offsets::camera_manager
{
	constexpr auto camera_cache_private = 0x1A40;

	constexpr auto location = 0x0;
	constexpr auto rotation = 0xC;
	
	constexpr auto pov = 0x10;
	constexpr auto fov = 0x18;
}

namespace engine::offsets::pawn
{
	static struct player_controller
	{
		std::uint32_t self = 0x30;

		std::uint32_t mesh = 0x398;

		std::uint32_t character = 0x288;

		std::uint32_t pawn_player_state = 0x340;
	} player_controller;

	static struct health
	{
		std::uint32_t self = 0xd28;

		std::uint32_t max_health = 0x16D0;
	} health;
}

namespace engine::offsets::mesh
{
	constexpr auto cached_bone_space = 0x518;

	constexpr auto component_to_world = 0x1F0;

	constexpr auto b_recently_rendered = 0x597;
}

namespace engine::types
{
	struct gworld_t;
	struct fnames_t;
	struct gobjects_t;
	struct player_t;
	struct player_controller_t;
}

namespace engine::values
{
	extern types::gworld_t gworld;
	extern types::fnames_t fnames;
	extern types::gobjects_t gobjects;
	extern std::uintptr_t process_base;
	extern HWND game_handle;
}

namespace engine::types
{
	enum actor_types_t : std::uintptr_t
	{
		dshk_shielded = 0x368A8C8
	};

	struct base_t
	{
		std::uintptr_t address;

		template< actor_types_t type >
		bool is_a( )
		{
			const auto vftable = driver.read< std::uintptr_t >( address );

			return vftable == engine::values::process_base + type;
		}

		bool operator==( const base_t& obj )
		{
			return address == obj.address;
		}

		bool operator==( std::uintptr_t comparative )
		{
			return address == comparative;
		}

		operator bool()
		{
			return address != 0;
		}
	};

	template<class t, std::uint64_t offset> 
	struct tarray_t
	{
		std::uint32_t get_size( std::uintptr_t base_address )
		{
			if( !base_address )
				return 0u;

			return driver.read< std::uint32_t >( base_address + offset + sizeof( std::uintptr_t ) );
		}

		[[nodiscard]] std::vector< t > get_members( std::uintptr_t base_address )
		{
			std::vector< t > members;

			members.reserve( get_size( base_address ) );

			if ( !base_address )
				return members;

			if ( const auto list_start = driver.read< std::uintptr_t >( base_address + offset ) )
			{
				for ( auto i = list_start; i < list_start + members.capacity( ) * sizeof( std::uintptr_t ); i += sizeof( std::uintptr_t ) )
				{
					if ( auto member = driver.read< t >( i ) )
					    members.push_back( member );
				}
			}

			return members;
		}
	};

	struct mesh_t : public base_t
	{
		std::optional< engine::structs::transform_t > get_bone( engine::structs::bones bone ) const;

		std::optional< engine::structs::transform_t > get_bone_int( int bone ) const;

		bool is_recently_rendered( );

		engine::structs::transform_t get_component_to_world( );
	};

	struct player_state_t : public base_t
	{
		std::uint8_t get_team_id( );
	};

	struct pawn_t : public base_t
	{
		mesh_t get_mesh( );

		player_controller_t get_player_controller( );

		player_state_t get_player_state( );

		float get_health( );

		float get_max_health( );
	};

	struct actor_t : public base_t
	{
		pawn_t get_pawn( );

		mesh_t get_root_component( );
	};

	struct camera_manager_t : public base_t
	{
		engine::structs::vector_t get_position( );

		engine::structs::vector_t get_rotation( );

		float get_fov( );
	};

	struct character_t : public base_t
	{
		camera_manager_t get_camera_manager( );
	};

	struct player_controller_t : public base_t
	{
		pawn_t get_pawn();

		bool is_local_player( );

		character_t get_character( );

		player_state_t get_player_state( );

		camera_manager_t get_camera_manager( );
	};

	struct player_t : public base_t
	{
		player_controller_t get_player_controller( );
	};

	struct ugame_instance_t : public base_t
	{
		tarray_t< player_t, offsets::u_game_instance::local_player > local_player;
	};

	struct ulevel_t : public base_t
	{
		tarray_t< actor_t, engine::offsets::u_level::actors > actors;

		void get_aimbot_players( engine::types::ugame_instance_t o_g_i );

		void get_visual_players( engine::types::ugame_instance_t o_g_i );
	};

	struct gworld_t : public base_t
	{
		ulevel_t get_ulevel( );

		ugame_instance_t get_owning_game_instance( );
	};

	struct fnames_t : public base_t
	{

	};

	struct gobjects_t : public base_t
	{

	};
}

namespace engine::functions
{
	void initiate( );

	extern std::mutex visual_actor_lock;

	extern std::vector< engine::types::actor_t > visual_actors;

	extern std::mutex aim_actor_lock;

	extern std::vector< engine::types::actor_t > aim_actors;

	engine::structs::matrix4x4_t to_scaled_matrix( const engine::structs::transform_t& transform );

	engine::structs::matrix4x4_t matrix_multiply( const engine::structs::matrix4x4_t& lhs, const engine::structs::matrix4x4_t& rhs );

	engine::structs::vector_t get_bone_position( engine::types::actor_t player, engine::structs::bones bone );

	engine::structs::vector_t get_bone_position_int( engine::types::actor_t player, int bone );

	engine::structs::rotator_t calculate_angle( engine::structs::vector_t local_player, engine::structs::vector_t target_player );

	std::pair< std::int32_t, std::int32_t > get_screen_size( );

	engine::structs::matrix3x4_t get_rotation_matrix( );

	bool world_to_screen( structs::vector_t& in, structs::vector_t& out );
}