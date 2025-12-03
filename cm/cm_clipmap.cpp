#include "cm_clipmap.hpp"
#include "cm_typedefs.hpp"

LevelGeometry_t CClipMap::m_pLevelGeometry;
GeometryPtr_t CClipMap::m_pWipGeometry;
fvec3 CClipMap::m_vecWipGeometryColor;
std::mutex CClipMap::mtx;


void CClipMap::Insert(std::unique_ptr<cm_geometry>& geom) {

	if (geom)
		m_pLevelGeometry.emplace_back(std::move(geom));

	m_pWipGeometry = nullptr;
}
void CClipMap::Insert(std::unique_ptr<cm_geometry>&& geom) {

	if (geom)
		m_pLevelGeometry.emplace_back(std::move(geom));

	m_pWipGeometry = nullptr;
}
void CClipMap::ClearAllOfType(const cm_geomtype t) {
	auto itr = std::remove_if(m_pLevelGeometry.begin(), m_pLevelGeometry.end(), [&t](std::unique_ptr<cm_geometry>& g){
		return g->Type() == t;
	});

	m_pLevelGeometry.erase(itr, m_pLevelGeometry.end());

}
auto CClipMap::GetAllOfType(const cm_geomtype t) {
	std::vector<LevelGeometry_t::iterator> r;

	for (auto b = m_pLevelGeometry.begin(); b != m_pLevelGeometry.end(); ++b) {
		if (b->get()->Type() == t)
			r.push_back(b);
	}

	return r;
}
