#pragma once

#include <string>
#include <vector>
#include <variant>

#include "r_gui.hpp"
struct ImGuiInputTextCallbackData;
struct dvar_s;

using CCallbackCommand = std::string;

struct CConsoleSuggestion {
	std::variant<dvar_s*, CCallbackCommand> m_suggestion;
	bool m_callback{ false };
};

class CConsoleGui : public CBaseGui
{
public:
	void Render() override;

private:
	void OnEnterPressed();
	void OnUpPressed();
	void OnDownPressed();
	void OnUpdated(ImGuiInputTextCallbackData* data);
	static int InputTextCallback(ImGuiInputTextCallbackData* data);

	void UpdateConsoleSuggestions(const std::string& src);

	void DrawConsoleInput(float w);
	void DrawSuggestions();
	void DrawConsoleOutput();

	std::string m_inputText{};
	std::size_t m_oldInputLength{};


	bool m_bRenderingOutput{};
	std::int32_t m_autoCompletionIndex{-1};
	std::vector<CConsoleSuggestion> m_suggestions;
};

struct CConsoleHistory {
	std::vector<std::string> m_history;
	std::int32_t m_depth;
};

class CStaticConsoleGui final
{
public:
	static void Open() { if (Console) Console->Open(); }
	static void Close() { if (Console) Console->Close(); }
	static void Toggle();

	static std::unique_ptr<CConsoleGui> Console;
	static CConsoleHistory m_history;
};
