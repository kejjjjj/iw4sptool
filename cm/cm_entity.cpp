#include "cm_entity.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm/cm_brush.hpp"
#include "com/com_vector.hpp"
#include "cm/cm_terrain.hpp"
#include "g/g_spawn.hpp"
#include "cm_debug.hpp"
#include "r/backend/rb_endscene.hpp"
#include "r/r_drawtools.hpp"
#include "sl/sl_string.hpp"
#include "utils/engine.hpp"
#include "cm/cm_renderer.hpp"
#include "cm/cm_clipmap.hpp"

#include <cassert>
#include <ranges>

std::unordered_map<gentity_s*, CStaticEntityFields::EntityKVs> CStaticEntityFields::m_oGentityFields;

CGameEntity::CGameEntity(gentity_s* const g) :
	m_pOwner(g),
	m_vecOrigin((fvec3&)g->r.currentOrigin),
	m_vecAngles((fvec3&)g->r.currentAngles) {
	assert(m_pOwner != nullptr);
	ParseEntityFields();
}
CGameEntity::~CGameEntity() = default;

bool CGameEntity::IsBrushModel() const noexcept
{
	assert(m_pOwner != nullptr);
	return m_pOwner->classname == *scr_const_script_brushmodel;
}
void CGameEntity::ParseEntityFields()
{
	if (!CStaticEntityFields::m_oGentityFields.contains(m_pOwner)) {
		return;
	}

	m_oEntityFields = CStaticEntityFields::m_oGentityFields.at(m_pOwner);
}
void CGameEntity::GenerateConnections(const LevelGentities_t& lgentities)
{
	if (!m_oEntityFields.contains("target"))
		return;

	const auto& target = m_oEntityFields["target"];

	for (const auto& gent : lgentities) {

		if (gent->m_pOwner == m_pOwner || !gent->m_oEntityFields.contains("targetname"))
			continue;

		const auto& targetname = gent->m_oEntityFields["targetname"];

		if (target == targetname)
			m_oGentityConnections.push_back({ gent->m_pOwner, m_pOwner->r.currentOrigin, gent->m_pOwner->r.currentOrigin });
	}

	if (!m_oGentityConnections.empty())
		m_oGentityConnectionVertices.resize(m_oGentityConnections.size() * 2);
}

std::unique_ptr<CGameEntity> CGameEntity::CreateEntity(gentity_s* const g, bool makeAbstract) {

	if(makeAbstract)
		return std::make_unique<CGameEntity>(g);


	if (CStaticEntityFields::m_oGentityFields.contains(g)) {

		const auto& fields = CStaticEntityFields::m_oGentityFields.at(g);
		
		if(fields.contains("classname")){
			const auto& classname = fields.at("classname");

			if (classname == "script_brushmodel") {
				auto&& p = std::make_unique<CBrushModel>(g);

				if (p->HasBrushModels())
					return p;

			} else if (classname.starts_with("trigger")) {
				return std::make_unique<CTrigger>(g);
			}
		}
	}

	return std::make_unique<CGameEntity>(g);
}
void CGameEntity::RB_RenderConnections([[maybe_unused]] const cm_renderinfo& info, [[maybe_unused]] int& nverts) const
{
	if (m_oGentityConnections.empty())
		return;

	const vec4_t RED = { 1, 0, 0, info.alpha };

	for (const auto& connection : m_oGentityConnections) {
		nverts = RB_AddDebugLine(CGDebugData::g_debugPolyVerts, info.depth_test, connection.start.As<vec_t*>(), connection.end.As<vec_t*>(), RED, nverts);
	}
}


void CGameEntity::CG_Render2D(float drawDist, entity_info_type entType) const
{
	if (entType == entity_info_type::eit_disabled)
		return;

	const auto distance = m_vecOrigin.dist(viewpos);

	if (distance > drawDist || m_oEntityFields.empty())
		return;


	fvec3 center = {
		m_pOwner->r.currentOrigin[0],
		m_pOwner->r.currentOrigin[1],
		m_pOwner->r.currentOrigin[2] /*+ (m_pOwner->r.maxs[2] - m_pOwner->r.mins[2]) / 2 */
	};

	std::string buff;
	for (const auto& [key, value] : m_oEntityFields) {
		if (entType == entity_info_type::eit_enabled) {
			if (std::ranges::find(nonVerboseInfoStrings, key) == nonVerboseInfoStrings.end())
				continue;
		}

		buff += std::string(key) + ": " + value + "\n";
	}

	if (buff.empty())
		return;

	if (auto op = WorldToScreen(center)) {
		const float scale = R_ScaleByDistance(distance) * 0.15f;
		R_DrawTextWithEffects(buff, "fonts/bigdevfont", op->x, op->y, scale, scale, 0.f, vec4_t{ 1,1,1,1 }, 5, vec4_t{ 1,0,0,0 });
	}

}


