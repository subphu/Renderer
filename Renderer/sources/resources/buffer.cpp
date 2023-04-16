//  Copyright © 2021 Subph. All rights reserved.
//

#include "buffer.h"

#include <system.h>

Buffer::~Buffer() {}
Buffer::Buffer() : mBufferInfo(GetDefaultBufferCreateInfo()) {}

Buffer::Buffer( const VkBufferCreateInfo bufferInfo ) : mBufferInfo( bufferInfo ) {}

Buffer::Buffer( const VkDeviceSize size, const VkBufferUsageFlags usage ) {
	mBufferInfo.size = size;
	mBufferInfo.usage = usage;
}

void Buffer::cleanup() { mCleaner.flush("Buffer"); }

void Buffer::create() {
	createBuffer();
	allocateBufferMemory();
}

void Buffer::createBuffer() {
	LOG( "Buffer::createBuffer" );
	const VkDevice device = System::Device()->getDevice();
	VkResult result = vkCreateBuffer( device, &mBufferInfo, nullptr, &mBuffer );
	ASSERT_VKERROR( result, "failed to buffer!" );
	mCleaner.push( [=]() { vkDestroyBuffer( device, mBuffer, nullptr ); } );
}

void Buffer::allocateBufferMemory() {
	LOG("Buffer::allocateBufferMemory");
	Device* devicePtr = System::Device();
	const VkDevice device = devicePtr->getDevice();

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements( device, mBuffer, &memoryRequirements );
	u32 memoryTypeIndex = devicePtr->findMemoryTypeIndex( memoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
	
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;
	
	const VkResult result = vkAllocateMemory( device, &allocInfo, nullptr, &mBufferMemory );
	ASSERT_VKERROR( result, "failed to allocate buffer memory!" );
	mCleaner.push( [=]() { vkFreeMemory( device, mBufferMemory, nullptr ); } );
	vkBindBufferMemory( device, mBuffer, mBufferMemory, 0 );

	mDescriptorBufferInfo.buffer = mBuffer;
	mDescriptorBufferInfo.range = mBufferInfo.size;
	mDescriptorBufferInfo.offset = 0;
}

void Buffer::cmdClearBuffer( const VkCommandBuffer cmdBuffer, float fdata ) {
	u32 fbits = 0;
	memcpy( &fbits, &fdata, sizeof( fbits ) );
	vkCmdFillBuffer( cmdBuffer, mBuffer, 0, mBufferInfo.size, fbits );
}

void* Buffer::fillBuffer( const void* address, const VkDeviceSize size, const u32 offset ) {
	void* ptr = mapMemory(size);
	ptr = static_cast<c8*>(ptr) + offset;
	memcpy( ptr, address, size );
	unmapMemory();
	return ptr;
}

void* Buffer::fillBufferFull( const void* address ) {
	return fillBuffer(address, static_cast<size_t>(mBufferInfo.size));
}

void* Buffer::mapMemory( const VkDeviceSize size ) {
	void* ptr;
	vkMapMemory( System::Device()->getDevice(), mBufferMemory, 0, size, 0, &ptr );
	return ptr;
}

void Buffer::unmapMemory() {
	vkUnmapMemory( System::Device()->getDevice(), mBufferMemory );
}

VkBuffer		Buffer::get			   () { return mBuffer; }
VkDeviceMemory	Buffer::getBufferMemory() { return mBufferMemory; }
VkDeviceSize	Buffer::getBufferSize  () { return mBufferInfo.size; }

VkDescriptorBufferInfo* Buffer::getDescriptorInfo() { return &mDescriptorBufferInfo; }


// Static ==================================================


VkBufferCreateInfo Buffer::GetDefaultBufferCreateInfo() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	return bufferInfo;
}
