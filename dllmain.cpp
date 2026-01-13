#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>

#include "cg/cg_init.hpp"

#define USE_CONSOLE 0

BOOL APIENTRY DllMain([[maybe_unused]]HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    std::jthread t;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        t = std::jthread([]() {
            #if(USE_CONSOLE)
                FILE* _con = 0;
                AllocConsole();
                freopen_s(&_con, "CONOUT$", "w", stdout);
            #endif

            puts("hello!");

            CG_Init();

            #if(USE_CONSOLE)
                if (_con)
                    fclose(_con);
            #endif
        });
        t.detach();

        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

