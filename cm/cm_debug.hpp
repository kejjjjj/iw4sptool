#pragma once

#include "cg/cg_local.hpp"

struct CGDebugData
{
	static volatile int tessVerts;
	static volatile int tessIndices;
	static GfxPointVertex g_debugPolyVerts[2725];
};
