#include <chrono>
#include <print>
#include <thread>
#include <Windows.h>

#include "cg_local.hpp"
#include "cg_offsets.hpp"
#include "cmd/cmd.hpp"
#include "cm/cm_brush.hpp"
#include "in/in_main.hpp"
#include "r/gui/r_main.hpp"
#include "r/r_drawactive.hpp"
#include "r/r_endscene.hpp"
#include "sys/sys_thread.hpp"
#include "utils/engine.hpp"
#include "utils/hook.hpp"
#include "wnd/wndproc.hpp"
#include "r/gui/r_console.hpp"

using namespace std::chrono_literals;

void CG_Init() {

	while (!dx || !dx->device)
		std::this_thread::sleep_for(200ms);

	Sys_SuspendAllThreads();
	std::this_thread::sleep_for(300ms);

	CStaticHooks::Initialize();
	CStaticMainGui::Initialize();

	if (dx && dx->device) {
		CStaticHooks::Create("R_EndScene", reinterpret_cast<std::uintptr_t>((*reinterpret_cast<PVOID**>(dx->device))[42]), R_EndScene);
		CStaticHooks::Create("WndProc", 0x4D89E0, WndProc);
		//CStaticHooks::Create("R_Shutdown", 0x50BE60, R_Shutdown);
		CStaticHooks::Create("R_RecoverLostDevice", 0x50C4F0, R_RecoverLostDevice);
		CStaticHooks::Create("R_CreateDeviceInternal", 0x50B3B0, R_CreateDeviceInternal);
		CStaticHooks::Create("RB_DrawDebug", 0x5594C0, RB_DrawDebug);
		CStaticHooks::Create("__asm_adjacency_winding", 0x483D16, __brush::__asm_adjacency_winding);
	}

	Cmd_AddCommand("cm_showCollisionFilter", CM_ShowCollisionFilter);

	//Cmd_AddCommand("showconsole", CStaticConsoleGui::Toggle);
	Sys_ResumeAllThreads();

	while (!(GetAsyncKeyState(VK_HOME)&1)) {
		std::this_thread::sleep_for(100ms);
	}

	Sys_SuspendAllThreads();
	CStaticHooks::Uninitialize(); //removes all hooks
	Sys_ResumeAllThreads();
}