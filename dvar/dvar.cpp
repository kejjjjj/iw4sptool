#include "dvar.hpp"

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