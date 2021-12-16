#include <Windows.h>
#include "util.hpp"


int main( int argc, char* argv[ ] ) {
	if ( argc < 2 ) {
		printf( "Usage: %s <c:\\path\\to\\Telegram.exe>\n", argv[ 0 ] );
		system( "pause" );
		return EXIT_FAILURE;
	}

	const auto f = std::filesystem::path( argv[ 1 ] );
	const auto buf = util::io::read_file( f.c_str( ) );
	if ( !buf.size( ) ) {
		printf( "got empty buffer\n" );
		system( "pause" );
		return EXIT_FAILURE;
	}

	uintptr_t f_addr = uintptr_t( buf.data( ) );
	printf( "got buffer at 0x%p\n", f_addr );

	auto f_backup_path = f.parent_path( );
	f_backup_path += L"\\Telegram_backup.exe";
	printf( "creating backup at %ws\n", f_backup_path.c_str( ) );
	util::io::write_file( f_backup_path.c_str( ), f_addr, buf.size( ) );

	auto fn = util::mem::sig( f_addr, { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x8B, 0x45, 0x08, 0x83, 0xEC, 0x18, 0x8B, 0x50, 0x04, 0x56, 0x57, 0x8B, 0x38 }, buf.size( ) );
	if ( !fn ) {
		printf( "sig not found\n" );
		system( "pause" );
		return EXIT_FAILURE;
	}
	printf( "fn: 0x%p [0x%p]\n", fn, fn + f_addr );

	printf( "successfully created backup, patching\n" );

	uint8_t patch[ ] = {
		0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, 0
		0xC2, 0x04, 0x00 // retn 4
	};
	memcpy( reinterpret_cast< void* >( f_addr + fn ), patch, sizeof( patch ) );
	printf( "successfully patched %d bytes\n", sizeof( patch ) );

	printf( "writing file...\n" );
	util::io::write_file( f.c_str( ), f_addr, buf.size( ) );

	printf( "finished\n" );

	system( "pause" );
	return EXIT_SUCCESS;
}
