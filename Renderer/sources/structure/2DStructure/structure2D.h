#pragma once

#include <structure/structure.h>

class Structure2D : public Structure {

public:
	~Structure2D();
	Structure2D();

	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override;

private:

	Layer mResolveLayer;

};

