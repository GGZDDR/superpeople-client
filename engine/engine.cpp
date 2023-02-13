#include "engine.hpp"

#include "../renderer/menu/config/config.hpp"

#include <ranges>
#include <cmath>

using namespace engine::types;

HWND engine::values::game_handle;

gworld_t engine::values::gworld;
fnames_t engine::values::fnames;
gobjects_t engine::values::gobjects;
std::uintptr_t engine::values::process_base;

std::mutex engine::functions::aim_actor_lock;

std::vector< engine::types::actor_t > temporary_aim_actors;

std::mutex engine::functions::visual_actor_lock;

std::vector < engine::types::actor_t > temporary_visual_actors;

std::vector< engine::types::actor_t > engine::functions::aim_actors;
std::vector< engine::types::actor_t > engine::functions::visual_actors;

constexpr auto bone_size = 0x30u;
constexpr auto M_PI = 3.14159265358979323846f;

std::optional< engine::structs::transform_t > engine::types::mesh_t::get_bone( engine::structs::bones bone ) const
{
	std::uint64_t bone_array = driver.read< std::uint64_t >( address + 16 * ( driver.read< std::uint32_t >( address + 0x484 ) + 0x44 ) );

	if ( !bone_array )
		return std::nullopt;

	return driver.read< engine::structs::transform_t >( bone_array + ( bone * bone_size ) );
}

std::optional< engine::structs::transform_t > engine::types::mesh_t::get_bone_int( int bone ) const
{
	std::uint64_t bone_array = driver.read< std::uint64_t >( address + 16 * ( driver.read< std::uint32_t >( address + 0x484 ) + 0x44 ) );

	if ( !bone_array )
		return std::nullopt;

	return driver.read< engine::structs::transform_t >( bone_array + ( bone * bone_size ) );
}

engine::structs::transform_t engine::types::mesh_t::get_component_to_world( )
{
	return driver.read< engine::structs::transform_t >( address + engine::offsets::mesh::component_to_world );
}

mesh_t engine::types::pawn_t::get_mesh( )
{
	return driver.read< mesh_t >( address + offsets::pawn::player_controller.mesh );
}

player_controller_t engine::types::pawn_t::get_player_controller( )
{
	return driver.read< player_controller_t >( address + offsets::pawn::player_controller.self );
}

bool engine::types::mesh_t::is_recently_rendered( )
{
	if ( driver.read< std::uint8_t >( address + offsets::mesh::b_recently_rendered ) == 43 )
		return true;
	else 
		return false;
}

float engine::types::pawn_t::get_health( )
{
	return driver.read< float >( address + offsets::pawn::health.self );
}

float engine::types::pawn_t::get_max_health( )
{
	return driver.read< float >( address + offsets::pawn::health.max_health );
}

pawn_t engine::types::actor_t::get_pawn(  )
{
	return driver.read< pawn_t >( address + engine::offsets::actor::pawn );
}

mesh_t engine::types::actor_t::get_root_component( )
{
	return driver.read< mesh_t >( address + engine::offsets::actor::root_component );
}

engine::structs::vector_t engine::types::camera_manager_t::get_position( )
{
	return driver.read< structs::vector_t >( address + engine::offsets::camera_manager::camera_cache_private + engine::offsets::camera_manager::pov );
}

engine::structs::vector_t engine::types::camera_manager_t::get_rotation( )
{
	return driver.read< structs::vector_t >( address + engine::offsets::camera_manager::camera_cache_private + engine::offsets::camera_manager::pov + engine::offsets::camera_manager::rotation );
}

float engine::types::camera_manager_t::get_fov( )
{
	return driver.read< float >( address + engine::offsets::camera_manager::camera_cache_private + engine::offsets::camera_manager::pov + engine::offsets::camera_manager::fov );
}

camera_manager_t engine::types::character_t::get_camera_manager( )
{
	return driver.read< camera_manager_t >( address + engine::offsets::character::camera_manager );
}

bool engine::types::player_controller_t::is_local_player( )
{
	return driver.read< bool >( address + engine::offsets::player_controller::is_local_player_controller );
}

character_t engine::types::player_controller_t::get_character( )
{
	return driver.read< character_t >( address + engine::offsets::player_controller::character );
}

camera_manager_t engine::types::player_controller_t::get_camera_manager( )
{
	return driver.read< camera_manager_t >( address + engine::offsets::character::camera_manager );
}

