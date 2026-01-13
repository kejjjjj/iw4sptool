#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cmd/cmd.hpp"
#include "dvar/dvar.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "r_console.hpp"
#include "utils/resolution.hpp"

#include <algorithm>
#include <ranges>

void CConsoleGui::Render()
{
	ImGui::Begin("##console2", nullptr, 
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar 
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

	fvec2 size = adjust_from_640x480({ 640.f, 480.f });
	size.x -= ImGui::GetStyle().FramePadding.x * 2;
	size.y -= ImGui::GetStyle().FramePadding.y * 2;

	ImGui::SetWindowSize({ size.x, size.y });
	ImGui::SetWindowPos(ImGui::GetStyle().FramePadding, ImGuiCond_Once);

	DrawConsoleInput(size.x);

	const auto& keys = ImGui::GetIO().KeysData;

	const auto rsHeld = keys[ImGuiKey_RightShift - ImGuiKey_NamedKey_BEGIN].DownDuration >= 0.f;
	const auto lsHeld = keys[ImGuiKey_LeftShift - ImGuiKey_NamedKey_BEGIN].DownDuration >= 0.f;

	const auto rsPressed = keys[ImGuiKey_RightShift - ImGuiKey_NamedKey_BEGIN].DownDuration == 0.f;
	const auto lsPressed = keys[ImGuiKey_LeftShift - ImGuiKey_NamedKey_BEGIN].DownDuration == 0.f;

	if ((rsHeld && lsPressed) || (lsHeld && rsPressed))
		m_bRenderingOutput = !m_bRenderingOutput;

	if(m_bRenderingOutput)
		DrawConsoleOutput();


	ImGui::End();
}
void CConsoleGui::OnEnterPressed()
{
	if (m_inputText.empty())
		return;

	const auto str = m_inputText + "\n";
	Cbuf_AddText(str.c_str());
	auto& m_history = CStaticConsoleGui::m_history.m_history;
	m_history.insert(m_history.begin(), m_inputText);

	if (m_history.size() > 100)
		m_history.erase(m_history.begin() + 100, m_history.end());

	m_inputText.clear();
}
void CConsoleGui::OnUpPressed() {

	auto& m_history = CStaticConsoleGui::m_history.m_history;
	auto& m_depth = CStaticConsoleGui::m_history.m_depth;

	if (m_history.empty()) {
		m_inputText = "";
		return;
	}

	m_inputText = m_history[m_depth];
	m_depth = std::clamp(m_depth + 1, 0, static_cast<std::int32_t>(m_history.size()) - 1);
}
void CConsoleGui::OnDownPressed() {

	auto& m_history = CStaticConsoleGui::m_history.m_history;
	auto& m_depth = CStaticConsoleGui::m_history.m_depth;
	if (m_history.empty()) {
		m_inputText = "";
		return;
	}

	if (m_depth == 0) {
		m_inputText = "";
		return;
	}

	m_depth = std::clamp(m_depth - 1, 0, static_cast<std::int32_t>(m_history.size()) - 1);
	m_inputText = m_history[m_depth]; //clear when there is nothing
}
void CConsoleGui::OnUpdated(ImGuiInputTextCallbackData* data) {
	data->DeleteChars(0, data->BufTextLen);
	data->InsertChars(0, m_inputText.c_str());
}

dvar_s** sortedDvars = reinterpret_cast<dvar_s**>(0x1965A90);
int* dvarCount = reinterpret_cast<int*>(0x1965A68);

int CConsoleGui::InputTextCallback(ImGuiInputTextCallbackData* data)
{
	const auto console = CStaticConsoleGui::Console.get();

	if (!console)
		return 0;

	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackHistory:
		console->m_autoCompletionIndex = -1;
		if (data->EventKey == ImGuiKey_UpArrow){
			console->OnUpPressed();  
			console->OnUpdated(data);
			return 1;
		}
		if (data->EventKey == ImGuiKey_DownArrow){
			console->OnDownPressed();
			console->OnUpdated(data);
			return 1;
		}
		break;
	case ImGuiInputTextFlags_CallbackCharFilter:
		if(data->EventChar != '\t')
			console->m_autoCompletionIndex = -1;
		break;
	case ImGuiInputTextFlags_CallbackCompletion:
		
		if (console->m_suggestions.empty())
			return 0;

		console->m_autoCompletionIndex = (console->m_autoCompletionIndex + 1) % static_cast<std::int32_t>(console->m_suggestions.size());
		if(console->m_suggestions[console->m_autoCompletionIndex].m_callback)
			console->m_inputText = std::get<1>(console->m_suggestions[console->m_autoCompletionIndex].m_suggestion);
		else
			console->m_inputText = std::get<0>(console->m_suggestions[console->m_autoCompletionIndex].m_suggestion)->name;

		console->OnUpdated(data);
		return 1;
	}
	return 0;
}

