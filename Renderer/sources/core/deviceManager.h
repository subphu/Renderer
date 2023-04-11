#pragma once

#include <common.h>
#include <core/device.h>

class DeviceManager {

public:

	~DeviceManager();
	DeviceManager();

	void cleanup();

	void createInstance();
	void createDevices( const VkSurfaceKHR surface );

	VkInstance	getInstance();
	Device*		getDevice();
	
private:

	Cleaner mCleaner{};

	VkInstance mInstance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

	vector<Device*> mDevices;
	u32 mUsingDevice = 0;
};

