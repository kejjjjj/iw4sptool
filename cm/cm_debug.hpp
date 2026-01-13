#pragma once

#include <string>
#include <unordered_map>

#include "cg/cg_local.hpp"

struct CGDebugData
{
	static volatile int tessVerts;
	static volatile int tessIndices;
	static GfxPointVertex g_debugPolyVerts[2725];

	static struct TriggerData {
		gentity_s* ent;
		std::unordered_map<std::string, std::string> m_oEntityFields;
	}currentTrigger;

};


void CM_LoadDvars();