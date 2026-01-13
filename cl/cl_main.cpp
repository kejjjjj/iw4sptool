#include "cl_main.hpp"
#include "cm/cm_clipmap.hpp"
#include "cm/cm_cgentities.hpp"
#include "cm/cm_typedefs.hpp"
#include "utils/hook.hpp"

void CL_Disconnect(int a1, int a2) {
	CClipMap::ClearThreadSafe();
	CGentities::ClearThreadSafe();

	if (const auto f = CStaticHooks::FindByName("CL_Disconnect"))
		return f->Call<void>(a1, a2);
}