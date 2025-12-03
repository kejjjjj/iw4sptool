#include "cm_terrain.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm/cm_typedefs.hpp"
#include "com/com_vector.hpp"
#include "cm/cm_clipmap.hpp"

#include <ranges>

void CM_LoadAllTerrainToClipMapWithFilter(const std::string& filter)
{
	std::unique_lock<std::mutex> lock(CClipMap::GetLock());

	CClipMap::ClearAllOfType(cm_geomtype::terrain);

	const auto filters = CM_TokenizeFilters(filter);

	if (filters.empty())
		return;

	CM_DiscoverTerrain(filters);
}

bool CM_AabbTreeHasCollision(const CollisionAabbTree* tree) {
	const auto materialInfo = &cm->materials[tree->materialIndex];
	return (materialInfo->contents & MASK_PLAYERSOLID) != 0;
}

std::unordered_map<CollisionPartition*, CollisionPartition*> discovered_partitions;

bool CM_DiscoverTerrain(const std::unordered_set<std::string>& filters)
{

	try {
		discovered_partitions.clear();
		for (const auto i : std::views::iota(0u, cm->numLeafs)) {
			CClipMap::Insert( CM_TerrainFromLeaf(&cm->leafs[i], filters));
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

std::unique_ptr<cm_terrain> CM_TerrainFromLeaf(const cLeaf_t* leaf, const std::unordered_set<std::string>& filters)
{
	if (!leaf || !leaf->collAabbCount)
		return nullptr;

	auto aabbIdx = 0u;

	std::vector<Triangles> tris;

	do {
		const CollisionAabbTree* aabb = &cm->aabbTrees[aabbIdx + leaf->firstCollAabbIndex];

		if(const auto&& tri = CM_AdvanceAabbTree(aabb, filters, vec4_t{ 0,0.1f,1.f, 0.8f }); !tri.empty())
			tris.emplace_back(tri);

		++aabbIdx;
	} while (aabbIdx < leaf->collAabbCount);

	return std::make_unique<cm_terrain>(leaf, std::move(tris));
}
std::vector<cm_triangle> CM_AdvanceAabbTree(const CollisionAabbTree* aabbTree, const std::unordered_set<std::string>& filters, const float* color)
{
	if (aabbTree->childCount) {
		std::vector<cm_triangle> tris;

		auto child = &cm->aabbTrees[aabbTree->u.firstChildIndex];
		for ([[maybe_unused]] const auto i : std::views::iota(0u, aabbTree->childCount)) {
			auto newTris = CM_AdvanceAabbTree(child, filters, color);
			tris.insert(tris.end(), newTris.begin(), newTris.end());
			++child;
		}
		return tris;
	}

	const auto mat = cm->materials[aabbTree->materialIndex].name;
	if (CM_IsMatchingFilter(filters, mat) == false) {
		return {};
	}

	CollisionAabbTreeIndex fChild = aabbTree->u;
	CollisionPartition* partition = &cm->partitions[fChild.partitionIndex];

	//saves some fps
	if (discovered_partitions.contains(partition))
		return {};

	discovered_partitions[partition] = partition;

	std::vector<cm_triangle> tris;

	auto firstTri = partition->firstTri;
	if (firstTri < firstTri + partition->triCount)
	{
		auto triIndice = 3 * firstTri;

		do {
			cm_triangle tri;
			tri.has_collision = CM_AabbTreeHasCollision(aabbTree);
			tri.a = cm->verts[cm->triIndices[triIndice]];
			tri.b = cm->verts[cm->triIndices[triIndice + 1]];
			tri.c = cm->verts[cm->triIndices[triIndice + 2]];
			tri.material = mat;

			PlaneFromPointsASM(tri.plane, tri.a, tri.b, tri.c);

			tri.color[0] = color[0];
			tri.color[1] = color[1];
			tri.color[2] = color[2];
			tri.color[3] = 0.3f;

			tris.emplace_back(tri);

			++firstTri;
			triIndice += 3;

		} while (firstTri < partition->firstTri + partition->triCount);

	}

	return tris;
}
bool CM_TriangleInView(const cm_triangle* tris, struct cplane_s* frustumPlanes, int numPlanes)
{
	if (numPlanes <= 0)
		return 1;
	cplane_s* plane = frustumPlanes;
	int idx = 0;
	while ((BoxOnPlaneSide(tris->get_mins(), tris->get_maxs(), plane) & 1) != 0) {
		++plane;
		++idx;

		if (idx >= numPlanes)
			return 1;
	}

	return 0;
}