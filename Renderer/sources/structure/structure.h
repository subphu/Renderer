#pragma once

#include <common.h>
#include <system.h>

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
	virtual void refresh() = 0;
	virtual void cleanup() = 0;

	void setupViewportScissor() {
		UInt2D extent = System::Window()->getFrameSize();
		mViewport.width  = F32(extent.width);
		mViewport.height = F32(extent.height);
		mScissor.extent  = extent;
	}

	Image* getRenderedImage() { return mRenderedImagePtr; }

protected:

	VkViewport mViewport { 0, 0, 0, 0, 0, 1 };
	VkRect2D   mScissor { { 0, 0 }, { 0, 0 } };

	Image* mRenderedImagePtr = nullptr;


};

