//  Copyright © 2021 Subph. All rights reserved.
//

#include "device.h"

#include <system.h>

namespace {
	const vector<VkSurfaceFormatKHR> GetSurfaceFormatKHR( const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface ) {
		u32 count;
		vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &count, nullptr );
		vector<VkSurfaceFormatKHR> formats( count );
		vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &count, formats.data() );
		return formats;
	}

	const vector<VkPresentModeKHR> GetPresentModeKHR( const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface ) {
		u32 count;
		vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, surface, &count, nullptr );
		vector<VkPresentModeKHR> modes( count );
		vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, surface, &count, modes.data() );
		return modes;
	}

	const vector<VkQueueFamilyProperties> GetQueueFamilyProperties( const VkPhysicalDevice physicalDevice ) {
		u32 count;
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &count, nullptr );
		vector<VkQueueFamilyProperties> properties( count );
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &count, properties.data() );
		return properties;
	}

	const s32 FindGraphicQueueIndex( const VkPhysicalDevice physicalDevice ) {
		vector<VkQueueFamilyProperties> queueFamilies = GetQueueFamilyProperties( physicalDevice );
		for (u32 i = 0; i < queueFamilies.size(); i++) {
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) return i;
		}
		return -1;
	}

	const s32 FindPresentQueueIndex( const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface ) {
		vector<VkQueueFamilyProperties> queueFamilies = GetQueueFamilyProperties( physicalDevice );
		for (u32 i = 0; i < queueFamilies.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, surface, &presentSupport );
			if (presentSupport) return i;
		}
		return -1;
	}

	const bool CheckFeatureSupport( const VkPhysicalDevice device, VkPhysicalDeviceFeatures features ) {
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures( device, &supportedFeatures );

		u32 size = sizeof( VkPhysicalDeviceFeatures ) / sizeof( VkBool32 );
		VkBool32* pBoolSupported = reinterpret_cast<VkBool32*>(&supportedFeatures);
		VkBool32* pBoolFeatures = reinterpret_cast<VkBool32*>(&features);

		VkBool32 supported = VK_TRUE;
		for (u32 i = 0; i < size; i++) {
			supported &= pBoolFeatures[i] ? pBoolSupported[i] : supported;
		}
		return supported;
	}

	const bool CheckDeviceExtensionSupport( const VkPhysicalDevice physicalDevice, const vector<const c8*>& extensions ) {
		u32 count;
		vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &count, nullptr );
		vector<VkExtensionProperties> availableExtensions( count );
		vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &count, availableExtensions.data() );

		std::set<string> requiredExtensions( extensions.begin(), extensions.end() );
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase( extension.extensionName );
		}
		return requiredExtensions.empty();
	}

	vector<VkDeviceQueueCreateInfo> CreateQueueInfos( const std::set<u32> queueIndices, const f32& queuePriority ) {
		vector<VkDeviceQueueCreateInfo> queueInfos;
		for (u32 queueIndex : queueIndices) {
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueCount = 1;
			queueInfo.queueFamilyIndex = queueIndex;
			queueInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(queueInfo);
		}
		return queueInfos;
	}
}

Device::~Device() {}
Device::Device( const VkPhysicalDevice physicalDevice ) : mPhysicalDevice(physicalDevice) { }

void Device::cleanup() { mCleaner.flush( "Device" ); }

bool Device::isSupported( const VkSurfaceKHR surface ) {
	LOG("Device::setup");
	const VkPhysicalDeviceFeatures& deviceFeatures = System::Settings()->deviceFeatures;
	const vector<const c8*>& deviceExtensions = System::Settings()->deviceExtensions;

	vkGetPhysicalDeviceProperties( mPhysicalDevice, &mProperties );
	vkGetPhysicalDeviceMemoryProperties( mPhysicalDevice, &mMemoryProperties );
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( mPhysicalDevice, surface, &mSurfaceCapabilities );

	mGraphicQueueIndex	= FindGraphicQueueIndex( mPhysicalDevice );
	mPresentQueueIndex	= FindPresentQueueIndex( mPhysicalDevice, surface );
	mSurfaceFormats		= GetSurfaceFormatKHR( mPhysicalDevice, surface );
	mPresentModes		= GetPresentModeKHR( mPhysicalDevice, surface );

	bool swapchainAdequate	= !mSurfaceFormats.empty() && !mPresentModes.empty();
	bool queueSupported		= mGraphicQueueIndex > -1 && mPresentQueueIndex > -1;
	bool featureSupported	= CheckFeatureSupport( mPhysicalDevice, deviceFeatures );
	bool extensionSupported	= CheckDeviceExtensionSupport( mPhysicalDevice, deviceExtensions );

	PRINTLN2( "- Device: ", mProperties.deviceName );
	PRINTLN( "Available surface formats" );
	for (const auto& format : mSurfaceFormats) PRINTLN2( format.format, format.colorSpace );
	PRINTLN( "Available present modes" );
	for (const auto& mode	: mPresentModes	 ) PRINTLN( mode );

	mSupported = swapchainAdequate && queueSupported && featureSupported && extensionSupported;
	return mSupported;
}

