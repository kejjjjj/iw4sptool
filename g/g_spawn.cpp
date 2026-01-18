#include "g_spawn.hpp"
#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "utils/engine.hpp"
#include "sl/sl_string.hpp"
#include "utils/hook.hpp"
#include "cm/cm_entity.hpp"

#include <ranges>

void G_SpawnEntitiesFromString() {
	G_ResetEntityParsePoint();
	Engine::Tools::Call<void>(0x4E09E0);
}

void G_ResetEntityParsePoint() {
	*g_entityBeginParsePoint = cm->mapEnts->entityString;
	*g_entityEndParsePoint = *g_entityBeginParsePoint;
}

int G_ParseSpawnVars(SpawnVar* var){
	return Engine::Tools::Call<int>(0x43B130, var);
}

static bool G_GetSpawnItemIndex(const gentity_s* gent)
{
	const char* classname = 0;

	//G_SpawnString
	Engine::Tools::Call<void>(0x4B7D50, &level->spawnVar, "classname", "", &classname);
	return classname && std::string(classname) == SL_ConvertToString(gent->classname);
}

SpawnVar* G_GetGentitySpawnVars(const gentity_s* gent)
{
	SpawnVar* var = &level->spawnVar;
	auto parsed = false;

	while (parsed = G_ParseSpawnVars(var), parsed) {
		if (G_GetSpawnItemIndex(gent)) {
			//std::print("G_GetSpawnItemIndex(): {}\n", SL_ConvertToString(gent->classname));
			break;
		}
	};

	return parsed ? var : nullptr;
}


void G_ParseEntityFieldsInternal(gentity_s* ent, [[maybe_unused]]int ignoreModel) {
	CStaticEntityFields::EntityKVs kvs;

	for (const auto index : std::views::iota(0, level->spawnVar.numSpawnVars)) {
		const auto [key, value] = std::tie(level->spawnVar.spawnVars[index][0], level->spawnVar.spawnVars[index][1]);
		kvs[key] = value;
	}

	//if (CStaticEntityFields::m_oGentityFields.contains(ent))
	//	std::print("warning: entity {} already added\n", ent->s.number);

	CStaticEntityFields::m_oGentityFields[ent] = kvs;
}

__declspec(naked) void G_ParseEntityFields() {
	__asm {
		push edi;
		push ebx;
		call G_ParseEntityFieldsInternal;
		add esp, 8;
		pop esi;
		pop ebp;
		pop ebx;
		retn;
	}
}
