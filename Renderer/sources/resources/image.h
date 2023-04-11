//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Image {
	
public:
	~Image();
	Image();
	Image( const VkImageCreateInfo imageInfo, const VkImageViewCreateInfo viewInfo );

	void setUsage( const VkImageUsageFlags usage );
	void setAspectMask( const VkImageAspectFlags aspectMask );

	void calculateMipLevel();

	void cleanup();
	
	void create( const UInt2 size, const VkFormat format );
	void create( const UInt3 size, const VkFormat format );
	void create( const VkImage image, const VkFormat format );
	void create( const string filepath, const VkFormat format );
	void createHDR( const string filepath, const VkFormat format );

	void createImage();
	void createImageViews();
	void createSampler( VkSamplerCreateInfo samplerInfo = GetDefaultSamplerInfo() );
	void createDescriptorInfo();
	void allocateImageMemory();

	void copyTo( const VkCommandBuffer cmdBuffer, const VkImage dstImage );
	
	void clearColorImage( const VkCommandBuffer cmdBuffer, const VkClearColorValue clearColor = VEC4_BLACK );
	void generateMipmaps( VkCommandBuffer cmdBuffer );
	VkImageMemoryBarrier getMemoryBarrier();
	
	VkImageView    getImageView(u32 idx = 0);
	VkImage        getImage();
	VkSampler      getSampler();
	VkDeviceMemory getImageMemory();

	u32            getChannel();
	u32            getChannelSize();
	u32            getMipLevels();
	UInt3          getImageSize();
	VkDeviceSize   getDeviceSize();
	
	const VkImageLayout&         getImageLayout();
	const VkImageCreateInfo&     getImageInfo();
	const VkImageViewCreateInfo& getImageViewInfo();
	const vector<VkDescriptorImageInfo>& getDescriptorInfo();

	void setMipLevels( const u32 mipLevels );
	void setImageLayout( const VkImageLayout imageLayout );
	void setImageFormat( const VkFormat format );

	static VkImageCreateInfo     GetDefaultImageCreateInfo();
	static VkImageViewCreateInfo GetDefaultImageViewCreateInfo();
	static VkImageMemoryBarrier  GetDefaultImageMemoryBarrier();
	static VkSamplerCreateInfo   GetDefaultSamplerInfo();

private:
	Cleaner mCleaner;

	u32 mChannel;
	uc8* mRawData;
	float* mRawHDR;

	VkImage mImage = VK_NULL_HANDLE;
	VkDeviceMemory mImageMemory = VK_NULL_HANDLE;
	VkSampler mSampler = VK_NULL_HANDLE;
	vector<VkImageView> mImageViews;
	
	VkImageLayout		  mImageLayout;
	VkImageCreateInfo     mImageInfo{};
	VkImageViewCreateInfo mImageViewInfo{};
	VkBindImageMemoryInfo mBindImageInfo{};
	vector<VkDescriptorImageInfo> mDescriptorInfos;
		
};
