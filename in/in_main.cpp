#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "utils/engine.hpp"

void IN_ActivateMouse(bool active) {
    s_wmv->mouseInitialized = active;
    s_wmv->mouseActive = active;
}

void UI_SetSystemCursorPos(float x, float y) {
    __asm {
        push y;
        push x;
        push 0x191CEF0; //uiInfoArray
        mov esi, 0x4E2030;
        call esi;
        add esp, 12;
    }
}
void IN_ActivateKeyboard(bool active) {
    if (active) {
        Engine::Tools::WriteBytes(0x578C20, "\x51"s);
        Engine::Tools::WriteBytes(0x467EB0, "\x8B"s);
        return;
    }

    Engine::Tools::WriteBytes(0x578C20, "\xC3"s);
    Engine::Tools::WriteBytes(0x467EB0, "\xC3"s);
}