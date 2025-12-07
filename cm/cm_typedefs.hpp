#pragma once

#include "utils/vec.hpp"
#include "utils/defs.hpp"
#include "cm_brush.hpp"

#include <memory>
#include <unordered_set>

enum class cm_showcollision_type : char
{
	DISABLED,
	BRUSHES,
	TERRAIN,
	BOTH
};
enum cm_polytype : char
{
	pt_edges,
	pt_polys,
	pt_both
};

enum class cm_geomtype : char
{
	brush,
	terrain,
	model
};

struct cm_triangle
{
	fvec3 a;
	fvec3 b;
	fvec3 c;

	vec4_t plane = {};
	vec4_t color = {};
	const char* material = {};
	bool has_collision = false;

	inline fvec3 get_center() const noexcept {
		return (a + b + c) / 3;
	}

	inline fvec3 get_mins() const noexcept {
		return {
			std::min({a.x, b.x, c.x}),
			std::min({a.y, b.y, c.y}),
			std::min({a.z, b.z, c.z})
		};
	}

	inline fvec3 get_maxs() const noexcept {
		return {
			std::max({a.x, b.x, c.x}),
			std::max({a.y, b.y, c.y}),
			std::max({a.z, b.z, c.z})
		};
	}
};
struct cm_winding
{
	cm_winding() = default;
	cm_winding(const std::vector<fvec3>& p, const fvec3& normal, const fvec3& col);

	inline fvec3 get_mins() const noexcept {
		if (points.empty()) return {};

		float minx = points[0].x, miny = points[0].y, minz = points[0].z;
		for (auto& p : points) {
			minx = std::min(minx, p.x);
			miny = std::min(miny, p.y);
			minz = std::min(minz, p.z);
		}
		return { minx, miny, minz };
	}

	inline fvec3 get_maxs() const noexcept {
		if (points.empty()) return {};

		float maxx = points[0].x, maxy = points[0].y, maxz = points[0].z;
		for (auto& p : points) {
			maxx = std::max(maxx, p.x);
			maxy = std::max(maxy, p.y);
			maxz = std::max(maxz, p.z);
		}
		return { maxx, maxy, maxz };
	}

	std::vector<fvec3> points;
	fvec3 mins;
	fvec3 maxs;
	fvec3 normals;
	vec4_t color;
	bool is_bounce = {};
	bool is_elevator = {};
};

struct cplane_s;

struct cm_renderinfo
{
	cplane_s* frustum_planes = {};
	int num_planes = {};
	float draw_dist = {};
	bool depth_test = {};
	cm_polytype poly_type = {};
	bool only_colliding = {};
	bool only_bounces = {};
	int only_elevators = {};
	float alpha = 0.7f;
};

struct cm_brush;
struct cm_terrain;

struct cm_geometry {
	cm_geometry() = default;
	virtual ~cm_geometry() = default;
	[[nodiscard]] virtual constexpr cm_geomtype Type() const noexcept = 0;

	[[nodiscard]] virtual const cm_brush* AsBrush() const noexcept { return nullptr; }
	[[nodiscard]] virtual const cm_terrain* AsTerrain() const noexcept { return nullptr; }

	[[nodiscard]] virtual bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const { return false; }
	[[nodiscard]] virtual bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, [[maybe_unused]] int& nverts) const {
		return false;
	}

	fvec3 m_origin;
	fvec3 m_mins, m_maxs;
	int m_numVerts = {};
};

struct adjacencyWinding_t;
struct cbrush;

struct cm_brush final : public cm_geometry
{
	friend class CBrushModel;

	cm_brush();
	~cm_brush();

	friend void __cdecl adjacency_winding(adjacencyWinding_t* w, float* points, vec3_t normal);

	[[nodiscard]] cm_geomtype Type() const noexcept override { return cm_geomtype::brush; }
	[[nodiscard]] const cm_brush* AsBrush() const noexcept override { return this; }

	[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
	[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

	cbrush m_brush;
protected:
private:

	std::vector<cm_winding> m_windings; //used for rendering
	std::vector<cm_triangle> m_triangles; //used for exporting
};

using Triangles = std::vector<cm_triangle>;
struct cLeaf_t;
struct cm_terrain final : public cm_geometry
{
	friend class CBrushModel;

	cm_terrain(const cLeaf_t* _leaf, std::vector<Triangles>&& _tris);
	~cm_terrain();

	[[nodiscard]] cm_geomtype Type() const noexcept override { return cm_geomtype::terrain; }
	[[nodiscard]] const cm_terrain* AsTerrain() const noexcept override { return this; }

	[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
	[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

protected:
private:
	const cLeaf_t* m_leaf = 0;
	std::vector<Triangles> m_tris;
};

[[nodiscard]] std::unordered_set<std::string> CM_TokenizeFilters(const std::string& filters);
[[nodiscard]] bool CM_IsMatchingFilter(const std::unordered_set<std::string>& filters, const char* material);

