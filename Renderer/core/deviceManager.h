#pragma once

#include <common.h>
#include <core/device.h>
#include <window/sdlwindow.h>

class DeviceManager {

public:

	~DeviceManager();
	DeviceManager();

	void cleanup();

	void createInstance();
	void createDevices( VkSurfaceKHR surface );

	VkInstance	getInstance();
	Device*		getDevice();
	
private:

	Cleaner mCleaner{};

	VkInstance mInstance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

	vector<Device*> mDevices;
	u32 mUsingDevice = 0;
};

