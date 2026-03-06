#include "bg/bg_pmove.hpp"
#include "cg/cg_local.hpp"
#include "dvar/dvar.hpp"

__declspec(naked) void PM_BounceFixASM()
{
    static auto dvar_name = "pm_bounceFix";

    static const auto jmp1 = 0x46FFB4;
    static const auto jmp2 = 0x46FF7F;

    __asm {

        cmp byte ptr[esp + 0A6h], 0;
        jnz skip_jmp1;

        push eax;
        push dvar_name;
        call Dvar_FindMalleableVar;
        add esp, 4;

        cmp byte ptr [eax + 10h], 0;
        jnz has_bounce_fix;

        cmp dword ptr[esp + 24h], 0;
        jnz skip_jmp2;

    has_bounce_fix:
        pop eax;
        fld dword ptr ds : [0x006E5224];
        fcomp dword ptr[esp + 88h];
        fnstsw ax;
        test ah, 41h;
        jnz skip_jmp1;
        jmp jmp2;

    skip_jmp1:
        jmp jmp1;

    skip_jmp2:
        pop eax;
        jmp jmp2;

    }
}

static void Sys_SnapVector(float* v) {

    if (const auto d = Dvar_FindMalleableVar("pm_fpsFix"); d && !d->current.enabled)
        return;

    int i{};
    float f{};

    f = *v;
    __asm fld f;
    __asm fistp i;
    *v = static_cast<float>(i);
    v++;
    f = *v;
    __asm fld f;
    __asm fistp i;
    *v = static_cast<float>(i);
    v++;
    f = *v;
    __asm fld f;
    __asm fistp i;
    *v = static_cast<float>(i);
}

__declspec(naked) void PM_FpsFixASM() 
{
    __asm {

        push eax;
        lea eax, [ebp + 28h];
        push eax;
        call Sys_SnapVector;
        add esp, 4;
        pop eax;

        pop edi;
        pop esi;
        pop ebp;
        pop ebx;
        add esp, 90h;
        retn;
    }
}