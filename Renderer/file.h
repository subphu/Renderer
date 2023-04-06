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
		return std::filesystem::current_path().parent_path().append( System::Settings()->glslcWinDir ).string();
#elif __unix__
		return std::filesystem::current_path().parent_path().append( System::Settings()->glslcMacDir ).string();
#endif
	}

	static const string GetSpirvPath( const string filename ) {
		return std::filesystem::current_path().parent_path().append( System::Settings()->spirvDir ).append( filename + System::Settings()->spirvExt ).string();
	}

	static const string GetShaderPath( const string filename ) {
		return std::filesystem::current_path().parent_path().append( System::Settings()->shaderDir ).append( filename ).string();
	}
};

