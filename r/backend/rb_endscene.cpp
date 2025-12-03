#include "rb_endscene.hpp"
#include "utils/engine.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "utils/vec.hpp"
#include "utils/defs.hpp"

#include <array>
#include <ranges>

void RB_TessOverflow(bool two_sided, bool depthTest){
	RB_EndTessSurface();
	RB_BeginSurfaceInternal(two_sided, depthTest);
}

void RB_EndTessSurface() {
	Engine::Tools::Call<void>(0x523C30);
}
bool RB_CheckTessOverflow(int vertexCount, int indexCount) {
	if (vertexCount + tess->vertexCount + 4 > 0x154A || indexCount + tess->indexCount + 6 > 0x100000)
		return true;

	return false;
}
void RB_BeginSurface(bool two_sided, bool depthTest) {

	static Material material;

	material = *rgp->whiteMaterial;

	static unsigned int loadBits[2] = { material.stateBitsTable->loadBits[0], material.stateBitsTable->loadBits[1] };
	static unsigned int loadBitsLine[2] = { rgp->lineMaterial->stateBitsTable->loadBits[0], rgp->lineMaterial->stateBitsTable->loadBits[1] };

	material.stateBitsTable->loadBits[1] = depthTest ? loadBitsLine[1] : loadBits[1];
	material.stateBitsTable->loadBits[0] = two_sided ? 422072677 : 422089061;

	tess->firstVertex = 0;
	tess->lastVertex = 0;

	gfxCmdBufState->material = &material;
	gfxCmdBufState->techType = MaterialTechniqueType::TECHNIQUE_UNLIT;
	gfxCmdBufState->prim.vertDeclType = VERTDECL_GENERIC;
	gfxCmdBufState->technique = gfxCmdBufState->material->techniqueSet->remappedTechniqueSet->techniques[static_cast<int>(gfxCmdBufState->techType)];

}
void RB_BeginSurfaceInternal(bool two_sided, bool depthTest) {
	if (tess->indexCount)
		RB_EndTessSurface();

	RB_BeginSurface(two_sided, depthTest);
}

void RB_DrawLines3D(int count, int width, const GfxPointVertex* verts, bool depthTest) {
	Engine::Tools::Call<void>(0x536F30, count, width, verts, depthTest);
}
int RB_AddDebugLine(GfxPointVertex* verts, char depthTest, const float* start, const float* end, const float* color, int vertCount)
{
	int _vc = vertCount;
	if (vertCount + 2 > 2725)
	{
		RB_DrawLines3D(vertCount / 2, 1, verts, depthTest);
		_vc = 0;
	}

	GfxPointVertex* vert = &verts[_vc];
	if (color) {
		R_ConvertColorToBytes(color, (GfxColor*)&vert->color);
	}

	verts[_vc + 1].color[0] = vert->color[0];
	verts[_vc + 1].color[1] = vert->color[1];
	verts[_vc + 1].color[2] = vert->color[2];
	verts[_vc + 1].color[3] = vert->color[3];

	VectorCopy(start, vert->xyz);

	vert = &verts[_vc + 1];
	VectorCopy(end, vert->xyz);
	
	return _vc + 2;

}
void R_ConvertColorToBytes(const float* in, GfxColor* out){
	Engine::Tools::Call<void>(0x461180, in, out);
}
GfxPointVertex verts2[2075];

constexpr std::array<std::int32_t, 24> iEdgePairs =
{
  0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3,
  2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7
};

constexpr auto iNextEdgePair = &iEdgePairs[1];
constexpr auto iota = std::views::iota;

void RB_DrawBoxEdges(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color) {
	float v[8][3]{};
	for (const auto i : iota(0u, 8u)) {
		for (const auto j : iota(0u, 3u)) {

			if ((i & (1 << j)) != 0)
				v[i][j] = maxs[j];
			else
				v[i][j] = mins[j];
		}
	}

	std::vector<fvec3> points;
	for (const auto i : iota(0u, 12u)) {
		points.emplace_back(v[iEdgePairs[i * 2]]);
		points.emplace_back(v[iNextEdgePair[i * 2]]);
	}

	auto vert_count = 0;
	for (const auto i : iota(0u, 12u)) {
		vert_count = RB_AddDebugLine(verts2, depthtest, points[i * 2].As<float*>(), points[i * 2 + 1].As<float*>(), color, vert_count);
	}

	RB_DrawLines3D(vert_count / 2, 1, verts2, depthtest);
}
void RB_DrawPolyInteriors(const std::vector<fvec3>& points, const float* c, bool two_sided, bool depthTest)
{

	auto n_points = points.size();

	//partly copied from iw3xo :)
	if (n_points < 3u)
		return;

	GfxColor color;
	R_ConvertColorToBytes(c, &color);

	RB_BeginSurfaceInternal(two_sided, depthTest);


	if (RB_CheckTessOverflow(n_points, 3 * (n_points - 2)))
		RB_TessOverflow(two_sided, depthTest);

	for (const auto idx : std::views::iota(std::size_t{ 0 }, n_points))
		RB_SetPolyVertice(points[idx].As<float*>(), &color, tess->vertexCount + idx, idx);


	for (const auto idx : std::views::iota(std::size_t{ 2 }, n_points)) {
		tess->indices[tess->indexCount + 0] = static_cast<short>(tess->vertexCount);
		tess->indices[tess->indexCount + 1] = static_cast<short>(idx + tess->vertexCount);
		tess->indices[tess->indexCount + 2] = static_cast<short>(idx + tess->vertexCount - 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += n_points;

	RB_EndTessSurface();

}
void RB_DrawBoxPolygons(const fvec3& mins, const fvec3& maxs, const float* color, bool two_sided, bool depthTest)
{
	float v[8][3]{};
	for (const auto i : iota(0u, 8u)) {
		for (const auto j : iota(0u, 3u)) {

			if ((i & (1 << j)) != 0)
				v[i][j] = maxs[j];
			else
				v[i][j] = mins[j];
		}
	}


	std::vector<fvec3> points;
	for (const auto i : iota(0u, 12u)) {
		points.emplace_back(v[iEdgePairs[i * 2]]);
		points.emplace_back(v[iNextEdgePair[i * 2]]);
	}

	std::vector<fvec3> points2;

	for (const auto i : iota(0u, 12u)) {
		points2.emplace_back(points[i * 2]);
		points2.emplace_back(points[i * 2 + 1]);

	}

	RB_DrawPolyInteriors(points2, color, two_sided, depthTest);
}
void RB_SetPolyVertice(float* xyz, const GfxColor* color, const int vert, const int index)
{
	VectorCopy(xyz, tess->verts[vert].xyzw);
	tess->verts[vert].color.packed = color->packed;

	switch (index)
	{
	case 0:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vert].normal.packed = 1073643391;
}