void Device::createDevice() {
	const VkPhysicalDeviceFeatures& deviceFeatures = System::Settings()->deviceFeatures;
	const vector<const c8*>& deviceExtensions = System::Settings()->deviceExtensions;
	const vector<const c8*>& validationLayers = System::Settings()->validationLayers;
	const f32 queuePriorities = 1.0f;
	const vector<VkDeviceQueueCreateInfo> queueInfos = CreateQueueInfos( { U32( mGraphicQueueIndex ), U32( mPresentQueueIndex ) }, queuePriorities );

	VkDeviceCreateInfo deviceInfo{};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pEnabledFeatures			= &deviceFeatures;
	deviceInfo.enabledExtensionCount	= U32( deviceExtensions.size() );
	deviceInfo.ppEnabledExtensionNames	= deviceExtensions.data();
	deviceInfo.enabledLayerCount		= U32( validationLayers.size() );
	deviceInfo.ppEnabledLayerNames		= validationLayers.data();
	deviceInfo.queueCreateInfoCount		= U32( queueInfos.size() );
	deviceInfo.pQueueCreateInfos		= queueInfos.data();

	const VkResult result = vkCreateDevice( mPhysicalDevice, &deviceInfo, nullptr, &mDevice );
	ASSERT_VKERROR( result, "failed to create logical device" );
	mCleaner.push( [=]() { vkDestroyDevice( mDevice, nullptr ); } );

	vkGetDeviceQueue( mDevice, mGraphicQueueIndex, 0, &mGraphicQueue );
	vkGetDeviceQueue( mDevice, mPresentQueueIndex, 0, &mPresentQueue );

	useSurfaceFormat( System::Settings()->defaultSurfaceFormat );
	usePresentMode( System::Settings()->defaultPresentMode );
}

void Device::usePresentMode( const VkPresentModeKHR mode ) {
	for (const auto& availableMode : mPresentModes) {
		if (availableMode == mode) {
			mPresentMode = availableMode;
			return;
		}
	}
	mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void Device::useSurfaceFormat( const VkSurfaceFormatKHR surfaceFormat ) {
	for (const auto& availableFormat : mSurfaceFormats) {
		if (availableFormat.format == surfaceFormat.format &&
			availableFormat.colorSpace == surfaceFormat.colorSpace) {
			mSurfaceFormat = availableFormat;
			return;
		}
	}
	mSurfaceFormat = mSurfaceFormats[0];
}

u32 Device::findMemoryTypeIndex( const u32 typeFilter, const VkMemoryPropertyFlags flags ) {
	for (u32 i = 0; i < mMemoryProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (mMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;
	}
	RUNTIME_ERROR( "failed to find suitable memory type!" );
}

VkFormatProperties Device::getFormatProperties( const VkFormat format ) {
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties( mPhysicalDevice, format, &formatProperties );
	return formatProperties;
}

void Device::waitDeviceIdle() { vkDeviceWaitIdle( mDevice ); }
void Device::waitQueueIdle() { vkQueueWaitIdle( mPresentQueue ); }

VkDevice		   Device::getDevice()		   { return mDevice; }
VkPhysicalDevice   Device::getPhysicalDevice() { return mPhysicalDevice; }

VkSurfaceCapabilitiesKHR Device::getSurfaceCapabilities() { return mSurfaceCapabilities; }
VkSurfaceFormatKHR Device::getSurfaceFormat() { return mSurfaceFormat; }
VkPresentModeKHR   Device::getPresentMode() { return mPresentMode; }

VkQueue	Device::getGraphicQueue()		{ return mGraphicQueue; }
VkQueue	Device::getPresentQueue()		{ return mPresentQueue; }
u32		Device::getGraphicQueueIndex()	{ return U32( mGraphicQueueIndex ); }
u32		Device::getPresentQueueIndex()	{ return U32( mPresentQueueIndex ); }

