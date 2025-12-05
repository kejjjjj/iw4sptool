#include "r_drawtools.hpp"
#include "cg/cg_local.hpp"
#include "utils/vec.hpp"
#include "utils/resolution.hpp"
#include "utils/engine.hpp"
#include "cg/cg_offsets.hpp"
#include <cstdint>


void CG_AdjustFrom640(float& x, float& y, float& w, float& h)
{
	const float scale = (float)vidConfig->displayWidth / 640.f;
	const float scaleY = (float)vidConfig->displayHeight / 480.f;

	x *= scale;
	y *= scaleY;
	w *= scale;
	h *= scaleY;
}
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

	return ((void(__cdecl*)(char* text, int maxChars, Font_s * font, float x, float y, float xScale, float yScale, float rotation,
		const float* color, int style, const float* glowColor, Material * fxMaterial, Material * fxMaterialGlow,
		int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration))0x50E310)(
			text, maxChars, font, x, y, xScale, yScale,
			rotation,color, style, glowColor, fxMaterial, fxMaterialGlow,
			fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);
}
void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	Font_s* _font = R_RegisterFont(font);
	R_AddCmdDrawTextWithEffects(text, strlen(text), _font, x, y, xScale, yScale, rotation, color, style, glowColor, fxMaterial, fxMaterialGlow, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);

}
void R_DrawTextWithEffects(const std::string& text, const char* fontname, float x, float y, float xScale, float yScale, float rotation, const float* color, int style, const float* glowColor)
{
	Material* fxMaterial = R_RegisterMaterial("decode_characters");
	Material* fxMaterialGlow = R_RegisterMaterial("decode_characters_glow");

	CG_AdjustFrom640(x, y, xScale, yScale);

	return R_AddCmdDrawTextWithEffects((char*)text.c_str(), fontname, x, y, xScale, yScale, rotation, color, style, glowColor, fxMaterial, fxMaterialGlow, 0, 0, 0, 0);
}

void CG_CornerDebugPrint(const char* text, float* color, const ScreenPlacement* ssP, float posX, float posY)
{
	float lw = 0.f;
	const char* label = " !";
	__asm
	{
		push ebx;
		push label;
		push lw;
		push posY;
		push posX;
		mov esi, ssP;
		mov eax, color;
		mov ecx, text;
		mov ebx, 0x55F180;
		call ebx;
		add esp, 16;
		pop ebx;
	}
}
ScreenPlacement* Scr_GetPlacement() {
	return Engine::Tools::Call<ScreenPlacement*>(0x448A30, 0); // 0 is for clientnum
}
