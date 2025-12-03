
#pragma once

#include "utils/vec.hpp"

[[nodiscard]] int BoxOnPlaneSide(const fvec3& emins, const fvec3& emaxs, struct cplane_s* p);

void PlaneFromPointsASM(float* plane, float* v0, float* v1, float* v2);

void CreateFrustumPlanes(struct cplane_s* frustumPlanes);
void BuildFrustumPlanes(struct cplane_s* FrustumPlanes);
void SetPlaneSignbits(cplane_s* out);