CTrigger::CTrigger(gentity_s* const g) : CGameEntity(g){
	CreateBrush(g->r.absBox);

	m_vecMins = fvec3(g->r.absBox.midPoint) - fvec3(g->r.absBox.halfSize);
	m_vecMaxs = fvec3(g->r.absBox.midPoint) + fvec3(g->r.absBox.halfSize);
}
CTrigger::~CTrigger() = default;

void CTrigger::CreateBrush(const Bounds& bounds)
{
	for (const auto i : std::views::iota(0u, cm->numBrushes)) {
		if (!cm->brushes[i].numsides) {
			cbrush_t fakeBrush = cm->brushes[i];
			cbrush aabb_brush(&fakeBrush, &bounds, MASK_PLAYERSOLID);

			if (auto&& brush = CM_GetBrushPoints(aabb_brush, vec3_t{ 1.f, 0.f, 0.f})) {
				//std::print("insert brush {}\n", CClipMap::Size());
				CClipMap::InsertTrigger(std::move(brush));
			}

			return;
		}
	}

}

bool CTrigger::RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const {
	//return m_pBrush->RB_MakeInteriorsRenderable(info);
	return false;
}
bool CTrigger::RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, [[maybe_unused]] int& nverts) const {
	//return m_pBrush->RB_MakeOutlinesRenderable(info, nverts);
	return false;
}

CBrushModel::CBrushModel(gentity_s* const g) : CGameEntity(g) {
	assert(IsBrushModel());

	const auto leaf = &cm->cmodels[g->s.index.brushmodel].leaf;
	const auto& leafBrushNode = cm->leafbrushNodes[leaf->leafBrushNode];
	const auto numBrushes = leafBrushNode.leafBrushCount;

	//brush
	if (numBrushes > 0) {
		for (const auto brushIndex : std::views::iota(0, numBrushes)) {
			const auto brushWorldIndex = leafBrushNode.data.leaf.brushes[brushIndex];
			auto&& temp = cbrush(&cm->brushes[brushWorldIndex], &cm->brushBounds[brushWorldIndex], cm->brushContents[brushWorldIndex]);

			m_oBrushModels.emplace_back(std::make_unique<CBrush>(g, temp));
		}
		return;
	}

	if (const auto terrain = CM_TerrainFromLeaf(leaf, { "all" }, vec4_t{1, 1, 0, 0.8f}))
		m_oBrushModels.emplace_back(std::make_unique<CTerrain>(g, leaf, *terrain));

}
CBrushModel::~CBrushModel() = default;

bool CBrushModel::RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const
{
	bool state = false;

	for (auto& bmodel : m_oBrushModels) {

		if (m_vecOrigin != m_vecOldOrigin || m_vecAngles != m_vecOldAngles)
			bmodel->OnPositionChanged(m_vecOrigin, m_vecAngles);

		if (bmodel->RB_MakeInteriorsRenderable(info))
			state = true;
	}

	m_vecOldOrigin = m_vecOrigin;
	m_vecOldAngles = m_vecAngles;

	return state;
}
bool CBrushModel::RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, int& nverts) const
{
	auto state = false;

	for (auto& bmodel : m_oBrushModels) {

		if (m_vecOrigin != m_vecOldOrigin || m_vecAngles != m_vecOldAngles)
			bmodel->OnPositionChanged(m_vecOrigin, m_vecAngles);

		if (bmodel->RB_MakeOutlinesRenderable(info, nverts))
			state = true;
	}

	m_vecOldOrigin = m_vecOrigin;
	m_vecOldAngles = m_vecAngles;

	return state;
}

CBrushModel::CIndividualBrushModel::CIndividualBrushModel(gentity_s* const g) : m_pOwner(g) { assert(g != nullptr); }
CBrushModel::CIndividualBrushModel::~CIndividualBrushModel() = default;

bool CBrushModel::CIndividualBrushModel::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const {
	return GetSource().RB_MakeInteriorsRenderable(info);
}

