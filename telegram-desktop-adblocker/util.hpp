#pragma once
#include <vector>
#include <cstdint>
#include <fstream>
#include <filesystem>


namespace util {
	namespace io {
		__forceinline std::vector<std::uint8_t> read_file( const std::wstring& path ) {
			std::fstream f( path, std::ios::in | std::ios::binary );
			if ( !f )
				return {};

			f.seekg( 0, f.end );
			const auto f_size = f.tellg( );
			f.seekg( 0, f.beg );

			std::vector<uint8_t> buffer( f_size );
			f.read( reinterpret_cast< char* >( buffer.data( ) ), buffer.size( ) );

			return buffer;
		}

		__forceinline void write_file( const std::wstring& path, std::uint32_t raw_buffer, size_t buffer_size ) {
			std::ofstream f( path, std::ios::binary | std::ios::out );
			f.write( reinterpret_cast< char* >( raw_buffer ), buffer_size );
			f.close( );
		}
	}

	namespace mem {
		inline std::uintptr_t sig( uintptr_t module, std::vector<int> pattern, std::size_t sizeOfImage ) {
			std::uint8_t* scanBytes = reinterpret_cast< std::uint8_t* >( module );

			std::size_t pattern_size = pattern.size( );
			int* pattern_data = pattern.data( );

			for ( std::uint32_t i = 0ul; i < sizeOfImage - pattern_size; ++i ) {
				bool found = true;

				for ( std::uint32_t j = 0ul; j < pattern_size; ++j ) {
					if ( scanBytes[ i + j ] == pattern_data[ j ] || pattern_data[ j ] == -1 )
						continue;

					found = false;
					break;
				}

				if ( !found )
					continue;

				return i;
			}

			return 0;
		}
	}
}