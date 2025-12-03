#include "r_drawtools.hpp"
#include "cg/cg_local.hpp"
#include "utils/vec.hpp"
#include "utils/resolution.hpp"

#include <cstdint>

Material* R_RegisterMaterial(const char* mtl){
	return ((Material * (*)(const char*, int))0x508760)(mtl, 7);
}
Font_s* R_RegisterFont(const char* fontname) {
	return ((Font_s * (*)(const char*, int))0x4451E0)(fontname, 0);
}

void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	ivec2 xy = adjust_from_640x480({ x, y });
	ivec2 sxy = adjust_from_640x480({ xScale, yScale });

	return ((void(__cdecl*)(char* text, int maxChars, Font_s * font, float x, float y, float xScale, float yScale, float rotation,
		const float* color, int style, const float* glowColor, Material * fxMaterial, Material * fxMaterialGlow,
		int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration))0x50E310)(
			text, maxChars, font, static_cast<float>(xy.x), static_cast<float>(xy.y), static_cast<float>(sxy.x), 
			static_cast<float>(sxy.y),
			rotation,color, style, glowColor, fxMaterial, fxMaterialGlow,
			fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);
}