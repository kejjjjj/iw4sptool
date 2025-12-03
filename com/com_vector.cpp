#include "com_vector.hpp"
#include "cg/cg_local.hpp"
#include "utils/defs.hpp"

int BoxOnPlaneSide(const fvec3& emins, const fvec3& emaxs, struct cplane_s* p)
{
	float dist1, dist2;

	switch (p->signbits)
	{
	case 0:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 1:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 2:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 3:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 4:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;      // shut up compiler
		break;
	}

	return (2 * (dist2 < p->dist)) | (dist1 > p->dist);
}

void PlaneFromPointsASM(float* plane, float* v0, float* v1, float* v2)
{
	static constexpr auto fnc = 0x562050;
	__asm
	{
		push v2;
		push v1;
		push v0;
		push plane;
		mov esi, 0x40A890;
		call esi;
		add esp, 16;
	}
}
void CreateFrustumPlanes(cplane_s* frustum_planes)
{
	BuildFrustumPlanes(frustum_planes);

	frustum_planes[5].normal[0] = -frustum_planes[4].normal[0];
	frustum_planes[5].normal[1] = -frustum_planes[4].normal[1];
	frustum_planes[5].normal[2] = -frustum_planes[4].normal[2];

	frustum_planes[5].dist = -frustum_planes[4].dist - 2000;
	auto plane = &frustum_planes[5];

	char signbit = 0;

	if (plane->normal[0] != 1.f) {
		if (plane->normal[1] == 1.f)
			signbit = 1;
		else {
			signbit = plane->normal[2] == 1.f ? 2 : 3;
		}
	}

	plane->type = signbit;

	SetPlaneSignbits(plane);
}

void BuildFrustumPlanes(cplane_s* frustumPlanes)
{
	for (int i = 0; i < 5; i++) {

		uint32_t ebp = *(char*)0x75E688 * 0x2B8 + 0x1EF9BCC;

		const auto view = (DpvsView*)(ebp);
		cplane_s* plane = &frustumPlanes[i];

		auto fp = &view->frustumPlanes[i];

		VectorCopy(fp->coeffs, plane->normal);
		plane->dist = fp->coeffs[3] * -1;

		char signbit = 0;

		if (plane->normal[0] != 1.f) {
			if (plane->normal[1] == 1.f)
				signbit = 1;
			else {
				signbit = plane->normal[2] == 1.f ? 2 : 3;
			}
		}

		plane->type = signbit;
		SetPlaneSignbits(plane);

	};
}
void SetPlaneSignbits(cplane_s* out)
{
	char bits, j;

	// for fast box on planeside test
	bits = 0;
	for (j = 0; j < 3; j++) {
		if (out->normal[j] < 0) {
			bits |= 1 << j;
		}
	}
	out->signbits = bits;
}