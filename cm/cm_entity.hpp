#pragma once

#include "utils/vec.hpp"
#include "cm/cm_typedefs.hpp"

#include <memory>
#include <vector>
#include <array>
#include <string>

struct gentity_s;
struct cbrush;
struct cLeaf_t;
struct GfxPointVertex;

using GentityPtr_t = std::unique_ptr<class CGameEntity>;
using LevelGentities_t = std::vector<std::unique_ptr<CGameEntity>>;

struct CGentityConnection
{
	gentity_s* const m_pConnection{};
	fvec3 start, end;
};

enum class entity_info_type
{
	eit_disabled,
	eit_enabled,
	eit_verbose
};
inline static constexpr std::array<std::string_view, 6> nonVerboseInfoStrings = {
	"classname", "targetname", "spawnflags",
	"target", "script_noteworthy", "script_flag"
};

struct CStaticEntityFields {
	using EntityKVs = std::unordered_map<std::string, std::string>;
	static std::unordered_map<gentity_s*, EntityKVs> m_oGentityFields;
};

class CGameEntity
{
	friend class CGentities;
	friend void G_Trigger(gentity_s* self, gentity_s* other);
public:

	CGameEntity(gentity_s* const g);
	virtual ~CGameEntity();

	[[nodiscard]] static std::unique_ptr<CGameEntity> CreateEntity(gentity_s* const g, bool makeAbstract);

	[[nodiscard]] virtual bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const { return false; }
	[[nodiscard]] virtual bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, [[maybe_unused]] int& nverts) const { return false; }
	void RB_RenderConnections(const cm_renderinfo& info, int& nverts) const;

	virtual void CG_Render2D(float drawDist, entity_info_type entType) const;


	[[nodiscard]] constexpr gentity_s* GetOwner() const noexcept { return m_pOwner; }
	[[nodiscard]] virtual int GetNumVerts() const noexcept { return 0; }

protected:
	[[nodiscard]] bool IsBrushModel() const noexcept;

	fvec3& m_vecOrigin;
	fvec3& m_vecAngles;

	mutable fvec3 m_vecOldOrigin;
	mutable fvec3 m_vecOldAngles;

	CStaticEntityFields::EntityKVs m_oEntityFields;
private:
	[[nodiscard]] constexpr virtual bool HasBrushModels() const noexcept { return false; }

	void ParseEntityFields();
	void GenerateConnections(const LevelGentities_t& gentities);

	gentity_s* const m_pOwner{};

	std::vector<CGentityConnection> m_oGentityConnections;
	mutable std::vector<GfxPointVertex> m_oGentityConnectionVertices;

};

class CTrigger final : public CGameEntity
{
public:
	CTrigger(gentity_s* const g);
	~CTrigger();

	[[nodiscard]] bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const override;
	[[nodiscard]] bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, int& nverts) const override;

	//since it's an AABB
	[[nodiscard]] int GetNumVerts() const noexcept override { return 12; }

private:
	void CreateBrush(const struct Bounds& bounds);

	fvec3 m_vecMins;
	fvec3 m_vecMaxs;

	//std::unique_ptr<cm_geometry> m_pBrush;
};
class CBrushModel final : public CGameEntity
{
public:
	CBrushModel(gentity_s* const g);
	~CBrushModel();

	[[nodiscard]] bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const override;
	[[nodiscard]] bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, int& nverts) const override;
	[[nodiscard]] constexpr bool HasBrushModels() const noexcept override { return !m_oBrushModels.empty(); }

	struct CIndividualBrushModel
	{
		CIndividualBrushModel(gentity_s* const g);
		virtual ~CIndividualBrushModel();

		[[nodiscard]] virtual bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const;
		[[nodiscard]] virtual bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const;

		[[nodiscard]] virtual int GetNumVerts() const noexcept = 0;
		[[nodiscard]] virtual const cm_geometry& GetSource() const noexcept = 0;
		virtual void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) = 0;

	protected:
		[[nodiscard]] virtual fvec3 GetCenter() const noexcept;

		gentity_s* const m_pOwner{};

	};

	struct CBrush final : public CIndividualBrushModel
	{
		CBrush(gentity_s* const g, const cbrush& brush);
		~CBrush();

		[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
		[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

		void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) override;
		[[nodiscard]] const cm_geometry& GetSource() const noexcept override;

		[[nodiscard]] int GetNumVerts() const noexcept override { return m_oCurrentGeometry.m_numVerts; }

	private:
		cm_brush m_oOriginalGeometry;
		cm_brush m_oCurrentGeometry;
		const cbrush m_pSourceBrush;
	};

	struct CTerrain final : public CIndividualBrushModel
	{
		CTerrain(gentity_s* const g, const cLeaf_t* const leaf, const cm_terrain& terrain);
		~CTerrain();

		[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
		[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

		void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) override;
		[[nodiscard]] const cm_geometry& GetSource() const noexcept override;

		[[nodiscard]] int GetNumVerts() const noexcept override { return m_oCurrentGeometry.m_numVerts; }

	private:
		cm_terrain m_oOriginalGeometry;
		cm_terrain m_oCurrentGeometry;
		const cLeaf_t* const m_pSourceLeaf = {};
	};

private:
	std::vector<std::unique_ptr<CIndividualBrushModel>> m_oBrushModels;

};
