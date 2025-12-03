#pragma once

struct cmd_function_s;

[[maybe_unused]] cmd_function_s* Cmd_FindCommand(const char* name);
[[maybe_unused]] void Cmd_AddCommand(const char* cmdname, void(__cdecl* function)());

void Cbuf_AddText(const char* text, int localClientNum=0);
