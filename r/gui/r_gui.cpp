#include "r_gui.hpp"
#include "in/in_main.hpp"
#include "utils/resolution.hpp"
#include "imgui/imgui.h"

void CBaseGui::Toggle()
{
	b_open = !b_open;
	if (b_open)
		return Open();

	Close();
}

void CBaseGui::Open()
{
	IN_ActivateMouse(false);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(false);

#ifdef IMGUI_API
	ImGui::GetIO().MouseDrawCursor = true;
#endif

	b_open = true;
}

void CBaseGui::Close()
{

	IN_ActivateMouse(true);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(true);

#ifdef IMGUI_API
	ImGui::GetIO().MouseDrawCursor = false;
#endif	

	b_open = false;

	OnClose();
}

fvec2 CBaseGui::GetRealSize() const {
	const auto v = ImGui::GetWindowSize();
	return { v.x, v.y };
}
fvec2 CBaseGui::GetRealPos() const {
	const auto v = ImGui::GetWindowPos();
	return { v.x, v.y };
}
fvec2 CBaseGui::GetVirtualSize() const { return adjust_to_640x480(GetRealSize()); }
fvec2 CBaseGui::GetVirtualPos() const { return adjust_to_640x480(GetRealPos()); }