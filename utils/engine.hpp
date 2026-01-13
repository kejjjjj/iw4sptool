#pragma once
#include <type_traits>
#include <string>
#include <Windows.h>
#include <print>

#include "defs.hpp"
#include <print>
using namespace std::string_literals;

namespace Engine::Tools {

    template<class R, Addr A, class... Args>
    [[maybe_unused]] __declspec(safebuffers) __inline R Call(A addr, CC cc, Args... args) {
        using F = R(*)(Args...);
        auto fn = (F)addr;
        switch (cc) {
            case CC::Stdcall:  return ((R(__stdcall*)(Args...))addr)(std::forward<Args>(args)...);
            case CC::Fastcall: return ((R(__fastcall*)(Args...))addr)(std::forward<Args>(args)...);
            case CC::Thiscall: return ((R(__thiscall*)(Args...))addr)(std::forward<Args>(args)...);
            default:           return fn(std::forward<Args>(args)...);
        }
    }

    template<class R, Addr A, class... Args>
    [[maybe_unused]] __declspec(safebuffers) __inline R Call(A addr, Args... args) {
        return Call<R>(addr, CC::Cdecl, std::forward<Args>(args)...);
    }

    template<Addr A>
    [[maybe_unused]] bool WriteBytes(A dst, const std::string_view& bytes) {
        DWORD oldProtect = {};
        const auto size = bytes.length();
        if (VirtualProtect(reinterpret_cast<void*>(dst), size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy_s(reinterpret_cast<void*>(dst), size, bytes.data(), size);
            return VirtualProtect(reinterpret_cast<void*>(dst), size, oldProtect, &oldProtect) == S_OK;
        }
        return false;
    }

    template<Addr A>
    [[maybe_unused]] auto NOP(A dst) {
        return WriteBytes(dst, "\x90\x90\x90\x90\x90"s);
    }
}