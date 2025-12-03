#pragma once

#include "imgui/imgui.h"
#include "imgui_stdlib.h"

#include <string>
#include <windows.h>

struct InputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextWithHint(label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}
bool ImGui::ButtonCentered(const char* label, float alignment, ImVec2 _size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f + _size.x;
    const float avail = ImGui::GetContentRegionAvail().x;

    const float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label, _size);
}
void ImGui::TextCentered(const char* label, ...)
{
    char v2[4096];
    va_list va;

    va_start(va, label);
    _vsnprintf_s(v2, 0x1000u, label, va);
    v2[4095] = 0;

    ImGuiStyle& style = ImGui::GetStyle();

    const float size = ImGui::CalcTextSize(v2).x + style.FramePadding.x * 2.0f;
    const float avail = ImGui::GetContentRegionAvail().x;

    const float off = (avail - size) * .5f;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    ImGui::Text(v2);
}
void ImGui::CenterNextItem(float sizeX)
{
    const float avail = ImGui::GetContentRegionAvail().x;

    const float off = (avail - sizeX) * .5f;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}
void ImGui::TextCenteredVertical(const char* label, ...)
{
    char v2[4096];
    va_list va;

    va_start(va, label);
    _vsnprintf_s(v2, 0x1000u, label, va);
    v2[4095] = 0;

    ImGuiStyle& style = ImGui::GetStyle();

    auto size = ImGui::CalcTextSize(v2);
    size.x += style.FramePadding.x * 2.0f;
    size.y += style.FramePadding.y * 2.0f;

    auto avail = ImGui::GetContentRegionAvail();
    ImVec2 off = { (avail.x - size.x) * .5f, (avail.y - size.y) * .5f };

    if (off.x > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off.x);
    if (off.y > 0.0f)
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off.y);

    ImGui::Text(v2);
}

bool ImGui::IsHovered(ImVec2 mins, ImVec2 maxs)
{
    tagPOINT p;
    ::GetCursorPos(&p);

    return (
        p.x > mins.x
        && p.y > mins.y
        && p.x < maxs.x
        && p.y < maxs.y);

}
bool ImGui::IsClicked(ImVec2 mins, ImVec2 maxs)
{
    return IsHovered(mins, maxs) && GetIO().MouseDownDuration[0] == 0.f;
}