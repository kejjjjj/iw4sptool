#pragma once

#include <memory>

#include "r_gui.hpp"

struct IDirect3DDevice9;

class CMainGui final : public CBaseGui
{
public:

	CMainGui(IDirect3DDevice9* device, void* hwnd);

	[[nodiscard]] bool OnFrameBegin();
	void OnFrameEnd();
	void Render() override;
};
class CConsoleGui;

class CStaticMainGui final
{
public:
	static void Initialize();
	static void Open() { if(Owner) Owner->Open(); }
	static void Close() { if (Owner) Owner->Close(); }
	static void Toggle();

	static std::unique_ptr<CMainGui> Owner;
	static std::unique_ptr<CConsoleGui> Console;
};