#pragma once

#include <common.h>

class Shader {
	
public:
	~Shader();
	Shader();
	Shader( const string filepath, const VkShaderStageFlagBits stage, const char* entryPoint = "main" );
	
	void cleanup();

	void createModule( const string filepath );
	void createStageInfo( const VkShaderStageFlagBits stage, const char* entryPoint = "main" );
	
	const string getFilepath();
	const VkPipelineShaderStageCreateInfo& getShaderStageInfo();

	static const string CompileShader( const string filepath );
	
private:
	Cleaner mCleaner{};
	
	string mFilepath;

	VkShaderModule mShaderModule = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo mShaderStageInfo{};
};
