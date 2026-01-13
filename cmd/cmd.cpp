#include "cmd.hpp"
#include "cg/cg_local.hpp"
#include "utils/engine.hpp"

#include <cstdint>
#include <vector>

cmd_function_s* Cmd_FindCommand(const char* name) {
    __asm {
        push name;
        mov esi, 0x4D0310;
        call esi;
        add esp, 4;
    }
}
void Cmd_AddCommandInternal(const char* cmdName, void(__cdecl* function)(), cmd_function_s* allocedCmd, int unknown) {

    __asm
    {
        push unknown;
        push allocedCmd;
        push function;
        push cmdName;
        mov esi, 0x428090;
        call esi;
        add esp, 16;
    }

    //return Engine::Tools::Call<void>(0x428090, cmdName, function, allocedCmd, unknown);
}

void Cmd_AddCommand(const char* cmdname, void(__cdecl* function)())
{
    cmd_function_s* cmd = Cmd_FindCommand(cmdname);

    if (cmd)
        return;

    static cmd_function_s cmds[64];
    static std::uint8_t index = 0;
    cmd = &cmds[index++];

    Cmd_AddCommandInternal(cmdname, function, cmd, 1);
}

void Cbuf_AddText(const char* text, int localClientNum)
{
    __asm
    {
        push text;
        push localClientNum;
        mov esi, 0x409B40;
        call esi;
        add esp, 8;
    }
}
