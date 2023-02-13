#pragma once

#include <windows.h>

#include <string>

#include "../dependencies/xorstring/xorstring.hpp"
#include "../dependencies/lazy_importer/lazy_importer.hpp"

class driver_t
{
	struct data_t
	{
		enum class command_t
		{
			get_base,
			copy
		}
		const command;

		const std::uint64_t from_process, to_process;

		const std::uintptr_t from_address, to_address;

		const std::size_t size;

		const std::uint16_t magic = 0xEAC;
	};

	using nt_d_composition_set_child_root_visual_t = NTSTATUS( * )( const data_t& data, void* );
	const nt_d_composition_set_child_root_visual_t NtDCompositionSetChildRootVisual = lf( NtDCompositionSetChildRootVisual ).in< nt_d_composition_set_child_root_visual_t >( lm( "win32u.dll" ).get( ) );

	DWORD to, from = lf( GetCurrentProcessId )( );

public:	
	template < class value_t = std::uintptr_t > value_t read( const std::uintptr_t address ) const
	{
		value_t value;

		const auto data = data_t{ data_t::command_t::copy, to, from, address, reinterpret_cast< std::uintptr_t >( &value ), sizeof( value_t ) };
		NtDCompositionSetChildRootVisual( data, nullptr );

		return value;
	}

	template < class value_t >
	void write( const std::uintptr_t address, const value_t& value ) const
	{
		const auto data = data_t{ data_t::command_t::copy, from, to, reinterpret_cast< std::uintptr_t >( &value ), address, sizeof( value ) };
		NtDCompositionSetChildRootVisual( data, nullptr );
	}

	std::uintptr_t get_base( ) const;

	std::string read_string( std::uintptr_t address, const bool inner = true ) const;

	bool initiate( const char* const window_name );
}
extern driver;