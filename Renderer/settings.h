#pragma once

#include <common.h>

struct Settings {
	const char* appName = "Renderer";
	UInt2 windowSize = { 800, 600 };

	VkApplicationInfo appInfo{};
	VkPhysicalDeviceFeatures deviceFeatures{};

	vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME };
	vector<const char*> instanceExtensions = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };

	VkPresentModeKHR defaultPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	VkSurfaceFormatKHR defaultSurfaceFormat = { VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	Settings() {
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName	= "Application";
		appInfo.applicationVersion	= VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.pEngineName			= "Engine";
		appInfo.engineVersion		= VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.apiVersion			= VK_API_VERSION_1_3;

		deviceFeatures.samplerAnisotropy	= VK_TRUE;
		deviceFeatures.multiViewport		= VK_TRUE;
		deviceFeatures.geometryShader		= VK_TRUE;
		deviceFeatures.tessellationShader	= VK_TRUE;
		deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
		deviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	}
};
