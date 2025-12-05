#include <print>

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "r_drawactive.hpp"
#include "r_drawtools.hpp"
#include "utils/hook.hpp"
#include "dvar/dvar.hpp"
#include <windows.h>

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


	//CG_CornerDebugPrint("Hello!", vec4_t{ 1,1,1,1 }, Scr_GetPlacement(), 100.f, 100.f);

	if (const auto f = CStaticHooks::FindByName("CG_DrawFullScreenDebugOverlays")) {
		f->Call<void>(localClientNum);
	}

	vec4_t color{ 1,0.753f,0.796f,0.7f };
	float y = 45.f;

	if (pm_coordinates && pm_coordinates->current.enabled) 
		CG_DrawCoordinates(y, color);
}