pawn_t engine::types::player_controller_t::get_pawn( )
{
	return driver.read< pawn_t >( address + engine::offsets::player_controller::acknowledged_pawn );
}

player_controller_t engine::types::player_t::get_player_controller( )
{
	return driver.read< player_controller_t >( address + engine::offsets::u_player::player_controller );
}

player_state_t engine::types::player_controller_t::get_player_state( )
{
	return driver.read< player_state_t >( address + engine::offsets::u_player::controller_player_state );
}

player_state_t engine::types::pawn_t::get_player_state( )
{
	return driver.read< player_state_t >( address + engine::offsets::pawn::player_controller.pawn_player_state );
}

std::string get_name(std::uintptr_t object)
{
	std::string name;

	if (!object)
		return name;

	const auto index = driver.read< std::uint32_t >(object + 0x18);

	const auto block = driver.read< std::uintptr_t >(engine::values::fnames.address + (index >> 16) * sizeof(std::uintptr_t));

	const auto string_len = driver.read< std::uint16_t >(block + LOWORD(index) * 0x2) >> 6;

	auto string_start = block + LOWORD(index) * 0x2 + 0x2;

	for (auto i = string_start; i < string_start + string_len; ++i)
		name.push_back(driver.read< char >(i));

	return name;
}

ugame_instance_t engine::types::gworld_t::get_owning_game_instance( )
{
	return driver.read< ugame_instance_t >( address + offsets::core::g_world.owning_game_instance );
}

ulevel_t engine::types::gworld_t::get_ulevel( )
{
	return driver.read< ulevel_t >( address + offsets::core::g_world.u_level );
}

void engine::types::ulevel_t::get_aimbot_players( engine::types::ugame_instance_t o_g_i )
{
	temporary_aim_actors.clear( );

	for ( auto actor : actors.get_members( address ) )
	{
		if ( actor.is_a< actor_types_t::dshk_shielded >( ) ) 
		{ 
			auto camera_manager = o_g_i.local_player.get_members( o_g_i.address ).front( ).get_player_controller( ).get_camera_manager( );

			if ( !camera_manager.address )
				continue;

			auto root_world_position = engine::functions::get_bone_position( actor, engine::structs::bones::root );

			if ( !root_world_position.x || !root_world_position.y )
				continue;

			const auto distance = static_cast< std::uint32_t >( camera_manager.get_position( ).distance( root_world_position ) / 100.f );

			if ( distance < std::get< int >( menu::config::config_map[ "aimbot_max_distance" ] ) )
			   temporary_aim_actors.push_back( actor );
		}
	}

	std::unique_lock locked_table( engine::functions::aim_actor_lock );

	engine::functions::aim_actors = temporary_aim_actors;
}

void engine::types::ulevel_t::get_visual_players( engine::types::ugame_instance_t o_g_i )
{
	temporary_visual_actors.clear( );

	for ( auto actor : actors.get_members( address ) )
	{
		if ( actor.is_a< actor_types_t::dshk_shielded >( ) )
		{ 
			auto camera_manager = o_g_i.local_player.get_members( o_g_i.address ).front( ).get_player_controller( ).get_camera_manager( );

			if ( !camera_manager.address )
				continue;

			auto root_world_position = engine::functions::get_bone_position( actor, engine::structs::bones::root );

			if ( !root_world_position.x || !root_world_position.y )
				continue;

			const auto distance = static_cast< std::uint32_t >( camera_manager.get_position( ).distance( root_world_position ) / 100.f );

			if ( distance < std::get< int >( menu::config::config_map[ "visuals_max_distance" ] ) )
			   temporary_visual_actors.push_back( actor );
		}
	}

	std::unique_lock locked_table( engine::functions::visual_actor_lock );

	engine::functions::visual_actors = temporary_visual_actors;
}

engine::structs::vector_t engine::functions::get_bone_position( engine::types::actor_t player, engine::structs::bones bone )
{
	auto mesh = player.get_pawn( ).get_mesh( );

	if ( !mesh )
		return { 0.f, 0.f, 0.f };

	const auto bone_transform = mesh.get_bone( bone );

	if ( !bone_transform.has_value( ) )
		return { 0.f, 0.f, 0.f };

	const auto matrix = matrix_multiply( to_scaled_matrix( bone_transform.value( ) ) , to_scaled_matrix( mesh.get_component_to_world( ) ) );

	return { matrix.matrix4x4[0][3], matrix.matrix4x4[1][3], matrix.matrix4x4[2][3] };
}

