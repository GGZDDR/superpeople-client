#include "driver.hpp"

#include <iostream>

driver_t driver;

std::uintptr_t driver_t::get_base( ) const
{
	static const auto data = data_t{ data_t::command_t::get_base, to };

	if ( !data.from_address )
		NtDCompositionSetChildRootVisual( data, nullptr );

	return data.from_address;
}

std::string driver_t::read_string( std::uintptr_t address, const bool inner ) const
{
	if ( inner )
		address = driver.read( address );

	std::string string;

	do
		string.push_back( driver.read< char >( address++ ) );
	while ( driver.read< char >( address ) != '\0' );

	return string;
}

bool driver_t::initiate( const char* const window_name )
{
	return GetWindowThreadProcessId( FindWindowA( window_name, nullptr ), &to );
}