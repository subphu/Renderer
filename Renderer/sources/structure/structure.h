#pragma once

#include <common.h>

#include <renderpass/rpResolve.h>

#include <pipeline/plResolve.h>

class Renderpass;
class Pipeline;
class Image;

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
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void cleanup() = 0;

	Image* getRenderedImage() { return mRenderedImagePtr; }

protected:

	Image* mRenderedImagePtr = nullptr;

};

