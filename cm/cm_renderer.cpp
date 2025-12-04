#include "cm_renderer.hpp"
#include "utils/vec.hpp"
#include "r/backend/rb_endscene.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "utils/defs.hpp"
#include "cm/cm_clipmap.hpp"
#include "cm/cm_typedefs.hpp"
#include "cm_debug.hpp"
#include "com/com_channel.hpp"
#include "com/com_vector.hpp"
#include "dvar/dvar.hpp"

#include <algorithm>
#include <ranges>
#include <print>
#include <windows.h>

void CM_ShowCollision()
{
	if (CClipMap::Size() == 0)
		return;

	cplane_s frustum_planes[6];
	CreateFrustumPlanes(frustum_planes);

	cm_renderinfo render_info =
	{
		.frustum_planes = frustum_planes,
		.num_planes = 5,
		.draw_dist = Dvar_FindMalleableVar("cm_showCollisionDist")->current.value,
		.depth_test = Dvar_FindMalleableVar("cm_depthTest")->current.enabled,
		.poly_type = static_cast<cm_polytype>(Dvar_FindMalleableVar("cm_showCollisionPolyType")->current.integer),
		.only_colliding = Dvar_FindMalleableVar("cm_ignoreNonColliding")->current.enabled,
		.only_bounces = Dvar_FindMalleableVar("cm_onlyBounces")->current.enabled,
		.only_elevators = Dvar_FindMalleableVar("cm_onlyElevators")->current.enabled,
		.alpha = Dvar_FindMalleableVar("cm_showCollisionPolyAlpha")->current.value
	};

	const auto collisionType = static_cast<cm_showcollision_type>(Dvar_FindMalleableVar("cm_showCollision")->current.integer);
	const bool brush_allowed = collisionType == cm_showcollision_type::BRUSHES || collisionType == cm_showcollision_type::BOTH;
	const bool terrain_allowed = collisionType == cm_showcollision_type::TERRAIN || collisionType == cm_showcollision_type::BOTH;

	CGDebugData::tessVerts = 0;
	CGDebugData::tessIndices = 0;
	
	if (render_info.poly_type != pt_edges) {
		std::unique_lock<std::mutex> lock(CClipMap::GetLock());

		RB_BeginSurfaceInternal(true, render_info.depth_test);

		CClipMap::ForEach([&](const GeometryPtr_t& poly) {

			if (RB_CheckTessOverflow(poly->m_numVerts, 3 * (poly->m_numVerts - 2)))
				RB_TessOverflow(true, render_info.depth_test);

			if (poly->Type() == cm_geomtype::brush && brush_allowed
				|| poly->Type() == cm_geomtype::terrain && terrain_allowed
				|| poly->Type() == cm_geomtype::model) {
				if (poly->RB_MakeInteriorsRenderable(render_info)) {
					CGDebugData::tessVerts += poly->m_numVerts;
					CGDebugData::tessIndices += 3 * (poly->m_numVerts - 2);
				}
			}

			});

		RB_EndTessSurface();
	}

	if (render_info.poly_type != pt_polys) {

		int vert_count = 0;
		CClipMap::ForEach([&](const GeometryPtr_t& poly) {
			if (poly->Type() == cm_geomtype::brush && brush_allowed
				|| poly->Type() == cm_geomtype::terrain && terrain_allowed
				|| poly->Type() == cm_geomtype::model) {
				if (poly->RB_MakeOutlinesRenderable(render_info, vert_count)) {
					CGDebugData::tessVerts += poly->m_numVerts;
					CGDebugData::tessIndices += 3 * (poly->m_numVerts - 2);
				}
			}
		});

		if (vert_count)
			RB_DrawLines3D(vert_count / 2, 1, CGDebugData::g_debugPolyVerts, render_info.depth_test);
	}


	//if (CGDebugData::tessVerts) {
	//	std::print("verts: {}\n", (int)CGDebugData::tessVerts);
	//	RB_DrawLines3D(vert_count / 2, 1, CGDebugData::g_debugPolyVerts, render_info.depth_test);
	//}
}
void CM_MakeInteriorRenderable(const std::vector<fvec3>& points, const float* _color) {
	GfxColor color;
	std::size_t idx{};

	auto n_points = points.size();

	R_ConvertColorToBytes(_color, &color);

	for (idx = 0; idx < n_points; ++idx) {
		RB_SetPolyVertice(points[idx].As<float*>(), &color, tess->vertexCount + idx, idx);
	}

	for (idx = 2; idx < n_points; ++idx) {
		tess->indices[tess->indexCount + 0] = static_cast<short>(tess->vertexCount);
		tess->indices[tess->indexCount + 1] = static_cast<short>(idx + tess->vertexCount);
		tess->indices[tess->indexCount + 2] = static_cast<short>(idx + tess->vertexCount - 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += n_points;
}
int CM_MakeOutlinesRenderable(const std::vector<fvec3>& points, const float* color, bool depthTest, int nverts)
{
	auto n_points = points.size();
	auto vert_index_prev = n_points - 1u;

	for (auto i : std::views::iota(0u, n_points)) {

		nverts = RB_AddDebugLine(CGDebugData::g_debugPolyVerts, depthTest,
			points[vert_index_prev].As<float*>(), points[i].As<float*>(), color, nverts);
		vert_index_prev = i;
	}

	return nverts;
}
