#pragma once

#include <common.h>

class Descriptor {
	
public:
	enum SetIndex {
		GLOBAL_SET = 0,
		RENDERPASS_SET = 1,
		SUBPASS_SET = 2,
		PIPELINE_SET = 3,
		TOTAL_SET
	};

	Descriptor();
	~Descriptor();
	
	void cleanup();
	
	void createPool();
	void allocateDescriptorSets();
	u32 createSetLayout( const vector<VkDescriptorSetLayoutBinding>& layoutBindings );

	const VkDescriptorSet* getSetPtr( const u32 idx );
	const VkDescriptorSetLayout* getSetlayoutPtr( const u32 idx );
	
private:

	Cleaner mCleaner;

	VkDescriptorPoolCreateInfo mPoolInfo{};
	VkDescriptorPool mPool = VK_NULL_HANDLE;

	vector<VkDescriptorSetLayout> mSetLayouts;
	vector<VkDescriptorSet> mSets;

	const vector<VkDescriptorPoolSize> PoolSizes = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER				, 0xFF },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER	, 0xFF },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE			, 0xFF },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE			, 0xFF },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER	, 0xFF },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER	, 0xFF },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER			, 0xFF },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER			, 0xFF },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC	, 0xFF },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC	, 0xFF },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT		, 0xFF }
	};
};
