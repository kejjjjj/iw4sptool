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
#include "cm/cm_debug.hpp"

using namespace std::chrono_literals;

auto SteamTest()
{
	std::print("yoooo\n");
	const auto v = Engine::Tools::Call<HANDLE>(0x004297D0);
	*(HANDLE*)0x1BF9A44 = v;
	return *(HANDLE*)0x1BF9A44;
}

void CG_Init() {

	while (!dx || !dx->device)
		std::this_thread::sleep_for(200ms);

	Sys_SuspendAllThreads();
	std::this_thread::sleep_for(300ms);

	CStaticHooks::Initialize();
	CStaticMainGui::Initialize();

	Engine::Tools::WriteBytes(0x424CB0, "\xC3");
	Engine::Tools::WriteBytes(0x474E10, "\xC3");
	Engine::Tools::WriteBytes(0x410870, "\xC3");
	Engine::Tools::WriteBytes(0x401190, "\xC3");
	Engine::Tools::WriteBytes(0x4516F0, "\xC3");
	Engine::Tools::WriteBytes(0x427540, "\xC3");
	Engine::Tools::WriteBytes(0x4A9F80, "\xC3");
	Engine::Tools::WriteBytes(0x478980, "\xC3");
	Engine::Tools::WriteBytes(0x4A6740, "\xC3");
	Engine::Tools::WriteBytes(0x4093E0, "\xC3");

	if (dx && dx->device) {
		CStaticHooks::Create("SteamTest", 0x4257D0, SteamTest);

		CStaticHooks::Create("R_EndScene", reinterpret_cast<std::uintptr_t>((*reinterpret_cast<PVOID**>(dx->device))[42]), R_EndScene);
		CStaticHooks::Create("WndProc", 0x4D89E0, WndProc);
		//CStaticHooks::Create("R_Shutdown", 0x50BE60, R_Shutdown);
		CStaticHooks::Create("R_RecoverLostDevice", 0x50C4F0, R_RecoverLostDevice);
		CStaticHooks::Create("R_CreateDeviceInternal", 0x50B3B0, R_CreateDeviceInternal);
		CStaticHooks::Create("RB_DrawDebug", 0x5594C0, RB_DrawDebug);
		CStaticHooks::Create("__asm_adjacency_winding", 0x483D16, __brush::__asm_adjacency_winding);
	}

	CM_LoadDvars();

	//Cmd_AddCommand("showconsole", CStaticConsoleGui::Toggle);
	Sys_ResumeAllThreads();

	while (!(GetAsyncKeyState(VK_HOME)&1)) {
		std::this_thread::sleep_for(100ms);
	}

	Sys_SuspendAllThreads();
	CStaticHooks::Uninitialize(); //removes all hooks
	Sys_ResumeAllThreads();
}