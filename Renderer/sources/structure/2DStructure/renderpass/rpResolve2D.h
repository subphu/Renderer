#pragma once

#include <core/renderpass.h>

class RpResolve2D : public Renderpass {

public:
	~RpResolve2D();
	RpResolve2D();

	void setupAttachment() override;
	void setupSubpass() override;
	void setupDependency() override;
	void createFrame();

private:

	vector<Frame*> mFrames;

};

