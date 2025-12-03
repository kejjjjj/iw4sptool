#pragma once

template<typename T>
struct vec3;

using fvec3 = vec3<float>;

void RB_TessOverflow(bool two_sided, bool depthTest);
void RB_EndTessSurface();
[[nodiscard]] bool RB_CheckTessOverflow(int vertexCount, int indexCount);
void RB_BeginSurfaceInternal(bool two_sided, bool depthTest);
void RB_BeginSurface(bool two_sided, bool depthTest);

void RB_DrawLines3D(int count, int width, const struct GfxPointVertex* verts, bool depthTest);
void RB_DrawBoxEdges(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color);
void RB_DrawBoxPolygons(const fvec3& mins, const fvec3& maxs, const float* color, bool two_sided, bool depthTest);

int RB_AddDebugLine(struct GfxPointVertex* verts, char depthTest, const float* start, const float* end, const float* color, int vertCount);
void R_ConvertColorToBytes(const float* in, union GfxColor* out);
void RB_SetPolyVertice(float* xyz, const union GfxColor* color, const int vert, const int index);
