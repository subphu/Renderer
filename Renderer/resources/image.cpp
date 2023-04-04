//  Copyright © 2021 Subph. All rights reserved.
//

#include "image.h"

#include <system.h>

namespace {

	unsigned int GetChannelSize( VkFormat format ) {
		switch (format) {
		case VK_FORMAT_R8G8B8_SRGB: return 3; break;
		case VK_FORMAT_R8G8B8A8_SRGB: return 4; break;
		case VK_FORMAT_R32G32B32_SFLOAT: return 12; break;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return 16; break;
		default: return 0; break;
		}
	}

	const u32 CalculateMaxMipLevel( const UInt3 size ) {
		return U32( fmin( UINT32( std::floor( std::log2( max( size.width, size.height ) ) ) ) + 1, 7 ) );
	}

	void GenerateMipMap( const VkCommandBuffer cmdBuffer, VkImageMemoryBarrier barrier, const UInt3 size, const u32 mipLevels ) {
		VkOffset3D maxOffset = { S32(size.width), S32(size.height), S32(size.depth) };
		for (u32 i = 0; i < mipLevels - 1; i++) {
			VkOffset3D halfMaxOffset = { S32( ceil( maxOffset.x / 2.0f ) ),
										 S32( ceil( maxOffset.y / 2.0f ) ),
										 S32( ceil( maxOffset.z / 2.0f ) ) };

			barrier.subresourceRange.baseMipLevel = i;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			vkCmdPipelineBarrier(cmdBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								 0, nullptr,
								 0, nullptr,
								 1, &barrier);
		
			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = maxOffset;
			blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel       = i;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount     = barrier.subresourceRange.layerCount;
		
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = halfMaxOffset;
			blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel       = i + 1;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount     = barrier.subresourceRange.layerCount;
			vkCmdBlitImage(cmdBuffer,
						   barrier.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						   barrier.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						   1, &blit,
						   VK_FILTER_LINEAR);
		
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			vkCmdPipelineBarrier(cmdBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								 0, nullptr,
								 0, nullptr,
								 1, &barrier);

			maxOffset = halfMaxOffset;
		}
	}
}

Image::~Image() {}
Image::Image() : mImageInfo(GetDefaultImageCreateInfo()),
				 mImageViewInfo(GetDefaultImageViewCreateInfo()) { }

Image::Image( const VkImageCreateInfo imageInfo, const VkImageViewCreateInfo viewInfo ) : 
	mImageInfo( imageInfo ),
	mImageViewInfo( viewInfo ) {}


void Image::setUsage( const VkImageUsageFlags usage ) {
	mImageInfo.usage = usage;
}

void Image::setAspectMask( const VkImageAspectFlags aspectMask ) {
	mImageViewInfo.subresourceRange.aspectMask = aspectMask;
}

void Image::calculateMipLevel() {
	mImageInfo.mipLevels = CalculateMaxMipLevel( mImageInfo.extent );
	mImageViewInfo.subresourceRange.levelCount = mImageInfo.mipLevels;
}

void Image::cleanup() { mCleaner.flush("Image"); }

void Image::create( const UInt2 size, const VkFormat format ) {
	UInt3 size3 = { size.height, size.width, 1 };
	create( size3, format );
}

void Image::create( const UInt3 size, const VkFormat format ) {
	mImageInfo.extent = size;
	mImageInfo.format = format;
	mImageViewInfo.format = format;
	createImage();
	createImageViews();
}

void Image::create( const VkImage image, const VkFormat format ) {
	mImage = image;
	mImageInfo.format = format;
	mImageViewInfo.format = format;
	createImageViews();
}

void Image::create( const string filepath, const VkFormat format ) {
	//int width, height, channels;
	//mRawData = STBI::LoadImage( filepath, &width, &height, &channels );
	//mChannel = channels;
	//UInt3 size = { U32( width ), U32( height ), 1 };
	//create( size, format );
}

void Image::createHDR( const string filepath, const VkFormat format ) {
	//int width, height, channels;
	//mRawHDR = STBI::LoadHDR( filepath, &width, &height, &channels );
	//mChannel = channels;
	//UInt3 size = { U32( width ), U32( height ), 1 };
	//create( size, format );
}

void Image::createImage() {
	LOG("Image::createImage");
	const VkDevice device = System::Device()->getDevice();
	const VkResult result = vkCreateImage(device, &mImageInfo, nullptr, &mImage);
	ASSERT_VKERROR(result, "failed to create image!");
	mCleaner.push([=](){ vkDestroyImage(device, mImage, nullptr); });
	mImageLayout = mImageInfo.initialLayout;
}

void Image::createImageViews() {
	LOG("Image::createImageViews");
	const VkDevice device = System::Device()->getDevice();
	const u32 mipLevels = mImageInfo.mipLevels;
	mImageViews.resize(mipLevels);
	mImageViewInfo.image = mImage;

	for (u32 i = 0; i < mipLevels; i++) {
		mImageViewInfo.subresourceRange.baseMipLevel = i;
		mImageViewInfo.subresourceRange.levelCount = mipLevels - i;
		const VkResult result = vkCreateImageView(device, &mImageViewInfo, nullptr, &mImageViews[i]);
		ASSERT_VKERROR(result, "failed to create image views!");
		mCleaner.push([=](){ vkDestroyImageView(device, mImageViews.back(), nullptr); mImageViews.pop_back(); });
	}

	mImageViewInfo.subresourceRange.baseMipLevel = 0;
	mImageViewInfo.subresourceRange.levelCount = mipLevels;
}

