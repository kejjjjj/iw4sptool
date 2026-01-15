
inline struct WinMouseVars_t* s_wmv = reinterpret_cast<WinMouseVars_t*>(0x19FBF30);
inline struct WinVars_t* g_wv = reinterpret_cast<WinVars_t*>(0x1A01204);
inline struct vidConfig_t* vidConfig = reinterpret_cast<vidConfig_t*>(0x1C8ED9C);
inline struct DxGlobals* dx = reinterpret_cast<DxGlobals*>(0x1C8C080);
inline struct Console* con = reinterpret_cast<Console*>(0x86BD1C);
inline struct cmd_function_s* cmd_functions = reinterpret_cast<cmd_function_s*>(0x144C9D8);
inline struct materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(0x221F900);
inline struct GfxCmdBufState* gfxCmdBufState = reinterpret_cast<GfxCmdBufState*>(0x2163B50);
inline struct r_global_permanent_t* rgp = reinterpret_cast<r_global_permanent_t*>(0x1C87E80);
inline struct clipMap_t* cm = reinterpret_cast<clipMap_t*>(0x1446800);
inline struct CmdArgs* cmd_args = reinterpret_cast<CmdArgs*>(0x144C950);
inline struct level_locals_t* level = reinterpret_cast<level_locals_t*>(0x10A3C10);
inline struct ent_field_t* ent_fields = reinterpret_cast<ent_field_t*>(0x6EAA18);
inline struct refdef_t* refdef = reinterpret_cast<refdef_t*>(0x77FAD0);
inline struct gentity_s* gentities = reinterpret_cast<gentity_s*>(0x04DA780);
inline float* viewpos = reinterpret_cast<float*>(0x77FAE8);
inline unsigned __int16* scr_const_script_brushmodel = reinterpret_cast<unsigned __int16*>(0x12CC022);

inline const char** g_entityBeginParsePoint = reinterpret_cast<const char**>(0xEA7684);
inline const char** g_entityEndParsePoint = reinterpret_cast<const char**>(0xEA7688);