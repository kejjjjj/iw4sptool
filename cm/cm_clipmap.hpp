#pragma once

#include "utils/defs.hpp"
#include "utils/vec.hpp"

#include <mutex>
#include <memory>
#include <ranges>

struct cm_geometry;
struct cbrush;
struct adjacencyWinding_t;

enum class cm_geomtype : char;

using GeometryPtr_t = std::unique_ptr<cm_geometry>;
using LevelGeometry_t = std::vector<GeometryPtr_t>;

class CClipMap
{
	NONCOPYABLE(CClipMap);
	friend std::unique_ptr<cm_geometry> CM_GetBrushPoints(const cbrush& brush, const fvec3& poly_col);
	friend void __cdecl adjacency_winding(adjacencyWinding_t* w, float* points, vec3_t normal);
	friend void CM_LoadBrushWindingsToClipMap(const cbrush& brush);
public:

	static void Insert(GeometryPtr_t& geom);
	static void Insert(GeometryPtr_t&& geom);
	static void InsertTrigger(GeometryPtr_t&& geom);

	static void ClearAllOfType(const cm_geomtype t);
	static auto GetAllOfType(const cm_geomtype t);

	//static auto begin() { return m_pLevelGeometry.begin(); }
	//static auto end() { return m_pLevelGeometry.end(); }
	static size_t Size() { return m_pLevelGeometry.size() + m_pLevelTriggerGeometry.size(); }
	static void Clear() { m_pLevelGeometry.clear(); m_pLevelTriggerGeometry.clear(); m_pWipGeometry.reset(); }
	static void ClearTriggers() { m_pLevelTriggerGeometry.clear(); }

	static void ClearThreadSafe() { std::unique_lock<std::mutex> lock(mtx); Clear(); }
	static void ClearClipMapThreadSafe() { std::unique_lock<std::mutex> lock(mtx); m_pLevelGeometry.clear(); m_pWipGeometry.reset(); }
	static void ClearTriggersThreadSafe() { std::unique_lock<std::mutex> lock(mtx); ClearTriggers(); }

	inline static auto& GetLock() { return mtx; }

	template<typename Func>
	static void ForEach(Func func) {

		for([[maybe_unused]] const auto i : std::views::iota(0u, m_pLevelGeometry.size()))
			func(m_pLevelGeometry[i]);

		for ([[maybe_unused]] const auto i : std::views::iota(0u, m_pLevelTriggerGeometry.size()))
			func(m_pLevelTriggerGeometry[i]);
	}

private:
	static GeometryPtr_t m_pWipGeometry;
	static fvec3 m_vecWipGeometryColor;
	static LevelGeometry_t m_pLevelGeometry;
	static LevelGeometry_t m_pLevelTriggerGeometry;

	static std::mutex mtx;
};
