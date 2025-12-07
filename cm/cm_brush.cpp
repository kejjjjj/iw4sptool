#include "cm_brush.hpp"

#include "cg/cg_local.hpp"
#include "utils/defs.hpp"
#include "cm/cm_typedefs.hpp"
#include "cm/cm_clipmap.hpp"
#include "com/com_vector.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_channel.hpp"
#include "cm_terrain.hpp"

#include <print>
#include <ranges>
#include <algorithm>

cbrush::cbrush(const cbrush_t* _b, const Bounds* _bounds, int _content) : b(_b),
	mins(fvec3(_bounds->midPoint) - fvec3(_bounds->halfSize)),
	maxs(fvec3(_bounds->midPoint) + fvec3(_bounds->halfSize)), content(_content) {}

void CM_ShowCollisionFilter()
{
	const auto num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args == 1) {
		if (CClipMap::Size() == 0)
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "there is no geometry to be cleared.. did you intend to use cm_showCollisionFilter <material>?\n");
		CClipMap::ClearThreadSafe();
		return;
	}

	CClipMap::ClearThreadSafe();

	std::string filter;

	for (int i = 1; i < num_args; i++) {
		filter += *(cmd_args->argv[cmd_args->nesting] + i);
		filter += " ";
	}

	CM_LoadAllBrushWindingsToClipMapWithFilter(filter);
	CM_LoadAllTerrainToClipMapWithFilter(filter);
}

void CM_LoadAllBrushWindingsToClipMapWithFilter(const std::string& filter)
{
	std::unique_lock<std::mutex> lock(CClipMap::GetLock());

	CClipMap::ClearAllOfType(cm_geomtype::brush);

	const auto filters = CM_TokenizeFilters(filter);

	if (filters.empty())
		return;

	for (const auto i : std::views::iota(0u, cm->numBrushes)) {

		cbrush b(&cm->brushes[i], &cm->brushBounds[i], cm->brushContents[i]);
		const auto materials = CM_GetBrushMaterials(b);

		if (!std::ranges::any_of(materials, [&filters](const std::string& v) {  return CM_IsMatchingFilter(filters, v.c_str()); }))
			continue;

		CM_LoadBrushWindingsToClipMap(b);
	}
}
void CM_LoadBrushWindingsToClipMap(const cbrush& brush) {
	if (!brush.b)
		return;

	CClipMap::m_pWipGeometry = CM_GetBrushPoints(brush, { 0.f, 1.f, 0.f });
	CClipMap::Insert(CClipMap::m_pWipGeometry);
}
namespace glob {
	float outPlanes[256][4]{};
	adjacencyWinding_t windings[256]{};
	SimplePlaneIntersection pts[1024];
}
std::unique_ptr<cm_geometry> CM_GetBrushPoints(const cbrush& b, const fvec3& poly_col)
{
	if (!b.b)
		return nullptr;

	const auto planeCount = BrushToPlanes(&b, glob::outPlanes);
	const auto intersections = GetPlaneIntersections((const float**)glob::outPlanes, planeCount, glob::pts);

	std::int32_t intersection = 0;
	std::int32_t num_verts = 0;

	CClipMap::m_pWipGeometry = std::make_unique<cm_brush>();
	CClipMap::m_vecWipGeometryColor = poly_col;

	auto c_brush = dynamic_cast<cm_brush*>(CClipMap::m_pWipGeometry.get());

	c_brush->m_brush = cbrush(b);

	c_brush->m_origin = b.mins + ((b.maxs - b.mins) / 2);
	c_brush->m_mins = b.mins;
	c_brush->m_maxs = b.maxs;

	do {
		if (const auto w = BuildBrushAdjacencyWindingForSide(glob::outPlanes[intersection], intersection, glob::pts, intersections, &glob::windings[intersection], 12)) {
			num_verts += w->numsides;
		}
		++intersection;
	} while (intersection < planeCount);

	c_brush->m_numVerts = num_verts;

	return std::move(CClipMap::m_pWipGeometry);
}

int GetPlaneIntersections(const float** planes, int planeCount, SimplePlaneIntersection* OutPts) {
	int r = 0;
	__asm
	{
		push esi;
		push OutPts;
		push planeCount;
		push planes;
		mov esi, 0x5EB810;
		call esi;
		add esp, 12;
		mov r, eax;
		pop esi;
	}

	return r;
}
int BrushToPlanes(const cbrush* brush, float(*_outPlanes)[4])
{
	float planes[6][4]{};
	CM_BuildAxialPlanes((float(*)[6][4])planes, brush);
	auto i = 0u;
	do {
		CM_GetPlaneVec4Form(brush->b->sides, planes, i, _outPlanes[i]);

	} while (++i < brush->b->numsides + 6u);

	return i;
}
void CM_BuildAxialPlanes(float(*planes)[6][4], const cbrush* brush)
{
	(*planes)[0][0] = -1.0;
	(*planes)[0][1] = 0.0;
	(*planes)[0][2] = 0.0;
	(*planes)[0][3] = -brush->mins[0];
	(*planes)[1][0] = 1.0;
	(*planes)[1][1] = 0.0;
	(*planes)[1][2] = 0.0;
	(*planes)[1][3] = brush->maxs[0];
	(*planes)[2][0] = 0.0;
	(*planes)[2][2] = 0.0;
	(*planes)[2][1] = -1.0;
	(*planes)[2][3] = -brush->mins[1];
	(*planes)[3][0] = 0.0;
	(*planes)[3][2] = 0.0;
	(*planes)[3][1] = 1.0;
	(*planes)[3][3] = brush->maxs[1];
	(*planes)[4][0] = 0.0;
	(*planes)[4][1] = 0.0;
	(*planes)[4][2] = -1.0;
	(*planes)[4][3] = -brush->mins[2];
	(*planes)[5][0] = 0.0;
	(*planes)[5][1] = 0.0;
	(*planes)[5][2] = 1.0;
	(*planes)[5][3] = brush->maxs[2];
}
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], unsigned int index, float* expandedPlane)
{
	if (index >= 6u) {
		cplane_s* plane = sides[index - 6u].plane;

		expandedPlane[0] = plane->normal[0];
		expandedPlane[1] = plane->normal[1];
		expandedPlane[2] = plane->normal[2];
		expandedPlane[3] = plane->dist;
		return;
	}

	const float* plane = axialPlanes[index];

	*expandedPlane = plane[0];
	expandedPlane[1] = plane[1];
	expandedPlane[2] = plane[2];
	expandedPlane[3] = plane[3];

}

