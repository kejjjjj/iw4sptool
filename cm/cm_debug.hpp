#pragma once

#include <string>
#include <unordered_map>

#include "cg/cg_local.hpp"
#include "utils/defs.hpp"

struct CGDebugData
{
	static volatile int tessVerts;
	static volatile int tessIndices;
	static GfxPointVertex g_debugPolyVerts[2725];

	static struct TriggerData {
		gentity_s* ent;
		std::unordered_map<std::string, std::string> m_oEntityFields;
		std::string m_sDisplayText;
		unsigned long m_uTouchTime;
	}currentTrigger;

};

void CM_LoadDvars();
void CM_DrawDebug(float& y, const vec4_t color);
