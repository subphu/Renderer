#pragma once

#include <core/renderpass.h>

class RpResolve : public Renderpass {

public:
	~RpResolve();
	RpResolve();

	void setupAttachment() override;
	void setupSubpass() override;
	void setupDependency() override;
	void createFrame();

private:

	vector<Frame*> mFrames;

};