std::string Trim(std::string const& str) {
	static char const* whitespaceChars = "\n\r\t ";
	std::string::size_type start = str.find_first_not_of(whitespaceChars);
	std::string::size_type end = str.find_last_not_of(whitespaceChars);

	return start != std::string::npos ? str.substr(start, 1 + end - start) : std::string();
}
void CConsoleGui::UpdateConsoleSuggestions(const std::string& src)
{
	const auto toLower = [](std::uint8_t c) { return std::tolower(c); };
	std::string srcCopy = Trim(src);

	static char const* whitespaceChars = "\n\r\t ";
	const auto first_whitespace = srcCopy.find_first_of(whitespaceChars);

	if (first_whitespace != std::string::npos)
		srcCopy = srcCopy.substr(0, first_whitespace);

	std::transform(srcCopy.begin(), srcCopy.end(), srcCopy.begin(), toLower);

	const auto prefixOnly = Dvar_FindMalleableVar("con_matchPrefixOnly");

	m_suggestions.clear();

	if (srcCopy.empty())
		return;

	const auto testMatch = [this, &prefixOnly](const auto& a, const auto& b, CConsoleSuggestion&& sugg) -> bool {
		if (a == b) {
			m_suggestions.clear();
			m_suggestions.push_back(sugg); //exact match, clear everything else
			return true;
		}

		if (prefixOnly && prefixOnly->current.enabled) {
			if (a.starts_with(b))
				m_suggestions.push_back(sugg);
		}
		else if (a.contains(b)) {
			m_suggestions.push_back(sugg);
		}

		return false;
	};

	for (auto f = cmd_functions->next; f != nullptr; f = f->next) {

		if (!f->name)
			continue;

		auto name = std::string(f->name);
		std::transform(name.begin(), name.end(), name.begin(), toLower);

		if (testMatch(name, srcCopy, CConsoleSuggestion{ .m_suggestion = f->name, .m_callback = true }))
			return;
	}

	for (const auto i : std::views::iota(0, *dvarCount)) {
		const auto dvar = sortedDvars[i];

		auto name = std::string(dvar->name);
		std::transform(name.begin(), name.end(), name.begin(), toLower);

		if (testMatch(name, srcCopy, CConsoleSuggestion{ .m_suggestion=dvar, .m_callback=false }))
			return;
	}
}
void CConsoleGui::DrawConsoleInput(float w)
{
	ImGui::Text(">");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(w - ImGui::GetCursorPosX());
	ImGui::SetKeyboardFocusHere();

	if (ImGui::InputText("##label2", &m_inputText, ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_CallbackHistory |
		ImGuiInputTextFlags_CallbackCharFilter |
		ImGuiInputTextFlags_CallbackCompletion,
		InputTextCallback)) {
		OnEnterPressed();
	}

	if (m_oldInputLength != m_inputText.length() && m_autoCompletionIndex == -1) {
		UpdateConsoleSuggestions(m_inputText);
		m_oldInputLength = m_inputText.length();
	}

	if (m_suggestions.size()) {
		DrawSuggestions();
	}

}
void CConsoleGui::DrawSuggestions() {
	const float key_width = static_cast<float>(adjust_from_640(180.0f));

	auto Row = [&](const char* key, const char* value, const ImVec4 color={1,1,1,1}) {
		ImGui::AlignTextToFramePadding();
		ImGui::TextColored(color, key);

		ImGui::SameLine(key_width);
		ImGui::TextColored(color, value);
	};

	if (m_suggestions.size() == 1) {
		const auto& var = m_suggestions.front();

		if (var.m_callback) {
			ImGui::Text(std::get<1>(var.m_suggestion).c_str());
		} else {
			const auto dvar = std::get<0>(var.m_suggestion);
			ImGui::Text(dvar->description);
			const auto d = Dvar_DomainToString(dvar->type, dvar->domain);
			ImGui::Text(d.c_str());
		}
		
	}

	for (const auto& s : m_suggestions) {

		if (s.m_callback)
			Row(std::get<1>(s.m_suggestion).c_str(), "", ImVec4{ 0.7f, 0.8f, 1.f,1.f});
		else {
			const auto d = std::get<0>(s.m_suggestion);
			Row(d->name, Dvar_ValueToString(d, d->current));
		}
	}
}
void CConsoleGui::DrawConsoleOutput()
{
	const fvec2 size = adjust_from_640x480({ 640.f, 480.f });
	float y = size.y - ImGui::GetCursorPosY();

	ImGui::BeginChild("##child1", { size.x, y });

	int row = 0;
	
	for (auto i = 0; i < con->consoleWindow.lineCount; i++) {
		const auto lineIndex = (row + con->consoleWindow.firstLineIndex + i) % con->consoleWindow.lineCount;
		const auto textPool = con->consoleWindow.circularTextBuffer;
		const auto charCount = con->consoleWindow.lines[lineIndex].textBufSize;
		const auto poolSize = con->consoleWindow.textBufSize;
		const auto firstChar = con->consoleWindow.lines[lineIndex].textBufPos;

		if (charCount < 1)
			continue;

		std::string text;
		if (charCount > poolSize - firstChar) {
			text = std::string(&textPool[firstChar], poolSize - firstChar);
			text += std::string(textPool, charCount - (poolSize - firstChar));
		}
		else
			text = std::string(&textPool[firstChar], charCount);


		ImGui::Text("%s", text.c_str());
	}
	ImGui::EndChild();
}

std::unique_ptr<CConsoleGui> CStaticConsoleGui::Console{};
CConsoleHistory CStaticConsoleGui::m_history;

void CStaticConsoleGui::Toggle() {

	if (!Console) {
		Console = std::make_unique<CConsoleGui>();
		Console->Open();
		return;
	}

	m_history.m_depth = 0;
	Console->Close();
	Console = nullptr;
}