#pragma once

#include "utils/defs.hpp"
#include "utils/vec.hpp"

#include <mutex>
#include <memory>

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
	static void ClearAllOfType(const cm_geomtype t);
	static auto GetAllOfType(const cm_geomtype t);

	static auto begin() { return m_pLevelGeometry.begin(); }
	static auto end() { return m_pLevelGeometry.end(); }
	static size_t Size() { return m_pLevelGeometry.size(); }
	static void Clear() { m_pLevelGeometry.clear(); m_pWipGeometry.reset(); }
	static void ClearThreadSafe() { std::unique_lock<std::mutex> lock(mtx); Clear(); }

	inline static auto& GetLock() { return mtx; }

	template<typename Func>
	static void ForEach(Func func) {

		for (auto& geo : m_pLevelGeometry)
			func(geo);

	}

private:
	static GeometryPtr_t m_pWipGeometry;
	static fvec3 m_vecWipGeometryColor;
	static LevelGeometry_t m_pLevelGeometry;

	static std::mutex mtx;
};
