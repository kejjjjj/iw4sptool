#include "dvar.hpp"
#include "utils/engine.hpp"
dvar_s* Dvar_FindMalleableVar(const char* name) {
	__asm {
		mov edi, name;
		mov esi, 0x633480;
		call esi;
	}
}
const char* Dvar_ValueToString(const dvar_s* dvar, DvarValue value) {
	__asm{
		push value;
		push dvar;
		mov esi, 0x4C5AE0;
		call esi;
		add esp, 8;
	}
}
char* Dvar_DomainToString_Internal(size_t BufferCount, char* outBuffer, char _type, DvarLimits domain, int* outLineCount){
	__asm {
		push ebx;
		push esi;
		push outLineCount;
		lea eax, domain;
		push dword ptr[eax + 4];
		push dword ptr[eax];
		movzx eax, _type;
		push eax;
		mov ebx, outBuffer;
		mov ecx, BufferCount;
		mov esi, 0x632970;
		call esi;
		add esp, 16;
		pop esi;
		pop ebx;
	}
}
std::string Dvar_DomainToString(char type, DvarLimits domain) {
	char buff[1024];
	return Dvar_DomainToString_Internal(1024, buff, type, domain, 0);
}

dvar_s* Dvar_RegisterBool(const char* dvarName, bool value, int flags, const char* description){
	return Engine::Tools::Call<dvar_s*>(0x4866B0, dvarName, value, flags, description);
}
dvar_s* Dvar_RegisterInt(const char* name, int value, int min, int max, int flags, const char* description){
	return Engine::Tools::Call<dvar_s*>(0x423710, name, value, min, max, flags, description);
}
dvar_s* Dvar_RegisterFloat(const char* name, float value, float min, float max, int flags, const char* description){
	return Engine::Tools::Call<dvar_s*>(0x49F700, name, value, min, max, flags, description);
}
dvar_s* Dvar_RegisterEnum(const char* name, const char** valueList, int defaultIndex, int flags, const char* description) {
	return Engine::Tools::Call<dvar_s*>(0x4FB5C0, name, valueList, defaultIndex, flags, description);
}
