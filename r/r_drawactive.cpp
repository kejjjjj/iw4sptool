#include <print>

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "r_drawactive.hpp"
#include "r_drawtools.hpp"
#include "utils/hook.hpp"
#include "dvar/dvar.hpp"
#include "cm/cm_cgentities.hpp"
#include "cm/cm_entity.hpp"
#include "cm/cm_debug.hpp"

#include <windows.h>
#include <sstream>

static void CG_DrawTrigger(float& y, const vec4_t color)
{
	auto& trigger = CGDebugData::currentTrigger;

	if (!trigger.ent) 
		return;
	
	std::stringstream ss;

	for (auto& [k, v] : trigger.m_oEntityFields)
		ss << k << ": " << v << '\n';

	auto asStr = ss.str();

	if (asStr.length()) {
		R_DrawTextWithEffects(asStr.c_str(), "fonts/bigdevfont", 0, y, 0.4f, 0.45f, 0, color, 3, vec4_t{ 1,0,0,0 });
		y += 10 * trigger.m_oEntityFields.size();
	}

}

static void CG_DrawCoordinates(float& y, const vec4_t color)
{

	char buff[256];
	sprintf_s(buff,
		"x:       %.6f\n"
		"y:       %.6f\n"
		"z:       %.6f", 
		viewpos[0], viewpos[1], viewpos[2]);

	R_DrawTextWithEffects(buff, "fonts/bigdevfont", 0, y, 0.4f, 0.45f, 0, color, 3, vec4_t{ 1,0,0,0 });
	y += 25.f;
}

void CG_DrawFullScreenDebugOverlays(int localClientNum) {

	static dvar_s* pm_coordinates = Dvar_FindMalleableVar("pm_coordinates");
	static dvar_s* cm_entityInfo = Dvar_FindMalleableVar("cm_entityInfo");
	static dvar_s* cm_showCollisionDist = Dvar_FindMalleableVar("cm_showCollisionDist");

	//CG_CornerDebugPrint("Hello!", vec4_t{ 1,1,1,1 }, Scr_GetPlacement(), 100.f, 100.f);

	if (const auto f = CStaticHooks::FindByName("CG_DrawFullScreenDebugOverlays")) {
		f->Call<void>(localClientNum);
	}

	vec4_t color{ 1,0.753f,0.796f,0.7f };
	float y = 45.f;

	if (pm_coordinates && pm_coordinates->current.enabled) 
		CG_DrawCoordinates(y, color);

	CG_DrawTrigger(y, color);

	if (cm_showCollisionDist && cm_entityInfo && cm_entityInfo->current.integer) {

		CGentities::ForEach([](GentityPtr_t& ptr) {
			ptr->CG_Render2D(cm_showCollisionDist->current.value, static_cast<entity_info_type>(cm_entityInfo->current.integer));
		});

	}
}