void Image::createSampler( VkSamplerCreateInfo samplerInfo ) {
	LOG("Image::createSampler");
	samplerInfo.maxLod = F32( mImageInfo.mipLevels );
	const VkDevice device = System::Device()->getDevice();
	const VkResult result = vkCreateSampler(device, &samplerInfo, nullptr, &mSampler);
	ASSERT_VKERROR(result, "failed to create texture sampler!");
	mCleaner.push([=](){ vkDestroySampler(device, mSampler, nullptr); });
}

void Image::createDescriptorInfo() {
	mDescriptorInfos.resize( mImageInfo.mipLevels );
	for (u32 i = 0; i < mImageInfo.mipLevels; i++) {
		mDescriptorInfos[i].imageLayout = mImageLayout;
		mDescriptorInfos[i].imageView = mImageViews[i];
		mDescriptorInfos[i].sampler = mSampler;
	}
}

void Image::allocateImageMemory() {
	LOG( "Image::allocateImageMemory" );
	Device* devicePtr = System::Device();
	const VkDevice device = devicePtr->getDevice();

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements( device, mImage, &memoryRequirements );
	u32 memoryTypeIndex = devicePtr->findMemoryTypeIndex( memoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	const VkResult result = vkAllocateMemory( device, &allocInfo, nullptr, &mImageMemory );
	ASSERT_VKERROR( result, "failed to allocate image memory!" );
	mCleaner.push( [=]() { vkFreeMemory( device, mImageMemory, nullptr ); } );

	mBindImageInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
	mBindImageInfo.image = mImage;
	mBindImageInfo.memory = mImageMemory;
}

void Image::clearColorImage( const VkCommandBuffer cmdBuffer, const VkClearColorValue clearColor ) {
	vkCmdClearColorImage(cmdBuffer, mImage, mImageLayout, &clearColor, 1, &mImageViewInfo.subresourceRange);
}

void Image::generateMipmaps( VkCommandBuffer cmdBuffer ) {
	LOG("Image::cmdGenerateMipmaps");
	Device* devicePtr = System::Device();
	
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties = devicePtr->getFormatProperties( mImageInfo.format );
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		WAR( "texture image format does not support linear blitting!" );
		return;
	}
	
	VkImageMemoryBarrier barrier = GetDefaultImageMemoryBarrier();
	barrier.image = mImage;
	barrier.subresourceRange = mImageViewInfo.subresourceRange;
	GenerateMipMap( cmdBuffer, barrier, mImageInfo.extent, mImageInfo.mipLevels );
}

void Image::getMemoryBarrier( const VkImageLayout newLayout, const VkAccessFlags dstAccess ) {
	VkImageMemoryBarrier barrier = GetDefaultImageMemoryBarrier();
	barrier.image = mImage;
	barrier.subresourceRange = mImageViewInfo.subresourceRange;
	barrier.oldLayout = mImageLayout;
	barrier.newLayout = newLayout;
	barrier.dstAccessMask = dstAccess;
}


VkImageView    Image::getImageView  ( u32 idx ) { return mImageViews[idx]; }
VkImage        Image::getImage      () { return mImage;       }
VkDeviceMemory Image::getImageMemory() { return mImageMemory; }
VkSampler      Image::getSampler    () { return mSampler;     }
u32            Image::getChannel    () { return mChannel;  }
u32            Image::getChannelSize() { return GetChannelSize( mImageInfo.format ); }
u32            Image::getMipLevels  () { return mImageInfo.mipLevels; }
UInt3          Image::getImageSize  () { return mImageInfo.extent; }
VkDeviceSize   Image::getDeviceSize () { return mImageInfo.extent.width * mImageInfo.extent.height * mImageInfo.extent.depth * mImageInfo.arrayLayers * getChannelSize(); }

const VkImageLayout&         Image::getImageLayout()   { return mImageLayout; }
const VkImageCreateInfo&     Image::getImageInfo()     { return mImageInfo; }
const VkImageViewCreateInfo& Image::getImageViewInfo() { return mImageViewInfo; }
const vector<VkDescriptorImageInfo>& Image::getDescriptorInfo() { return mDescriptorInfos; }

void Image::setMipLevels( const u32 mipLevels ) { mImageInfo.mipLevels = mipLevels; }
void Image::setImageLayout( const VkImageLayout imageLayout ) { mImageLayout = imageLayout; }
void Image::setImageFormat( const VkFormat format ) { mImageInfo.format = format; mImageViewInfo.format = format; }


// Static ==================================================

VkImageCreateInfo Image::GetDefaultImageCreateInfo() {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageInfo.mipLevels		= 1;
	imageInfo.arrayLayers	= 1;
	imageInfo.extent.depth	= 1;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.samples		= VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling		= VK_IMAGE_TILING_OPTIMAL;
	return imageInfo;
}

VkImageViewCreateInfo Image::GetDefaultImageViewCreateInfo() {
	VkImageViewCreateInfo imageViewInfo{};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = 1;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;
	return imageViewInfo;
}

VkImageMemoryBarrier Image::GetDefaultImageMemoryBarrier() {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	return barrier;
}

VkSamplerCreateInfo Image::GetDefaultSamplerInfo() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter    = VK_FILTER_LINEAR;
	samplerInfo.minFilter    = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy    = 16.0f;
	samplerInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.compareEnable    = VK_FALSE;
	samplerInfo.compareOp        = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias       = 0.0f;
	samplerInfo.minLod           = 0.0f;
	return samplerInfo;
}