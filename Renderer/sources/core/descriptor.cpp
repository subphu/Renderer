//  Copyright © 2021 Subph. All rights reserved.
//

#include "descriptor.h"

#include <system.h>

Descriptor::~Descriptor() {}
Descriptor::Descriptor() {}

void Descriptor::cleanup() { 
	mCleaner.flush("Descriptor");
	mSetLayouts.clear();
	mSets.clear();
}

void Descriptor::createPool() {
	LOG( "Descriptor::createPool" );
	VkDevice device = System::Device()->getDevice();

	mPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	mPoolInfo.maxSets = 0xFFF;
	mPoolInfo.poolSizeCount = U32( PoolSizes.size() );
	mPoolInfo.pPoolSizes = PoolSizes.data();

	VkResult result = vkCreateDescriptorPool( device, &mPoolInfo, nullptr, &mPool );
	ASSERT_VKERROR( result, "failed to create descriptor pool!" );
	mCleaner.push( [=]() { vkDestroyDescriptorPool( device, mPool, nullptr ); } );
}

u32 Descriptor::createSetLayout( const vector<VkDescriptorSetLayoutBinding>& layoutBindings ) {
	LOG( "Descriptor::createLayout" );
	VkDevice device = System::Device()->getDevice();

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = U32( layoutBindings.size() );
	layoutInfo.pBindings = layoutBindings.data();

	VkDescriptorSetLayout layout;
	VkResult result = vkCreateDescriptorSetLayout( device, &layoutInfo, nullptr, &layout );
	ASSERT_VKERROR( result, "failed to create descriptor set layout!" );
	mCleaner.push( [=]() { vkDestroyDescriptorSetLayout( device, layout, nullptr ); mSetLayouts.pop_back(); } );

	u32 idx = U32( mSetLayouts.size() );
	mSetLayouts.push_back( layout );

	return idx;
}

void Descriptor::allocateDescriptorSets() {
	LOG( "Descriptor::allocateDescriptorSet" );
	VkDevice device = System::Device()->getDevice();

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = mPool;
	allocInfo.descriptorSetCount = u32( mSetLayouts.size() );
	allocInfo.pSetLayouts = mSetLayouts.data();
	
	mSets.resize( allocInfo.descriptorSetCount );
	VkResult result = vkAllocateDescriptorSets( device, &allocInfo, mSets.data() );
	ASSERT_VKERROR( result, "failed to allocate descriptor set!" );
}

const VkDescriptorSet* Descriptor::getSetPtr( const u32 idx ) { return &mSets[idx]; }
const VkDescriptorSetLayout* Descriptor::getSetlayoutPtr( const u32 idx ) { return &mSetLayouts[idx]; }
