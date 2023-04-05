#pragma once

#include "renderpass.h"

class RpPresent : public Renderpass {

public:
	~RpPresent();
	RpPresent();

	void useFrame( const u32 idx );
	void createFrames( const vector<VkImage> images, const UInt2 size );

	void setupAttachment() override;
	void setupSubpass() override;
	void setupDependency() override;

private:

	vector<Frame*> mFrames;

};

