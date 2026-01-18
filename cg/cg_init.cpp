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
#include "g/g_entity.hpp"
#include "cl/cl_main.hpp"
#include "g/g_spawn.hpp"

using namespace std::chrono_literals;

auto SteamTest()
{
	std::print("yoooo\n");
	const auto v = Engine::Tools::Call<HANDLE>(0x004297D0);
	*(HANDLE*)0x1BF9A44 = v;
	return *(HANDLE*)0x1BF9A44;
}

void CG_Init() {
	std::print("yoooo\n");

	auto numAttempts = 0u;
	while (!dx || !dx->device) {
		std::print("bad\n");

		if (++numAttempts >= 25) {
			MessageBox(NULL, "directx device got stuck :(", "Error!", MB_ICONERROR);
			return;
		}

		std::this_thread::sleep_for(200ms);
	}
	std::print("start wait\n");
	std::this_thread::sleep_for(2s);

	Sys_SuspendAllThreads();
	std::print("suspended\n");

	std::this_thread::sleep_for(300ms);
	std::print("gg\n");

	CStaticHooks::Initialize();
	std::print("hooks\n");
	CStaticMainGui::Initialize();
	std::print("gui\n");

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
	Engine::Tools::WriteBytes(0x552B24, "\xEB\x00"s); //je -> jmp + 0
	Engine::Tools::NOP(0x04D35F2); // radius not specified for trigger_radius at (%g %g %g)
	Engine::Tools::NOP(0x44A45E); // radius not specified for trigger_radius at (%g %g %g)
	Engine::Tools::NOP(0x4D3642); // height not specified for trigger_radius at (%g %g %g)

	std::print("patches\n");

	CStaticHooks::Create("SteamTest", 0x4257D0, SteamTest);
	CStaticHooks::Create("R_EndScene", reinterpret_cast<std::uintptr_t>((*reinterpret_cast<PVOID**>(dx->device))[42]), R_EndScene);
	CStaticHooks::Create("WndProc", 0x4D89E0, WndProc);
	CStaticHooks::Create("CL_InitRef", 0x421100, CL_InitRef);
	CStaticHooks::Create("R_RecoverLostDevice", 0x50C4F0, R_RecoverLostDevice);
	CStaticHooks::Create("R_CreateDeviceInternal", 0x50B3B0, R_CreateDeviceInternal);
	CStaticHooks::Create("RB_DrawDebug", 0x5594C0, RB_DrawDebug);
	CStaticHooks::Create("__asm_adjacency_winding", 0x483D16, __brush::__asm_adjacency_winding);
	CStaticHooks::Create("CG_DrawFullScreenDebugOverlays", 0x4E2340, CG_DrawFullScreenDebugOverlays);
	CStaticHooks::Create("CL_Disconnect", 0x443DF0, CL_Disconnect);
	CStaticHooks::Create("G_ParseEntityFields", 0x5DB0E7, G_ParseEntityFields);

	CStaticHooks::Create("G_Trigger", 0x46E2C0, G_Trigger);
	
	std::print("hooks\n");

	CM_LoadDvars();
	std::print("dvars\n");

	//Cmd_AddCommand("showconsole", CStaticConsoleGui::Toggle);
	Sys_ResumeAllThreads();
	std::print("resume threads\n");

	while (!(GetAsyncKeyState(VK_HOME)&1)) {
		std::this_thread::sleep_for(100ms);
	}

	Sys_SuspendAllThreads();
	CStaticHooks::Uninitialize(); //removes all hooks
	Sys_ResumeAllThreads();
}