#include <string>
#include <optional>
#include "utils/vec.hpp"

struct Material;
struct Font_s;

void CG_AdjustFrom640(float& x, float& y, float& w, float& h);
[[nodiscard]] Material* R_RegisterMaterial(const char* fontname, int v);
[[nodiscard]] Font_s* R_RegisterFont(const char* fontname);

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

[[nodiscard]] std::optional<fvec2> WorldToScreen(const fvec3& location);
[[nodiscard]] float R_ScaleByDistance(float dist);
