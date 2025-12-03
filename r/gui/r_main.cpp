#include "r_main.hpp"
#include "r/gui/r_console.hpp"

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

#include <Windows.h>

std::unique_ptr<CMainGui> CStaticMainGui::Owner{};
std::unique_ptr<CConsoleGui> CStaticMainGui::Console{};

CMainGui::CMainGui(IDirect3DDevice9* device, void* hwnd) {

	if (ImGui::GetCurrentContext() || !device || !hwnd)
		return;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(device);

	io.WantCaptureMouse = true;
	io.WantCaptureKeyboard = true;
}

bool CMainGui::OnFrameBegin()
{
	if (!ImGui::GetCurrentContext())
		return false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}

void CMainGui::OnFrameEnd()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
void CMainGui::Render()
{
	if (!b_open)
		return;

	ImGui::ShowDemoWindow();

	//...nothing?
}
void CStaticMainGui::Initialize() {
	if (!Owner) {
		Owner = std::make_unique<CMainGui>(dx->device, FindWindowA(NULL, "Modern Warfare 2"));
	}
}
void CStaticMainGui::Toggle() {

	if (!Owner) {
		Initialize();
	}

	Owner->Toggle();
}