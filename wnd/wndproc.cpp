#include "wndproc.hpp"

#include "imgui/imgui.h"
#include "utils/hook.hpp"
#include "r/gui/r_console.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return 1l;
	}

	switch (uMsg) {
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_KEYDOWN:
		if (wParam == 220) {
			CStaticConsoleGui::Toggle();
			return 1;
		}

		if (CStaticConsoleGui::Console && CStaticConsoleGui::Console->IsOpen())
			return 1;

		break;
	}

	if (const auto f = CStaticHooks::FindByName("WndProc")) {
		return f->Call<LRESULT>(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}