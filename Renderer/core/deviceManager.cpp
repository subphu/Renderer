#include "deviceManager.h"

#include <set>
#include <system.h>

namespace {
	const vector<VkPhysicalDevice> GetPhysicalDevices( const VkInstance instance ) {
		u32 count;
		vkEnumeratePhysicalDevices( instance, &count, nullptr );
		vector<VkPhysicalDevice> devices( count );
		vkEnumeratePhysicalDevices( instance, &count, devices.data() );
		return devices;
	}

	const bool CheckLayerSupport( const vector<const char*>& layers ) {
		u32 count;
		vkEnumerateInstanceLayerProperties( &count, nullptr );
		vector<VkLayerProperties> availableLayers( count );
		vkEnumerateInstanceLayerProperties( &count, availableLayers.data() );

		PRINTLN( "Available instance layers:" );
		std::set<string> requiredLayers( layers.begin(), layers.end() );
		for (const auto& layerProperties : availableLayers) {
			size_t erased = requiredLayers.erase( layerProperties.layerName );
			PRINTLN3( "- ", layerProperties.layerName, (erased > 0 ? ": used" : "") );
		}
		return requiredLayers.empty();
	}

	const VkResult CreateDebugUtilsMessengerEXT( const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger ) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
		if (func != nullptr) {
			return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT( const VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator ) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
		if (func != nullptr) {
			func( instance, debugMessenger, pAllocator );
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData ) {
		std::cerr << "Validation layer: \n" << pCallbackData->pMessage << std::endl << std::endl;
		return VK_FALSE;
	}

}

DeviceManager::~DeviceManager() {}
DeviceManager::DeviceManager() {}

void DeviceManager::cleanup() { mCleaner.flush( "DeviceManager" ); }

void DeviceManager::createInstance() {
	LOG( "DeviceManager::createInstance" );
	const VkApplicationInfo& appInfo = System::Settings()->appInfo;
	const vector<const char*>& validationLayers = System::Settings()->validationLayers;
	const vector<const char*>& instanceExtensions = System::Settings()->instanceExtensions;

	bool isSupported = CheckLayerSupport( System::Settings()->validationLayers );
	ASSERT_FALSE( isSupported, "validation layers requested, but not available!" );

	VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugInfo.pfnUserCallback = DebugCallback;

	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo		 = &appInfo;
	instanceInfo.enabledExtensionCount	 = U32( instanceExtensions.size() );
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
	instanceInfo.enabledLayerCount		 = U32( validationLayers.size() );
	instanceInfo.ppEnabledLayerNames	 = validationLayers.data();
	instanceInfo.pNext					 = &debugInfo;

	VkResult result = vkCreateInstance( &instanceInfo, nullptr, &mInstance );
	ASSERT_VKERROR( result, "failed to create vulkan instance!" );
	mCleaner.push( [=]() { vkDestroyInstance( mInstance, nullptr ); } );

	result = CreateDebugUtilsMessengerEXT( mInstance, &debugInfo, nullptr, &mDebugMessenger );
	ASSERT_VKERROR( result, "failed to set up debug messenger!" );
	mCleaner.push( [=]() { DestroyDebugUtilsMessengerEXT( mInstance, mDebugMessenger, nullptr ); } );
}

void DeviceManager::createDevices( const VkSurfaceKHR surface ) {
	LOG( "DeviceManager::createDevices" );
	VkInstance instance = mInstance;
	vector<VkPhysicalDevice> physicalDevices  = GetPhysicalDevices( instance );

	ASSERT_ZERO( physicalDevices.size(), "failed to find GPUs with Vulkan support!" );

	for (u32 i = 0; i < physicalDevices.size(); i++) {
		Device* device = new Device( physicalDevices[i] );
		if (device->isSupported( surface )) {
			device->createDevice();
			mUsingDevice = mUsingDevice < i ? mUsingDevice : i;
		}
		mDevices.push_back( device );
		mCleaner.push( [=]() { device->cleanup(); } );
	}
}

VkInstance	DeviceManager::getInstance() { return mInstance; }
Device*		DeviceManager::getDevice() { return mDevices[mUsingDevice]; }
