#pragma once

#include <common.h>

#include "triangleStructure/plTriangle.h"

#include "defaultStructure/renderpass/rpResolve.h"
#include "defaultStructure/pipeline/plResolve.h"

#include "2DStructure/renderpass/rpResolve2D.h"
#include "2DStructure/pipeline/plResolve2D.h"

class Renderpass;
class Pipeline;
class Image;

struct RenderTime;

class Structure {

public:
	struct Layer {
		Renderpass* renderpass;
		vector<Pipeline*> pipelines;
	};

public:
	~Structure() {};
	Structure() {};

	virtual void setup() = 0;
	virtual void update( RenderTime renderTime ) = 0;
	virtual void draw() = 0;
	virtual void cleanup() = 0;

	Image* getRenderedImage() { return mRenderedImagePtr; }

protected:

	Image* mRenderedImagePtr = nullptr;

};