engine::structs::vector_t engine::functions::get_bone_position_int( engine::types::actor_t player, int bone )
{
	auto mesh = player.get_pawn( ).get_mesh( );

	if ( !mesh )
		return { 0.f, 0.f, 0.f };

	const auto bone_transform = mesh.get_bone_int( bone );

	if ( !bone_transform.has_value( ) )
		return { 0.f, 0.f, 0.f };

	const auto matrix = matrix_multiply( to_scaled_matrix( bone_transform.value( ) ) , to_scaled_matrix( mesh.get_component_to_world( ) ) );

	return { matrix.matrix4x4[0][3], matrix.matrix4x4[1][3], matrix.matrix4x4[2][3] };
}

void engine::functions::initiate( )
{
	engine::values::game_handle = FindWindowA( xs( "UnrealWindow" ), nullptr );

	engine::values::process_base = driver.get_base( );

	std::printf( "base 0x%p\n", engine::values::process_base );

	engine::values::gworld = driver.read< gworld_t >( engine::values::process_base + offsets::core::g_world.self );
	engine::values::fnames = fnames_t{ engine::values::process_base + offsets::core::fname_array.self };
	engine::values::gobjects = driver.read< gobjects_t >( engine::values::process_base + offsets::core::g_objects.self );

	std::printf( "gworld 0x%p\n", engine::values::gworld );
	std::printf( "fnames 0x%p\n", engine::values::fnames );
	std::printf( "gobjects 0x%p\n", engine::values::gobjects );
}

