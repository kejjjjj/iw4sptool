#pragma once
#include <string>

struct $BFBB53559BEAC4289F32B924847E59CB
{
	int stringCount;
	const char** strings;
};
struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
{
	int min;
	int max;
};
struct $251C2428A496074035CACA7AAF3D55BD
{
	float min;
	float max;
};

union DvarLimits
{
	$BFBB53559BEAC4289F32B924847E59CB enumeration;
	$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
	$251C2428A496074035CACA7AAF3D55BD value;
	$251C2428A496074035CACA7AAF3D55BD vector;
};

union DvarValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	float value;
	float vector[4];
	const char* string;
	unsigned __int8 color[4];
};
enum dvar_flags : unsigned short
{
	none = 0x0,
	saved = 0x1,
	user_info = 0x2, // sent to server on connect or change
	server_info = 0x4, // sent in response to front end requests
	replicated = 0x8,
	write_protected = 0x10,
	latched = 0x20,
	read_only = 0x40,
	cheat_protected = 0x80,
	temp = 0x100,
	no_restart = 0x400, // do not clear when a cvar_restart is issued
	user_created = 0x4000, // created by a set command
};
enum class dvar_type : char
{
	boolean = 0,
	value = 1,
	vec2 = 2,
	vec3 = 3,
	vec4 = 4,
	integer = 5,
	enumeration = 6,
	string = 7,
	color = 8,
	rgb = 9 // Color without alpha
};
struct dvar_s {
	const char* name;
	const char* description;
	unsigned __int32 flags;
	unsigned __int8 type;
	bool modified;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarLimits domain;
	bool(__cdecl* domainFunc)(dvar_s*, DvarValue);
	dvar_s* hashNext;
};

[[nodiscard]] dvar_s* Dvar_FindMalleableVar(const char* name);
[[nodiscard]] const char* Dvar_ValueToString(const dvar_s* dvar, DvarValue value);
[[nodiscard]] char* Dvar_DomainToString_Internal(size_t BufferCount, char* outBuffer, char type, DvarLimits domain, int* outLineCount);
[[nodiscard]] std::string Dvar_DomainToString(char type, DvarLimits domain);

[[maybe_unused]] dvar_s* Dvar_RegisterBool(const char* dvarName, bool value, int flags, const char* description);
[[maybe_unused]] dvar_s* Dvar_RegisterInt(const char* name, int value, int min, int max, int flags, const char* description);
[[maybe_unused]] dvar_s* Dvar_RegisterFloat(const char* name, float value, float min, float max, int flags, const char* description);
[[maybe_unused]] dvar_s* Dvar_RegisterEnum(const char* name, const char** valueList, int defaultIndex, int flags, const char* description);
