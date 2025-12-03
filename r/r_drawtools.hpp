
template<typename T>
struct vec2;

template<typename T>
struct vec3;

struct Material;
struct Font_s;

Material* R_RegisterMaterial(const char* fontname, int v);
Font_s* R_RegisterFont(const char* fontname);

void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);
