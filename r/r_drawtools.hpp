#include <string>

template<typename T>
struct vec2;

template<typename T>
struct vec3;

struct Material;
struct Font_s;

void CG_AdjustFrom640(float& x, float& y, float& w, float& h);
Material* R_RegisterMaterial(const char* fontname, int v);
Font_s* R_RegisterFont(const char* fontname);

void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void R_DrawTextWithEffects(const std::string& text, const char* fontname, float x, float y, float xScale, float yScale, 
	float rotation, const float* color, int style, const float* glowColor);


void CG_CornerDebugPrint(const char* text, float* color, const struct ScreenPlacement* sP, float posX, float posY);
[[nodiscard]] struct ScreenPlacement* Scr_GetPlacement();
