//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Buffer {
	
public:
	~Buffer();
	Buffer();

	Buffer( const VkBufferCreateInfo bufferInfo );
	Buffer( const VkDeviceSize size, const VkBufferUsageFlags usage );

	void cleanup();
	
	void setup (VkDeviceSize size, VkBufferUsageFlags usage);
	void create();
	
	void createBuffer();
	void allocateBufferMemory();

	void cmdClearBuffer( const VkCommandBuffer cmdBuffer, float fdata );
	
	void* fillBuffer	( const void* address, const VkDeviceSize size, const u32 offset = 0);
	void* fillBufferFull( const void* address );
	
	void* mapMemory	 ( const VkDeviceSize size );
	void  unmapMemory();
	
	VkBuffer	   get();
	VkDeviceSize   getBufferSize();
	VkDeviceMemory getBufferMemory();
	VkDescriptorBufferInfo* getDescriptorInfo();

	static VkBufferCreateInfo GetDefaultBufferCreateInfo();

private:
	Cleaner mCleaner;

	VkBufferCreateInfo mBufferInfo{};
	unsigned char* mDesc;
	
	VkBuffer         mBuffer         = VK_NULL_HANDLE;
	VkDeviceMemory   mBufferMemory   = VK_NULL_HANDLE;
	VkDescriptorBufferInfo mDescriptorBufferInfo{};
	
};
