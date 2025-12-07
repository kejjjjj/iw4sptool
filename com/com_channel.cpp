#include <Windows.h>
#include <stdio.h>

#include "cg/cg_local.hpp"
#include "utils/engine.hpp"

void Com_Printf($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...)
{
	char v2[4096];
	va_list va; 

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	return Engine::Tools::Call<void>(0x466380, channel, v2);
}
void Com_Printf(const char* msg, ...)
{
	char v2[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	return Engine::Tools::Call<void>(0x466380, CON_CHANNEL_CONSOLEONLY, v2);
}
