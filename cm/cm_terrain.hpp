#include <unordered_set>
#include <string>
#include <memory>

struct CollisionAabbTree;
struct cLeaf_t;

struct cm_terrain;
struct cm_geometry;
struct cm_triangle;

class CClipMap;

void CM_LoadAllTerrainToClipMapWithFilter(const std::string& filter);

[[nodiscard]] bool CM_AabbTreeHasCollision(const CollisionAabbTree* tree);
[[maybe_unused]] bool CM_DiscoverTerrain(const std::unordered_set<std::string>& filters);
[[nodiscard]] bool CM_TriangleInView(const cm_triangle* leaf, struct cplane_s* frustumPlanes, int numPlanes = 5);

[[nodiscard]] std::unique_ptr<cm_terrain> CM_TerrainFromLeaf(const cLeaf_t* leaf, const std::unordered_set<std::string>& filters);
[[nodiscard]] std::vector<cm_triangle> CM_AdvanceAabbTree(const CollisionAabbTree* aabbTree, const std::unordered_set<std::string>& filters, const float* color);
