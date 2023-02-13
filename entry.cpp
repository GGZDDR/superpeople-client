#include <windows.h>

#include <iostream>

#include "dependencies/xorstring/xorstring.hpp"

#include "settings/settings.hpp"
#include "engine/engine.hpp"

__forceinline void open_console( const char* const console_name )
{
	FILE* input_output_stream;

	AllocConsole( );

	freopen_s( &input_output_stream, "CONIN$", "r", stdin );
	freopen_s( &input_output_stream, "CONOUT$", "w", stdout );
	freopen_s( &input_output_stream, "CONOUT$", "w", stderr );

	SetConsoleTitleA( console_name );
}

int main( )
{
	if constexpr ( settings::values::debug )
			open_console( xs( "superpeople-client" ) );

	while ( !FindWindowA( xs( "UnrealWindow" ), nullptr ) )
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	driver.initiate( xs( "UnrealWindow" ) );

	return 0;
}

bool __stdcall DllMain( void*, const std::uint32_t injection_reason, void* )
{
	if ( injection_reason == DLL_PROCESS_ATTACH )
		std::thread{ main }.detach( );

	return true;
}