adjacencyWinding_t* BuildBrushAdjacencyWindingForSide(const float* sideNormal, int basePlaneIndex, SimplePlaneIntersection* pts,
	int ptCount, adjacencyWinding_t* optionalOutWinding, int optionalOutWindingCount)
{
	__asm {
		push optionalOutWindingCount;
		push optionalOutWinding;
		push ptCount;
		push pts;
		push basePlaneIndex;
		push sideNormal;
		mov esi, 0x483950;
		call esi;
		add esp, 4 * 6;
	}
}
const char* CM_MaterialForNormal(const cbrush_t* target, const fvec3& normals)
{
	//non-axial!
	for (unsigned int i = 0; i < target->numsides; i++) {

		cbrushside_t* side = &target->sides[i];

		if (normals == side->plane->normal)
			return cm->materials[side->materialNum].name;
	}


	short mtl = -1;

	if (normals.z == 1.f)
		mtl = target->axialMaterialNum[1][2];
	else if (normals.z == -1.f)
		mtl = target->axialMaterialNum[0][2];

	if (normals.x == 1)
		mtl = target->axialMaterialNum[1][0];
	else if (normals.x == -1)
		mtl = target->axialMaterialNum[0][0];

	if (normals.y == 1.f)
		mtl = target->axialMaterialNum[1][1];
	else if (normals.y == -1.f)
		mtl = target->axialMaterialNum[0][1];

	if (mtl >= 0)
		return cm->materials[mtl].name;


	return (char*)"caulk";

}

std::vector<std::string> CM_GetBrushMaterials(const cbrush& brush)
{
	std::vector<std::string> result;

	const auto planeCount = BrushToPlanes(&brush, glob::outPlanes);
	[[maybe_unused]] const auto intersections = GetPlaneIntersections((const float**)glob::outPlanes, planeCount, glob::pts);

	for (const auto i : std::views::iota(0, planeCount))
	{
		const fvec3 plane = glob::outPlanes[i];

		if (const auto mtl = CM_MaterialForNormal(brush.b, plane)) {
			result.emplace_back(mtl);
		}


	}

	return result;
}
bool CM_BoundsInView(const fvec3& mins, const fvec3& maxs, struct cplane_s* frustumPlanes, int numPlanes) {
	if (numPlanes <= 0)
		return 1;

	cplane_s* plane = frustumPlanes;
	int idx = 0;
	while ((BoxOnPlaneSide(mins, maxs, plane) & 1) != 0) {
		++plane;
		++idx;

		if (idx >= numPlanes)
			return 1;
	}

	return 0;
}
float RepresentativeTriangleFromWinding(const float *xyz, int pointCount, const float* normal, int* i0, int* i1, int* i2)
{
	__asm{
		push i2;
		push i1;
		push i0;
		push normal;
		push pointCount;
		push xyz;
		mov esi, 0x6394B0;
		call esi;
		add esp, 24;
	}
}

void __cdecl adjacency_winding(adjacencyWinding_t* w, float* points, vec3_t normal) {

	int i0, i1, i2;
	//points = (float*)((char*)points + 12);

	if (RepresentativeTriangleFromWinding(points, w->numsides, normal, &i0, &i1, &i2) < 0.001)
		return;

	cm_triangle tri;
	std::vector<fvec3> winding_points;

	tri.a = &points[i2];
	tri.b = &points[i1];
	tri.c = &points[i0];

	PlaneFromPointsASM(tri.plane, tri.a, tri.b, tri.c);

	if (DotProduct(tri.plane, normal) <= 0.f) {
		std::swap(tri.a, tri.c);
	}

	auto brush = dynamic_cast<cm_brush*>(CClipMap::m_pWipGeometry.get());
	tri.material = CM_MaterialForNormal(brush->m_brush.b, normal);

	brush->m_triangles.emplace_back(tri);

	for (const auto winding : std::views::iota(0, w->numsides)) {
		winding_points.emplace_back(fvec3{ &points[winding * 3] });
	}

	brush->m_windings.emplace_back(cm_winding{ winding_points, normal, CClipMap::m_vecWipGeometryColor });
}

__declspec(naked) void __brush::__asm_adjacency_winding() {
	static constexpr unsigned int dst = 0x483D28;

	__asm
	{

		push edi; //normal
		lea eax, [esp + 604Ch - 3000h];
		push eax; //points
		push esi; //w
		call adjacency_winding;
		add esp, 12;

		fld dword ptr[edi + 04h];
		fmul dword ptr[esp + 6048h - 600Ch];
		add esp, 10h;
		fld dword ptr[edi];
		fmul dword ptr[esp + 6038h - 6010h];
		faddp st(1), st;
		jmp dst;
	}

}
