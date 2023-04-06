#include "shader.h"

#include <system.h>
#include <file.h>
#include <format>

Shader::~Shader() {}
Shader::Shader() {}

Shader::Shader( const string filepath, const VkShaderStageFlagBits stage, const char* entryPoint ) {
	createModule( CompileShader( filepath ) );
	createStageInfo( stage, entryPoint );
}

void Shader::cleanup() { mCleaner.flush("Shader"); }

void Shader::createModule( const string filepath ) {
	LOG("Shader::createModule");
	VkDevice device = System::Device()->getDevice();
	mFilepath = filepath + System::Settings()->spirvExt;
	auto code = File::ReadBinaryFile(filepath);
	
	VkShaderModuleCreateInfo shaderInfo{};
	shaderInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.codeSize = code.size();
	shaderInfo.pCode    = reinterpret_cast<const u32*>(code.data());
	
	VkResult result = vkCreateShaderModule(device, &shaderInfo, nullptr, &mShaderModule );
	ASSERT_VKERROR( result, "failed to create shader modul!" );
	mCleaner.push([=](){ vkDestroyShaderModule(device, mShaderModule, nullptr); });
}

void Shader::createStageInfo( const VkShaderStageFlagBits stage, const char* entryPoint ) {
	mShaderStageInfo = {};
	mShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mShaderStageInfo.stage  = stage;
	mShaderStageInfo.pName  = entryPoint;
	mShaderStageInfo.module = mShaderModule;
}

const VkPipelineShaderStageCreateInfo& Shader::getShaderStageInfo() { return mShaderStageInfo; }

const string Shader::CompileShader( const string filepath ) {
	int retCode = system( std::format( "{} {} -o {}",
		File::GetGlslcPath(), File::GetShaderPath( filepath ), File::GetSpirvPath( filepath ) ).c_str() );

	return string();
}
