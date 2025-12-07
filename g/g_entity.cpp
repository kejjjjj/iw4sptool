#include "g_entity.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm/cm_cgentities.hpp"
#include "com/com_channel.hpp"
#include "dvar/dvar.hpp"

#include <string>
#include <ranges>

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

}

#include "utils/hook.hpp"
char InitTrigger([[maybe_unused]]struct gentity_s* g)
{
	gentity_s* p{};
	__asm mov p, esi;
	if (const auto f = CStaticHooks::FindByName("InitTrigger")) {
		const auto _ptr = f->trampoline;

		__asm {
			mov esi, p;
			call _ptr;
			retn;
		}

	}
	__asm {
		mov al, 0;
		retn;
	}
}
