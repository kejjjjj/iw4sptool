#include <print>

#include "cg/cg_local.hpp"
#include "r_drawactive.hpp"
#include "r_drawtools.hpp"
#include "utils/hook.hpp"
#include <windows.h>


void CG_DrawActive(int localClientNum) {
	//std::string t = std::to_string(localClientNum);
	//MessageBoxA(NULL, t.c_str(), "EEEE", MB_ICONMASK);
	const auto f = CStaticHooks::FindByName("CG_DrawActive");



	if (f) {
		((void(*)(int))f->trampoline)(localClientNum);

		const auto font = R_RegisterFont("fonts/normalfont");
		R_AddCmdDrawTextWithEffects((char*)"Hello!", 20, font, 320, 240, 10, 10, 0,
			vec4_t{ 0,1,0,1 }, 1, vec4_t{ 0,0,0,0 }, nullptr, nullptr, 0, 0, 0, 0);
	}
}
