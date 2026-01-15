#include "cm_typedefs.hpp"
#include "cm_renderer.hpp"
#include "cm_brush.hpp"
#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "r/backend/rb_endscene.hpp"
#include "cm_terrain.hpp"


cm_winding::cm_winding(const std::vector<fvec3>& p, const fvec3& normal, [[maybe_unused]] const fvec3& col) : points(p), normals(normal)
{
	is_bounce = normal[2] >= 0.3f && normal[2] <= 0.7f;
	is_elevator = std::fabs(normal[0]) == 1.f || std::fabs(normal[1]) == 1.f;
	normals = normal;
	VectorCopy(col, color);

	//if (rgp && rgp->world) {
	//	fvec3 new_color = SetSurfaceBrightness(col, normal, rgp->world->sunParse.angles);
	//	VectorCopy(new_color, color);
	//} else {
	//	VectorCopy(col, color);
	//}

	color[3] = 0.7f;

	mins = get_mins();
	maxs = get_maxs();
}
cm_brush::cm_brush() = default;
cm_brush::~cm_brush() = default;

bool cm_brush::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (info.only_colliding && (m_brush.content & MASK_PLAYERSOLID) == 0)
		return false;

	if (m_origin.dist(viewpos) > info.draw_dist)
		return false;

	if (!CM_BoundsInView(m_mins, m_maxs, info.frustum_planes, info.num_planes))
		return false;

	bool state = false;
	for (const auto& w : m_windings) {

		if (RB_CheckTessOverflow(m_numVerts, 3 * (m_numVerts - 2)))
			RB_TessOverflow(true, info.depth_test);

		if (info.only_bounces && w.is_bounce == false)
			continue;

		if (info.only_elevators && w.is_elevator == false)
			continue;

		vec4_t c = { 0,1,0, info.alpha };

		//only use sunlight when there are no outlines in a polygon
		//if (info.poly_type == pt_polys) {
		c[0] = w.color[0];
		c[1] = w.color[1];
		c[2] = w.color[2];
		//}

		if (info.only_bounces) {
			float n = w.normals[2];

			if (n > 0.7f || n < 0.3f)
				n = 0.f;
			else
				n = 1.f - (n - 0.3f) / (0.7f - 0.3f);

			c[0] = 1.f - n;
			c[1] = n;
			c[2] = 0.f;
		}

		CM_MakeInteriorRenderable(w.points, c);
		state = true;
	}

	return state;
}


bool cm_brush::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{

	if (info.only_colliding && (m_brush.content & MASK_PLAYERSOLID) == 0)
		return false;

	if (m_origin.dist(viewpos) > info.draw_dist)
		return false;

	if (!CM_BoundsInView(m_mins, m_maxs, info.frustum_planes, info.num_planes))
		return false;

	for (const auto& w : m_windings) {

		if (info.only_bounces && w.is_bounce == false)
			continue;

		if (info.only_elevators && w.is_elevator == false)
			continue;

		vec4_t c = { w.color[0],w.color[1],w.color[2],info.alpha };

		if (info.only_bounces) {
			float n = w.normals[2];

			if (n > 0.7f || n < 0.3f)
				n = 0.f;
			else
				n = 1.f - (n - 0.3f) / (0.7f - 0.3f);

			c[0] = 1.f - n;
			c[1] = n;
			c[2] = 0.f;
		}

		nverts = CM_MakeOutlinesRenderable(w.points, c, info.depth_test, nverts);
	}
	return true;
}

cm_terrain::cm_terrain(const cLeaf_t* _leaf, std::vector<Triangles>&& _tris)
	: m_leaf(_leaf), m_tris(std::move(_tris)) {
	m_numVerts = 3;
}
cm_terrain::~cm_terrain() = default;

bool cm_terrain::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (info.only_elevators)
		return false;

	std::vector<fvec3> points(3);

	bool state = false;

	for (const auto& tris : m_tris) {

		for (const auto& tri : tris) {
			if (info.only_colliding && tri.has_collision == false)
				continue;

			if (info.only_bounces && (tri.plane[2] < 0.3f || tri.plane[2] > 0.7f))
				continue;

			if (tri.a.dist(viewpos) > info.draw_dist)
				continue;

			if (!CM_TriangleInView(&tri, info.frustum_planes, info.num_planes))
				continue;

			if (RB_CheckTessOverflow(m_numVerts, 3 * (m_numVerts - 2)))
				RB_TessOverflow(true, info.depth_test);

			vec4_t c =
			{
				tri.color[0],
				tri.color[1],
				tri.color[2],
				info.alpha
			};

			if (info.only_bounces) {
				float n = tri.plane[2];

				if (n > 0.7f || n < 0.3f)
					n = 0.f;
				else
					n = 1.f - (n - 0.3f) / (0.7f - 0.3f);

				c[0] = 1.f - n;
				c[1] = n;
				c[2] = 0.f;
			}


			points[0] = (tri.a);
			points[1] = (tri.b);
			points[2] = (tri.c);

			CM_MakeInteriorRenderable(points, c);

			state = true;
		}
	}

	return state;
}
bool cm_terrain::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{

	if (info.only_elevators)
		return false;

	std::vector<fvec3> points(3);

	bool state = false;
	for (const auto& tris : m_tris) {

		for (const auto& tri : tris) {
			if (tri.has_collision == false && info.only_colliding)
				continue;

			if ((tri.plane[2] < 0.3f || tri.plane[2] > 0.7f) && info.only_bounces)
				continue;

			if (tri.a.dist(viewpos) > info.draw_dist)
				continue;

			if (!CM_TriangleInView(&tri, info.frustum_planes, info.num_planes))
				continue;

			if (RB_CheckTessOverflow(m_numVerts, 3 * (m_numVerts - 2)))
				RB_TessOverflow(true, info.depth_test);

			vec4_t c =
			{
				tri.color[0],
				tri.color[1],
				tri.color[2],
				info.alpha
			};

			if (info.only_bounces) {
				float n = tri.plane[2];

				if (n > 0.7f || n < 0.3f)
					n = 0.f;
				else
					n = 1.f - (n - 0.3f) / (0.7f - 0.3f);

				c[0] = 1.f - n;
				c[1] = n;
				c[2] = 0.f;
			}

			points[0] = (tri.a);
			points[1] = (tri.b);
			points[2] = (tri.c);

			nverts = CM_MakeOutlinesRenderable(points, c, info.depth_test, nverts);
			state = true;
		}
	}

	return state;
}
#include <sstream>
std::unordered_set<std::string> CM_TokenizeFilters(const std::string& filters)
{
	std::unordered_set<std::string> tokens;
	std::stringstream stream(filters);
	std::string token;

	while (stream >> token) {
		tokens.insert(token);
	}

	return tokens;
}
bool CM_IsMatchingFilter(const std::unordered_set<std::string>& filters, const char* material)
{
	std::string strMat = material;

	for (const auto& filter : filters) {
		if (filter == "all" || strMat.find(filter) != std::string::npos)
			return true;
	}

	return false;
}