#pragma once

#include "utils/vec.hpp"
#include "utils/defs.hpp"

struct cbrush_t;
struct cbrush {
	cbrush(const cbrush_t* _b, const struct Bounds* _bounds, int _content);
	const cbrush_t* b;
	fvec3 mins, maxs;
	int content;
};

void CM_ShowCollisionFilter();

void CM_LoadAllBrushWindingsToClipMapWithFilter(const std::string& filter);
void CM_LoadBrushWindingsToClipMap(const cbrush& brush);

std::unique_ptr<struct cm_geometry> CM_GetBrushPoints(const cbrush& b, const fvec3& poly_col);
[[nodiscard]] int GetPlaneIntersections(const float** planes, int planeCount, struct SimplePlaneIntersection* OutPts);
[[nodiscard]] int BrushToPlanes(const cbrush* brush, float(*outPlanes)[4]);
[[nodiscard]] struct adjacencyWinding_t* BuildBrushAdjacencyWindingForSide(const float* sideNormal, int basePlaneIndex, SimplePlaneIntersection* pts,
	int ptCount, adjacencyWinding_t* optionalOutWinding, int optionalOutWindingCount);
[[nodiscard]] const char* CM_MaterialForNormal(const cbrush_t* target, const fvec3& normals);
[[nodiscard]] std::vector<std::string> CM_GetBrushMaterials(const cbrush& brush);
[[nodiscard]] bool CM_BoundsInView(const fvec3& mins, const fvec3& maxs, struct cplane_s* frustumPlanes, int numPlanes);

void CM_BuildAxialPlanes(float(*planes)[6][4], const cbrush* brush);
void CM_GetPlaneVec4Form(const struct cbrushside_t* sides, const float(*axialPlanes)[4], unsigned int index, float* expandedPlane);


void __cdecl adjacency_winding(adjacencyWinding_t* w, float* points, vec3_t normal);
namespace __brush {
	void __asm_adjacency_winding();
}
