#include "utils/hook.hpp"
#include "cm/cm_renderer.hpp"
#include "cm/cm_brush.hpp"
#include "cm/cm_typedefs.hpp"
#include "r_endscene.hpp"
#include "r/gui/r_main.hpp"
#include "r/gui/r_console.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "cm/cm_clipmap.hpp"

#include <print>

long __stdcall R_EndScene(IDirect3DDevice9* device) {

	auto mainGui = CStaticMainGui::Owner.get();

	if (mainGui && mainGui->OnFrameBegin()) {

		auto console = CStaticConsoleGui::Console.get();

		if (console) {
			console->Render();
		}

		mainGui->OnFrameEnd();
	}

	if (const auto f = CStaticHooks::FindByName("R_EndScene")) {
		return f->Call<long>(device);
	}

	return S_OK;
}
void RB_DrawDebug(struct GfxViewParms* viewParms) {

	if (const auto f = CStaticHooks::FindByName("RB_DrawDebug")) {
		f->Call<void>(viewParms);
	}

	CM_ShowCollision();
}

void R_RecoverLostDevice()
{
	if (const auto f = CStaticHooks::FindByName("R_RecoverLostDevice")) {
		f->Call<void>(CC::Cdecl);
	}

	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
}
void CL_InitRef() {

	CStaticMainGui::Close();
	CStaticConsoleGui::Close();
	CStaticMainGui::Console = 0;
	CStaticMainGui::Owner = 0;

	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		ImGui::SetCurrentContext(nullptr);
	}

	if (const auto f = CStaticHooks::FindByName("CL_InitRef")) {
		return f->Call<void>(CC::Cdecl);
	}
}
int R_CreateDeviceInternal(struct HWND__* hwnd, int BehaviorFlags, struct _D3DPRESENT_PARAMETERS_* d3dpp)
{
	if (const auto f = CStaticHooks::FindByName("R_CreateDeviceInternal")) {
		const auto r = f->Call<int>(hwnd, BehaviorFlags, d3dpp);

		if (r >= 0) {
			CStaticMainGui::Initialize();
		}
		
		return r;
	}
	return 0;
}
