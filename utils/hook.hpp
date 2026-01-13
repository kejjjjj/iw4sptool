#pragma once
#include "engine.hpp"
#include "mh/MinHook.h"
#include <unordered_map>

struct hook_t {
    hook_t(void* _target, void* _detour) : target(_target), detour(_detour) {}
    ~hook_t() = default;

    [[nodiscard]] bool Install() {
        const auto s = MH_CreateHook(target, detour, &trampoline);
        return s == MH_OK && MH_EnableHook(target);
    }

    void Unhook() {
        if (!target)
            return;
        if (enabled)
            Disable();

        MH_RemoveHook(target);

        target = nullptr;
    }

    void Enable() {
        if (enabled)
            return;

        enabled = MH_EnableHook(target) == MH_OK;
    }

    void Disable() {
        if (!enabled)
            return;

        MH_DisableHook(target);
        enabled = false;
    }

    template<typename R, class ... Args>
    [[maybe_unused]] decltype(auto) Call(CC cc = CC::Cdecl, Args... args) const noexcept {
        return Engine::Tools::Call<R>(trampoline, cc, std::forward<Args>(args)...);
    }
    template<typename R, class ... Args>
    [[maybe_unused]] decltype(auto) Call(Args... args) const noexcept {
        return Engine::Tools::Call<R>(trampoline, CC::Cdecl, std::forward<Args>(args)...);
    }
    void* trampoline{};

private:
    void* target{};
    void* detour{};
    bool enabled{};
};


class CStaticHooks {
public:

    static void Initialize() {
        MH_Initialize();
    }
    static void Uninitialize() {

        for (const auto& [name, hook] : m_hooks)
            hook->Unhook();

        MH_Uninitialize();
    }

    static bool Create(const std::string& name, std::uintptr_t target, void* detour) {
        const auto [it, inserted] = m_hooks.insert({ name,
            std::make_unique<hook_t>(reinterpret_cast<void*>(target), reinterpret_cast<void*>(detour)) });

        if (inserted) {
            std::print("install -> ({})\n", name);
            const auto& [_, hook] = *it;
            return hook->Install();
        }

        return inserted;
    }

    static const hook_t* FindByName(const std::string& name) {
        if (!m_hooks.contains(name))
            return nullptr;

        return m_hooks.at(name).get();
    }
private:
    static std::unordered_map<std::string, std::unique_ptr<hook_t>> m_hooks;
};

inline std::unordered_map<std::string, std::unique_ptr<hook_t>> CStaticHooks::m_hooks{};