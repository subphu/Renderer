#pragma once

#include <common.h>
#include <system.h>
#include <fstream>
#include <format>

class File {

public:
	static vector<char> ReadBinaryFile( const string filename ) {
		std::ifstream file( filename, std::ios::ate | std::ios::binary );
		if (!file.is_open()) throw std::runtime_error( "failed to open file!" );

		size_t fileSize = (size_t)file.tellg();
		vector<char> buffer( fileSize );
		file.seekg( 0 );
		file.read( buffer.data(), fileSize );
		file.close();
		return buffer;
	}

	static const string GetGlslcPath() {
#ifdef _WIN32
		char* pValue;
		size_t len;
		errno_t err = _dupenv_s( &pValue, &len, "VULKAN_SDK" );
		ASSERT_NULLPTR( pValue, "Environment variable VULKAN_SDK not found" );
		string path = std::filesystem::path( pValue ).append(System::Settings()->glslcWinDir).string();
#elif __unix__
		string path = std::filesystem::current_path().parent_path().append( System::Settings()->glslcMacDir ).string();
#endif
		LOG( path );
		return path;
	}

	static const string GetSpirvPath( const string filename ) {
		string path = std::filesystem::current_path().append(System::Settings()->spirvDir).append(filename + System::Settings()->spirvExt).string();
		LOG( path );
		return path;
	}

	static const string GetShaderPath( const string filename ) {
		string path = std::filesystem::path( GetShaderRootDir() ).append(filename).string();
		LOG( path );
		return path;
	}

	static const string GetShaderRootDir() {
		string path = std::filesystem::current_path().append( System::Settings()->glslDir ).string();
		LOG( path );
		return path;
	}

	static void CreateDir( const string dir ) {
		std::filesystem::path path = std::filesystem::current_path().append( dir );
		std::filesystem::create_directories( path );
	}
};

