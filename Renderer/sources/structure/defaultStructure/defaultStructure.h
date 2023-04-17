#pragma once

#include <structure/structure.h>

class DefaultStructure : public Structure {

public:
	~DefaultStructure();
	DefaultStructure();

	void setup() override;
	void update( RenderTime renderTime ) override;
	void draw() override;
	void cleanup() override;

private:

	Layer mResolveLayer;

};