bool CBrushModel::CIndividualBrushModel::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const {
	return GetSource().RB_MakeOutlinesRenderable(info, nverts);
}

fvec3 CBrushModel::CIndividualBrushModel::GetCenter() const noexcept {
	assert(m_pOwner != nullptr);
	return m_pOwner->r.currentOrigin;
}
CBrushModel::CBrush::CBrush(gentity_s* const g, const cbrush& brush) : CIndividualBrushModel(g), m_pSourceBrush(brush)
{
	assert(m_pSourceBrush != nullptr);

	const vec3_t col = { 1.f, 0.f, 0.f };

	//questionable for sure!
	m_oOriginalGeometry = *dynamic_cast<cm_brush*>(&*CM_GetBrushPoints(m_pSourceBrush, col));
	m_oCurrentGeometry = m_oOriginalGeometry;

	OnPositionChanged(g->r.currentOrigin, g->r.currentAngles);


}
CBrushModel::CBrush::~CBrush() = default;
#include <print>
void CBrushModel::CBrush::OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles)
{
	m_oCurrentGeometry = m_oOriginalGeometry;

	for (auto& winding : m_oCurrentGeometry.m_windings) {
		for (auto& point : winding.points) {
			point = VectorRotate(point + newOrigin, newAngles, m_oCurrentGeometry.m_origin);
		}
	}

	m_oCurrentGeometry.m_mins = fvec3(m_pOwner->r.currentOrigin) - fvec3(m_pOwner->r.box.halfSize);
	m_oCurrentGeometry.m_maxs = fvec3(m_pOwner->r.currentOrigin) + fvec3(m_pOwner->r.box.halfSize);
	m_oCurrentGeometry.m_origin = m_pOwner->r.currentOrigin;
}

const cm_geometry& CBrushModel::CBrush::GetSource() const noexcept
{
	return m_oCurrentGeometry;
}

bool CBrushModel::CBrush::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(viewpos) > info.draw_dist)
		return false;

	//const auto center = GetCenter();

	//if (BoundsInView(center + m_oCurrentGeometry.m_mins, center + m_oCurrentGeometry.m_maxs, info.frustum_planes, info.num_planes) == false)
	//	return false;

	return m_oCurrentGeometry.RB_MakeInteriorsRenderable(info);
}
bool CBrushModel::CBrush::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(viewpos) > info.draw_dist)
		return false;

	//const auto center = GetCenter();

	//if (BoundsInView(center + m_oCurrentGeometry.m_mins, center + m_oCurrentGeometry.m_maxs, info.frustum_planes, info.num_planes) == false)
	//	return false;

	return m_oCurrentGeometry.RB_MakeOutlinesRenderable(info, nverts);
}

CBrushModel::CTerrain::CTerrain(gentity_s* const g, const cLeaf_t* const leaf, const cm_terrain& terrain)
	: CIndividualBrushModel(g), m_pSourceLeaf(leaf), m_oOriginalGeometry(terrain), m_oCurrentGeometry(terrain) {
	OnPositionChanged(g->r.currentOrigin, g->r.currentAngles);
}

CBrushModel::CTerrain::~CTerrain() = default;

void CBrushModel::CTerrain::OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles)
{
	m_oCurrentGeometry = m_oOriginalGeometry;

	const auto center = GetCenter();

	for (auto& tris : m_oCurrentGeometry.m_tris) {
		for (auto& tri : tris) {
			tri.a = VectorRotate(tri.a + newOrigin, newAngles, center);
			tri.b = VectorRotate(tri.b + newOrigin, newAngles, center);
			tri.c = VectorRotate(tri.c + newOrigin, newAngles, center);
		}

	}

	m_oCurrentGeometry.m_mins = fvec3(m_pOwner->r.box.midPoint) - fvec3(m_pOwner->r.box.halfSize);
	m_oCurrentGeometry.m_maxs = fvec3(m_pOwner->r.box.midPoint) + fvec3(m_pOwner->r.box.halfSize);
	m_oCurrentGeometry.m_origin = m_pOwner->r.currentOrigin;

}
const cm_geometry& CBrushModel::CTerrain::GetSource() const noexcept {
	return m_oCurrentGeometry;
}
bool CBrushModel::CTerrain::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(viewpos) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	return m_oCurrentGeometry.RB_MakeInteriorsRenderable(info);
}
bool CBrushModel::CTerrain::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(viewpos) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	return m_oCurrentGeometry.RB_MakeOutlinesRenderable(info, nverts);
}