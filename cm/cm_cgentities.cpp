#include "cm_cgentities.hpp"
#include "cm_typedefs.hpp"
#include "cm/cm_clipmap.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "sl/sl_string.hpp"
#include "cm_entity.hpp"
#include "g/g_spawn.hpp"

#include <ranges>

LevelGentities_t CGentities::m_pLevelGentities;
std::mutex CGentities::mtx;

void CGentities::CM_LoadAllEntitiesToClipMapWithFilter(const std::string& filter) {

	CClipMap::ClearTriggersThreadSafe();
	ClearThreadSafe();
	
	std::unique_lock<std::mutex> lock(GetLock());

	const auto filters = CM_TokenizeFilters(filter);

	//reset spawnvars
	//G_ResetEntityParsePoint();

	for (const auto i : std::views::iota(0, level->num_entities)) {
		auto gentity = &level->gentities[i];

		if (!gentity)
			continue;

		const auto classname = SL_ConvertToString(gentity->classname);
		if (!classname || !CM_IsMatchingFilter(filters, classname))
			continue;

		Insert(CGameEntity::CreateEntity(gentity, false));

	}

	ForEach([](GentityPtr_t& p) {
		p->GenerateConnections(m_pLevelGentities);
	});

}
void CGentities::Insert(GentityPtr_t& geom)
{
	m_pLevelGentities.emplace_back(std::move(geom));
}
void CGentities::Insert(GentityPtr_t&& geom)
{
	m_pLevelGentities.emplace_back(std::forward<GentityPtr_t&&>(geom));
}
auto CGentities::begin() { return m_pLevelGentities.begin(); }
auto CGentities::end() { return m_pLevelGentities.end(); }
size_t CGentities::Size() { return m_pLevelGentities.size(); }
void CGentities::Clear() { m_pLevelGentities.clear(); }
void CGentities::ClearThreadSafe() { std::unique_lock<std::mutex> lock(mtx); Clear(); }