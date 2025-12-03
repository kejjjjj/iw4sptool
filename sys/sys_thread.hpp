#pragma once

#define NUM_GAME_THREADS 8u

void Sys_SuspendAllThreads();
void Sys_ResumeAllThreads();

void _EnterCriticalSection(int v);
void _LeaveCriticalSection(int v);

