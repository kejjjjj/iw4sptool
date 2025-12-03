#include "sys_thread.hpp"
#include <windows.h>

unsigned int* threadHandles = reinterpret_cast<unsigned int*>(0x1551BB0);

void Sys_SuspendAllThreads() {

	_EnterCriticalSection(34);

	for (auto i = 0u; i < NUM_GAME_THREADS; i++) {
		SuspendThread((HANDLE)threadHandles[i]);
	}

	_LeaveCriticalSection(34);
}
void Sys_ResumeAllThreads() {
	for (auto i = 0u; i < NUM_GAME_THREADS; i++) {
		ResumeThread((HANDLE)threadHandles[i]);
	}
}

void _EnterCriticalSection(int v) {
	__asm{
		push v;
		mov esi, 0x491E80;
		call esi;
		add esp, 4;
	}
}
void _LeaveCriticalSection(int v) {
	__asm {
		push v;
		mov esi, 0x4D5410;
		call esi;
		add esp, 4;
	}
}