engine::structs::matrix4x4_t engine::functions::to_scaled_matrix( const engine::structs::transform_t& transform )
{
	engine::structs::matrix4x4_t matrix;

	matrix.matrix4x4[0][3] = transform.translation.x;
	matrix.matrix4x4[1][3] = transform.translation.y;
	matrix.matrix4x4[2][3] = transform.translation.z;

	const auto x2 = transform.rotation.x + transform.rotation.x;
	const auto y2 = transform.rotation.y + transform.rotation.y;
	const auto z2 = transform.rotation.z + transform.rotation.z;

	const auto xx2 = transform.rotation.x * x2;
	const auto yy2 = transform.rotation.y * y2;
	const auto zz2 = transform.rotation.z * z2;

	matrix.matrix4x4[ 0 ][ 0 ] = ( 1.0f - ( yy2 + zz2 ) ) * transform.scale_3d.x;
	matrix.matrix4x4[ 1 ][ 1 ] = ( 1.0f - ( xx2 + zz2 ) ) * transform.scale_3d.y;
	matrix.matrix4x4[ 2 ][ 2 ] = ( 1.0f - ( xx2 + yy2 ) ) * transform.scale_3d.z;

	const auto yz2 = transform.rotation.y * z2;
	const auto wx2 = transform.rotation.w * x2;

	matrix.matrix4x4[ 1 ][ 2 ] = ( yz2 - wx2 ) * transform.scale_3d.z;
	matrix.matrix4x4[ 2 ][ 1 ] = ( yz2 + wx2 ) * transform.scale_3d.y;

	const auto xy2 = transform.rotation.x * y2;
	const auto wz2 = transform.rotation.w * z2;

	matrix.matrix4x4[ 0 ][ 1 ] = ( xy2 - wz2 ) * transform.scale_3d.y;
	matrix.matrix4x4[ 1 ][ 0 ] = ( xy2 + wz2 ) * transform.scale_3d.x;

	const auto xz2 = transform.rotation.x * z2;
	const auto wy2 = transform.rotation.w * y2;

	matrix.matrix4x4[ 0 ][ 2 ] = ( xz2 + wy2 ) * transform.scale_3d.z;
	matrix.matrix4x4[ 2 ][ 0 ] = ( xz2 - wy2 ) * transform.scale_3d.x;

	matrix.matrix4x4[ 3 ][ 0 ] = 0.0f;
	matrix.matrix4x4[ 3 ][ 1 ] = 0.0f;
	matrix.matrix4x4[ 3 ][ 2 ] = 0.0f;
	matrix.matrix4x4[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

engine::structs::matrix4x4_t engine::functions::matrix_multiply( const engine::structs::matrix4x4_t& lhs, const engine::structs::matrix4x4_t& rhs )
{
	engine::structs::matrix4x4_t matrix;

	const auto a00 = lhs.matrix4x4[ 0 ][ 0 ]; const auto a01 = lhs.matrix4x4[ 0 ][ 1 ]; const auto a02 = lhs.matrix4x4[ 0 ][ 2 ]; const auto a03 = lhs.matrix4x4[ 0 ][ 3 ];
	const auto a10 = lhs.matrix4x4[ 1 ][ 0 ]; const auto a11 = lhs.matrix4x4[ 1 ][ 1 ]; const auto a12 = lhs.matrix4x4[ 1 ][ 2 ]; const auto a13 = lhs.matrix4x4[ 1 ][ 3 ];
	const auto a20 = lhs.matrix4x4[ 2 ][ 0 ]; const auto a21 = lhs.matrix4x4[ 2 ][ 1 ]; const auto a22 = lhs.matrix4x4[ 2 ][ 2 ]; const auto a23 = lhs.matrix4x4[ 2 ][ 3 ];
	const auto a30 = lhs.matrix4x4[ 3 ][ 0 ]; const auto a31 = lhs.matrix4x4[ 3 ][ 1 ]; const auto a32 = lhs.matrix4x4[ 3 ][ 2 ]; const auto a33 = lhs.matrix4x4[ 3 ][ 3 ];

	const auto b00 = rhs.matrix4x4[ 0 ][ 0 ]; const auto b01 = rhs.matrix4x4[ 0 ][ 1 ]; const auto b02 = rhs.matrix4x4[ 0 ][ 2 ]; const auto b03 = rhs.matrix4x4[ 0 ][ 3 ];
	const auto b10 = rhs.matrix4x4[ 1 ][ 0 ]; const auto b11 = rhs.matrix4x4[ 1 ][ 1 ]; const auto b12 = rhs.matrix4x4[ 1 ][ 2 ]; const auto b13 = rhs.matrix4x4[ 1 ][ 3 ];
	const auto b20 = rhs.matrix4x4[ 2 ][ 0 ]; const auto b21 = rhs.matrix4x4[ 2 ][ 1 ]; const auto b22 = rhs.matrix4x4[ 2 ][ 2 ]; const auto b23 = rhs.matrix4x4[ 2 ][ 3 ];
	const auto b30 = rhs.matrix4x4[ 3 ][ 0 ]; const auto b31 = rhs.matrix4x4[ 3 ][ 1 ]; const auto b32 = rhs.matrix4x4[ 3 ][ 2 ]; const auto b33 = rhs.matrix4x4[ 3 ][ 3 ];

	matrix.matrix4x4[ 0 ][ 0 ] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
	matrix.matrix4x4[ 0 ][ 1 ] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
	matrix.matrix4x4[ 0 ][ 2 ] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
	matrix.matrix4x4[ 0 ][ 3 ] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
 
	matrix.matrix4x4[ 1 ][ 0 ] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
	matrix.matrix4x4[ 1 ][ 1 ] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
	matrix.matrix4x4[ 1 ][ 2 ] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
	matrix.matrix4x4[ 1 ][ 3 ] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;

	matrix.matrix4x4[ 2 ][ 0 ] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
	matrix.matrix4x4[ 2 ][ 1 ] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
	matrix.matrix4x4[ 2 ][ 2 ] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
	matrix.matrix4x4[ 2 ][ 3 ] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;

	matrix.matrix4x4[ 3 ][ 0 ] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
	matrix.matrix4x4[ 3 ][ 1 ] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
	matrix.matrix4x4[ 3 ][ 2 ] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
	matrix.matrix4x4[ 3 ][ 3 ] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;

	return matrix;
}

engine::structs::rotator_t engine::functions::calculate_angle( engine::structs::vector_t local_player, engine::structs::vector_t target_player )
{
	engine::structs::rotator_t new_view_angle;

	const auto rotation = local_player - target_player;

	const auto hypotenuse = std::sqrt( rotation.x * rotation.x + rotation.y * rotation.y );

	new_view_angle.pitch = -std::atan( rotation.z / hypotenuse ) * ( 180.f / M_PI );
	new_view_angle.yaw = std::atan( rotation.y / rotation.x ) * ( 180.f / M_PI );
	new_view_angle.roll = 0.f;

	if ( rotation.x >= 0.f )
		new_view_angle.yaw += 180.0f;

	return new_view_angle.clamp( );
}

static HWND game;

std::pair< std::int32_t, std::int32_t > engine::functions::get_screen_size( )
{
	if ( !game )
		game = FindWindowA( xs( "UnrealWindow" ), 0 );

	RECT screen_dimensions{ };

	if ( GetClientRect( game, &screen_dimensions ) )
	{
		return { screen_dimensions.right - screen_dimensions.left, screen_dimensions.bottom - screen_dimensions.top };
	}

	return { 0, 0 };
}

engine::structs::matrix3x4_t engine::functions::get_rotation_matrix( )
{
	structs::matrix3x4_t rotation_matrix;

	auto owning_game_instance = engine::values::gworld.get_owning_game_instance( );

	auto camera_rotation = owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_camera_manager( ).get_rotation( );

	const auto p = camera_rotation.x * M_PI / 180;
	const auto y = camera_rotation.y * M_PI / 180;
	const auto r = camera_rotation.z * M_PI / 180;

	const auto sp = std::sinf( p ), cp = std::cosf( p ), sy = std::sinf( y ), cy = std::cosf( y ), sr = std::sinf( r ), cr = std::cosf( r );

	rotation_matrix.matrix3x4[ 0 ][ 0 ] = cp * cy; rotation_matrix.matrix3x4[ 0 ][ 1 ] = cp * sy; rotation_matrix.matrix3x4[ 0 ][ 2 ] = sp; rotation_matrix.matrix3x4[ 0 ][ 3 ] = 0.f;
	rotation_matrix.matrix3x4[ 1 ][ 0 ] = sr * sp * cy - cr * sy; rotation_matrix.matrix3x4[ 1 ][ 1 ] = sr * sp * sy + cr * cy; rotation_matrix.matrix3x4[ 1 ][ 2 ] = -sr * cp; rotation_matrix.matrix3x4[ 1 ][ 3 ] = 0.f;
	rotation_matrix.matrix3x4[ 2 ][ 0 ] = -(cr * sp * cy + sr * sy); rotation_matrix.matrix3x4[ 2 ][ 1 ] = cy * sr - cr * sp * sy; rotation_matrix.matrix3x4[ 2 ] [2 ] = cr * cp; rotation_matrix.matrix3x4[ 2 ][ 3 ] = 0.f;

	return rotation_matrix;
}

bool engine::functions::world_to_screen( structs::vector_t& in, structs::vector_t& out )
{
	const auto rotation_matrix = get_rotation_matrix( );

	auto owning_game_instance = engine::values::gworld.get_owning_game_instance( );

	auto camera_manager = owning_game_instance.local_player.get_members( owning_game_instance.address ).front( ).get_player_controller( ).get_camera_manager( );

	const auto position = camera_manager.get_position( );

	const auto camera_fov = camera_manager.get_fov( );

	structs::vector_t x{ rotation_matrix.matrix3x4[ 0 ][ 0 ], rotation_matrix.matrix3x4[ 0 ][ 1 ], rotation_matrix.matrix3x4[ 0 ][ 2 ] };
	structs::vector_t y{ rotation_matrix.matrix3x4[ 1 ][ 0 ], rotation_matrix.matrix3x4[ 1 ][ 1 ], rotation_matrix.matrix3x4[ 1 ][ 2 ] };
	structs::vector_t z{ rotation_matrix.matrix3x4[ 2 ][ 0 ], rotation_matrix.matrix3x4[ 2 ][ 1 ], rotation_matrix.matrix3x4[ 2 ][ 2 ] };

	const auto delta = in - position;

	structs::vector_t transformed{ delta.dot( y ), delta.dot( z ), delta.dot( x ) };

	transformed.z = transformed.z < 1.f ? 1.f : transformed.z;

	const auto [ width, height ] = get_screen_size( );

	const auto center_x = width / 2.f;
	const auto center_y = height / 2.f;

	const auto fov = std::tanf( camera_fov * M_PI / 360.f );

	out.x = center_x + transformed.x * center_x / fov / transformed.z;
	out.y = center_y - transformed.y * center_x / fov / transformed.z;

	if ( out.x > width || out.x < 0 || out.y > height || out.y < 0 )
		return false;

	return true;
}
