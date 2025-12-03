#pragma once

#include <vector>
#include "utils/vec.hpp"

void CM_ShowCollision();

void CM_MakeInteriorRenderable(const std::vector<fvec3>& points, const float* color);
[[nodiscard]] int CM_MakeOutlinesRenderable(const std::vector<fvec3>& points, const float* color, bool depthTest, int nverts);
