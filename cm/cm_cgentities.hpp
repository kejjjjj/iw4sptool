#pragma once

class CGameEntity;

#include <memory>
#include <vector>
#include <string>
#include <mutex>

using GentityPtr_t = std::unique_ptr<CGameEntity>;
using LevelGentities_t = std::vector<std::unique_ptr<CGameEntity>>;

class CGentities
{
public:

	static void CM_LoadAllEntitiesToClipMapWithFilter(const std::string& filter);

	static void Insert(GentityPtr_t& geom);
	static void Insert(GentityPtr_t&& geom);

	static auto begin();
	static auto end();
	static size_t Size();
	static void Clear();
	static void ClearThreadSafe();

	inline static auto& GetLock() { return mtx; }

	template<typename Func>
	static void ForEach(Func func) {
		for (auto& geo : m_pLevelGentities)
			func(geo);
	}

private:
	static LevelGentities_t m_pLevelGentities;
	static std::mutex mtx;

};

