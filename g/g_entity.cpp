#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm/cm_cgentities.hpp"
#include "cm/cm_debug.hpp"
#include "cm/cm_entity.hpp"
#include "com/com_channel.hpp"
#include "dvar/dvar.hpp"
#include "g_entity.hpp"
#include "sys/sys_time.hpp"

#include <ranges>
#include <sstream>
#include <string>

void Cmd_ShowEntities_f()
{
	int num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args == 1) {
		if (CGentities::Size() == 0) {
			return Com_Printf(CON_CHANNEL_CONSOLEONLY, "there are no entities to be cleared.. did you intend to use cm_showEntities <classname>?\n");
		}

		if (Dvar_FindMalleableVar("cm_debug")->current.enabled)
			Com_Printf("clearing %i entities from the render queue\n", CGentities::Size());

		return CGentities::ClearThreadSafe();
	}

	std::string filter;
	for(const auto i : std::views::iota(1, num_args)){
		filter += (*(cmd_args->argv[cmd_args->nesting] + i));
		filter += " ";
	}

	CGentities::CM_LoadAllEntitiesToClipMapWithFilter(filter);

	if (Dvar_FindMalleableVar("cm_debug")->current.enabled)
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "adding %i entities to the render queue\n", CGentities::Size());

	Com_Printf(CON_CHANNEL_OBITUARY, "^3Warning: entity information may not be accurate!!!");

}

#include "utils/hook.hpp"
void G_Trigger(gentity_s* self, gentity_s* other){

	if (const auto dvar = Dvar_FindMalleableVar("cm_triggerDisable"); dvar && dvar->current.enabled) {
		return;
	}

	if (const auto dvar = Dvar_FindMalleableVar("cm_triggerShowInfo"); dvar && dvar->current.enabled) {
		if (other->client && self != CGDebugData::currentTrigger.ent) {
			CGDebugData::currentTrigger.ent = self;
			CGDebugData::currentTrigger.m_oEntityFields.clear();

			if (const auto data = CGameEntity::CreateEntity(self)) {
				CGDebugData::currentTrigger.m_oEntityFields = data->m_oEntityFields;
				if (const auto* cm_entityInfo = Dvar_FindMalleableVar("cm_entityInfo")) {
					std::stringstream ss;
					for (const auto& [k, v] : CGDebugData::currentTrigger.m_oEntityFields) {
						if (static_cast<entity_info_type>(cm_entityInfo->current.integer) == entity_info_type::eit_enabled) {
							if (std::ranges::find(nonVerboseInfoStrings, k) == nonVerboseInfoStrings.end())
								continue;
						}

						ss << k << ": " << v << '\n';
					}

					CGDebugData::currentTrigger.m_sDisplayText = ss.str();
					CGDebugData::currentTrigger.m_uTouchTime = Sys_Milliseconds();
				}
			}
		}
	}

	if (const auto f = CStaticHooks::FindByName("G_Trigger")) {
		return f->Call<void>(self, other);
	}
}
