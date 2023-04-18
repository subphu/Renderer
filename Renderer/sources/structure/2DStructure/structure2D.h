#pragma once

#include <structure/structure.h>

class Structure2D : public Structure {

public:
	~Structure2D();
	Structure2D();

	void setup() override;
	void update( RenderTime renderTime ) override;
	void draw() override;
	void refresh() override;
	void cleanup() override;

private:

	Layer mResolveLayer;

};

