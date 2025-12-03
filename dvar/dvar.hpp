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
