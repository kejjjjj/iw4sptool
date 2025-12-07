#pragma once

#define MASK_PLAYERSOLID (42057745)

struct HINSTANCE__;
struct HWND__;

struct __declspec(align(4)) WinMouseVars_t
{
	int oldButtonState;
	long oldPos[2];
	bool mouseActive;
	bool mouseInitialized;
};

struct WinVars_t
{
	HINSTANCE__* reflib_library;
	int reflib_active;
	HWND__* hWnd;
	HINSTANCE__* hInstance;
	int activeApp;
	int isMinimized;
	int recenterMouse;
	unsigned int sysMsgTime;
};

struct cmd_function_s
{
	cmd_function_s* next;
	const char* name;
	const char* autoCompleteDir;
	const char* autoCompleteExt;
	void(__cdecl* function)();
};

struct GfxDrawSurfFields
{
	unsigned __int64 objectId : 16;
	unsigned __int64 reflectionProbeIndex : 8;
	unsigned __int64 customIndex : 5;
	unsigned __int64 materialSortedIndex : 11;
	unsigned __int64 prepass : 2;
	unsigned __int64 primaryLightIndex : 8;
	unsigned __int64 surfType : 4;
	unsigned __int64 primarySortKey : 6;
	unsigned __int64 unused : 4;
};

union GfxDrawSurf
{
	GfxDrawSurfFields fields;
	unsigned long long packed;
};

#pragma pack(push, 4)
struct MaterialInfo
{
	const char* name;
	char gameFlags;
	char sortKey;
	char textureAtlasRowCount;
	char textureAtlasColumnCount;
	GfxDrawSurf drawSurf;
	unsigned int surfaceTypeBits;
	unsigned __int16 hashIndex;
};
#pragma pack(pop)

struct MaterialStreamRouting
{
	char source;
	char dest;
};

struct MaterialVertexStreamRouting
{
	MaterialStreamRouting data[16];
	void* decl[16];
};

struct MaterialVertexDeclaration
{
	char streamCount;
	bool hasOptionalSource;
	bool isLoaded;
	MaterialVertexStreamRouting routing;
};
struct MaterialArgumentCodeConst
{
	unsigned __int16 index;
	char firstRow;
	char rowCount;
};

union MaterialArgumentDef
{
	const float* literalConst;
	MaterialArgumentCodeConst codeConst;
	unsigned int codeSampler;
	unsigned int nameHash;
};

/* MaterialShaderArgument->type */
#define MTL_ARG_MATERIAL_VERTEX_CONST	0x0
#define MTL_ARG_LITERAL_VERTEX_CONST	0x1
#define MTL_ARG_MATERIAL_PIXEL_SAMPLER	0x2
#define MTL_ARG_CODE_VERTEX_CONST	    0x3
#define MTL_ARG_CODE_PIXEL_SAMPLER	    0x4
#define MTL_ARG_CODE_PIXEL_CONST	    0x5
#define MTL_ARG_MATERIAL_PIXEL_CONST	0x6
#define MTL_ARG_LITERAL_PIXEL_CONST	    0x7

struct MaterialShaderArgument
{
	unsigned __int16 type;
	unsigned __int16 dest;
	MaterialArgumentDef u;
};
struct GfxVertexShaderLoadDef
{
	unsigned int* program;
	unsigned __int16 programSize;
	unsigned __int16 loadForRenderer;
};

struct MaterialVertexShaderProgram
{
	void* vs;
	GfxVertexShaderLoadDef loadDef;
};

struct MaterialVertexShader
{
	const char* name;
	MaterialVertexShaderProgram prog;
};
struct GfxPixelShaderLoadDef
{
	unsigned int* program;
	unsigned __int16 programSize;
	unsigned __int16 loadForRenderer;
};

struct MaterialPixelShaderProgram
{
	void* ps;
	GfxPixelShaderLoadDef loadDef;
};

struct MaterialPixelShader
{
	const char* name;
	MaterialPixelShaderProgram prog;
};

#pragma pack(push, 4)
struct MaterialPass
{
	MaterialVertexDeclaration* vertexDecl;
	MaterialVertexShader* vertexShader;
	MaterialPixelShader* pixelShader;
	char perPrimArgCount;
	char perObjArgCount;
	char stableArgCount;
	char customSamplerFlags;
	MaterialShaderArgument* args;
};
#pragma pack(pop)

struct MaterialTechnique
{
	const char* name;
	unsigned __int16 flags;
	unsigned __int16 passCount;
	MaterialPass passArray[1];	// count = passCount
};
enum MaterialWorldVertexFormat : char
{
	MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
	MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
	MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
	MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
	MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
	MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
	MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
	MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
	MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
	MTL_WORLDVERT_TEX_5_NRM_1 = 0x9,
	MTL_WORLDVERT_TEX_5_NRM_2 = 0xA,
	MTL_WORLDVERT_TEX_5_NRM_3 = 0xB,
};
struct MaterialTechniqueSet
{
	char* name;
	MaterialWorldVertexFormat worldVertFormat;
	bool hasBeenUploaded;
	char unused[1];
	MaterialTechniqueSet* remappedTechniqueSet;
	MaterialTechnique* techniques[34];

};
struct GfxStateBits
{
	unsigned int loadBits[2];
};
struct Material
{
	MaterialInfo info;
	char stateBitsEntry[34];
	char textureCount;
	char constantCount;
	char stateBitsCount;
	char stateFlags;
	char cameraRegion;
	char pad[16];
	struct MaterialTechniqueSet* techniqueSet;
	struct MaterialTextureDef* textureTable;
	struct MaterialConstantDef* constantTable;
	struct GfxStateBits* stateBitsTable;
};

struct Glyph
{
	unsigned __int16 letter;
	char x0;
	char y0;
	char dx;
	char pixelWidth;
	char pixelHeight;
	float s0;
	float t0;
	float s1;
	float t1;
};
struct Font_s
{
	const char* fontName;
	int pixelHeight;
	int glyphCount;
	Material* material;
	Material* glowMaterial;
	Glyph* glyphs;
};

struct __declspec(align(4)) vidConfig_t
{
	unsigned int sceneWidth;
	unsigned int sceneHeight;
	unsigned int displayWidth;
	unsigned int displayHeight;
	unsigned int displayFrequency;
	int isFullscreen;
	float aspectRatioWindow;
	float aspectRatioScenePixel;
	float aspectRatioDisplayPixel;
	unsigned int maxTextureSize;
	unsigned int maxTextureMaps;
	bool deviceSupportsGamma;
};

struct GfxWindowTarget
{
	HWND__* hwnd;
	void* swapChain;
	int width;
	int height;
};
struct __declspec(align(8)) DxGlobals
{
	HINSTANCE__* hinst;
	struct IDirect3D9* d3d9;
	struct IDirect3DDevice9* device;
	unsigned int adapterIndex;
	bool adapterNativeIsValid;
	int adapterNativeWidth;
	int adapterNativeHeight;
	int adapterFullscreenWidth;
	int adapterFullscreenHeight;
	int depthStencilFormat;
	unsigned int displayModeCount;
	void* displayModes[256];
	const char* resolutionNameTable[257];
	const char* refreshRateNameTable[257];
	char modeText[5120];
	void* fencePool[8];
	unsigned int nextFence;
	int gpuSync;
	int multiSampleType;
	unsigned int multiSampleQuality;
	int sunSpriteSamples;
	void* singleSampleDepthStencilSurface;
	bool deviceLost;
	bool inScene;
	int targetWindowIndex;
	int windowCount;
	GfxWindowTarget windows[1];
	int flushGpuQueryCount;
	void* flushGpuQuery;
	unsigned __int64 gpuSyncDelay;
	unsigned __int64 gpuSyncStart;
	unsigned __int64 gpuSyncEnd;
	bool flushGpuQueryIssued;
	int linearNonMippedMinFilter;
	int linearNonMippedMagFilter;
	int linearMippedMinFilter;
	int linearMippedMagFilter;
	int anisotropicMinFilter;
	int anisotropicMagFilter;
	int linearMippedAnisotropy;
	int anisotropyFor2x;
	int anisotropyFor4x;
	int mipFilterMode;
	unsigned int mipBias;
	void* swapFence;
};
struct Message
{
	int startTime;
	int endTime;
};

struct MessageLine
{
	int messageIndex;
	int textBufPos;
	int textBufSize;
	int typingStartTime;
	int lastTypingSoundTime;
	int flags;
};

struct MessageWindow
{
	MessageLine* lines;
	Message* messages;
	char* circularTextBuffer;
	int textBufSize;
	int lineCount;
	int padding;
	int scrollTime;
	int fadeIn;
	int fadeOut;
	int textBufPos;
	int firstLineIndex;
	int activeLineCount;
	int messageIndex;
};

struct MessageBuffer
{
	char gamemsgText[4][2048];
	MessageWindow gamemsgWindows[4];
	MessageLine gamemsgLines[4][12];
	Message gamemsgMessages[4][12];
	char miniconText[4096];
	MessageWindow miniconWindow;
	MessageLine miniconLines[100];
	Message miniconMessages[100];
	char errorText[1024];
	MessageWindow errorWindow;
	MessageLine errorLines[5];
	Message errorMessages[5];
};

struct Console
{
	int initialized;
	MessageWindow consoleWindow;
	MessageLine consoleLines[1024];
	Message consoleMessages[1024];
	char consoleText[0x10000];
	char textTempLine[512];
	unsigned int lineOffset;
	int displayLineOffset;
	int prevChannel;
	bool outputVisible;
	int fontHeight;
	int visibleLineCount;
	int visiblePixelWidth;
	float screenMin[2];
	float screenMax[2];
	MessageBuffer messageBuffer[1];
	float color[4];
};

union PackedUnitVec
{
	unsigned int packed;
	char array[4];
};
// fucking namespaces
union GfxColor
{
	unsigned int packed;
	unsigned char array[4];
};
struct GfxVertex{
	float xyzw[4];
	GfxColor color;
	float texCoord[2];
	PackedUnitVec normal;
};
enum MaterialVertexDeclType : __int32
{
	VERTDECL_GENERIC = 0x0,
	VERTDECL_PACKED = 0x1,
	VERTDECL_WORLD = 0x2,
	VERTDECL_WORLD_T1N0 = 0x3,
	VERTDECL_WORLD_T1N1 = 0x4,
	VERTDECL_WORLD_T2N0 = 0x5,
	VERTDECL_WORLD_T2N1 = 0x6,
	VERTDECL_WORLD_T2N2 = 0x7,
	VERTDECL_WORLD_T3N0 = 0x8,
	VERTDECL_WORLD_T3N1 = 0x9,
	VERTDECL_WORLD_T3N2 = 0xA,
	VERTDECL_WORLD_T4N0 = 0xB,
	VERTDECL_WORLD_T4N1 = 0xC,
	VERTDECL_WORLD_T4N2 = 0xD,
	VERTDECL_POS_TEX = 0xE,
	VERTDECL_STATICMODELCACHE = 0xF,
	VERTDECL_COUNT = 0x10,
};
struct __declspec() materialCommands_t
{
	GfxVertex verts[5450];
	unsigned __int16 indices[1048576];
	MaterialVertexDeclType vertDeclType;
	unsigned int vertexSize;
	int indexCount;
	int vertexCount;
	int firstVertex;
	int lastVertex;
	bool finishedFilling;
};
enum class MaterialTechniqueType
{
	TECHNIQUE_DEPTH_PREPASS = 0x0,
	TECHNIQUE_BUILD_FLOAT_Z = 0x1,
	TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
	TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
	TECHNIQUE_UNLIT = 0x4,
	TECHNIQUE_EMISSIVE = 0x5,
	TECHNIQUE_EMISSIVE_SHADOW = 0x6,
	TECHNIQUE_LIT_BEGIN = 0x7,
	TECHNIQUE_LIT = 0x7,
	TECHNIQUE_LIT_SUN = 0x8,
	TECHNIQUE_LIT_SUN_SHADOW = 0x9,
	TECHNIQUE_LIT_SPOT = 0xA,
	TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
	TECHNIQUE_LIT_OMNI = 0xC,
	TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
	TECHNIQUE_LIT_INSTANCED = 0xE,
	TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
	TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
	TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
	TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
	TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
	TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,
	TECHNIQUE_LIT_END = 0x15,
	TECHNIQUE_LIGHT_SPOT = 0x15,
	TECHNIQUE_LIGHT_OMNI = 0x16,
	TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
	TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
	TECHNIQUE_FAKELIGHT_VIEW = 0x19,
	TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
	TECHNIQUE_CASE_TEXTURE = 0x1B,
	TECHNIQUE_WIREFRAME_SOLID = 0x1C,
	TECHNIQUE_WIREFRAME_SHADED = 0x1D,
	TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
	TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
	TECHNIQUE_DEBUG_BUMPMAP = 0x20,
	TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
	//TECHNIQUE_COUNT = 0x22
};
struct gfxVertexSteamsUnk
{
	unsigned int stride;
	int* vb; // IDirect3DVertexBuffer9
	unsigned int offset;
};

struct GfxCmdBufPrimState
{
	int* device; // IDirect3DDevice9
	int* indexBuffer; // IDirect3DIndexBuffer9
	MaterialVertexDeclType vertDeclType;
	gfxVertexSteamsUnk streams[2];
	int* vertexDecl; // IDirect3DVertexDeclaration9
};
enum GfxDepthRangeType
{
	GFX_DEPTH_RANGE_SCENE = 0x0,
	GFX_DEPTH_RANGE_VIEWMODEL = 0x2,
	GFX_DEPTH_RANGE_FULL = 0xFFFFFFFF,
};
struct GfxViewport
{
	int x;
	int y;
	int width;
	int height;
};
enum GfxRenderTargetId
{
	R_RENDERTARGET_SAVED_SCREEN = 0x0,
	R_RENDERTARGET_FRAME_BUFFER = 0x1,
	R_RENDERTARGET_SCENE = 0x2,
	R_RENDERTARGET_RESOLVED_POST_SUN = 0x3,
	R_RENDERTARGET_RESOLVED_SCENE = 0x4,
	R_RENDERTARGET_FLOAT_Z = 0x5,
	R_RENDERTARGET_DYNAMICSHADOWS = 0x6,
	R_RENDERTARGET_PINGPONG_0 = 0x7,
	R_RENDERTARGET_PINGPONG_1 = 0x8,
	R_RENDERTARGET_SHADOWCOOKIE = 0x9,
	R_RENDERTARGET_SHADOWCOOKIE_BLUR = 0xA,
	R_RENDERTARGET_POST_EFFECT_0 = 0xB,
	R_RENDERTARGET_POST_EFFECT_1 = 0xC,
	R_RENDERTARGET_SHADOWMAP_SUN = 0xD,
	R_RENDERTARGET_SHADOWMAP_SPOT = 0xE,
	R_RENDERTARGET_COUNT = 0xF,
	R_RENDERTARGET_NONE = 0x10,
};
struct GfxCmdBufState
{
	char refSamplerState[16];
	unsigned int samplerState[16];
	struct GfxTexture* samplerTexture[16];
	GfxCmdBufPrimState prim;
	Material* material;
	MaterialTechniqueType techType;
	struct MaterialTechnique* technique;
	struct MaterialPass* pass;
	unsigned int passIndex;
	GfxDepthRangeType depthRangeType;
	float depthRangeNear;
	float depthRangeFar;
	unsigned __int64 vertexShaderConstState[64];
	unsigned __int64 pixelShaderConstState[256];
	char padding[16];
	char alphaRef;
	unsigned int refStateBits[2];
	unsigned int activeStateBits[2];
	struct MaterialPixelShader* pixelShader;
	struct MaterialVertexShader* vertexShader;
	GfxViewport viewport;
	GfxRenderTargetId renderTargetId;
	Material* origMaterial;
	MaterialTechniqueType origTechType;
};

struct GfxPointVertex
{
	float xyz[3];
	unsigned char color[4];
};

struct r_global_permanent_t
{
	Material* sortedMaterials[4127];
	int needSortMaterials;
	int materialCount;
	struct GfxImage* whiteImage;
	struct GfxImage* blackImage;
	struct GfxImage* blackImage3D;
	struct GfxImage* blackImageCube;
	struct GfxImage* grayImage;
	struct GfxImage* identityNormalMapImage;
	struct GfxImage* specularityImage;
	struct GfxImage* outdoorImage;
	struct GfxImage* pixelCostColorCodeImage;
	struct GfxLightDef* dlightDef;
	Material* defaultMaterial;
	Material* whiteMaterial;
	Material* additiveMaterial;
	Material* pointMaterial;
	Material* lineMaterial;
	Material* lineMaterialNoDepth;
	Material* clearAlphaStencilMaterial;
	Material* shadowClearMaterial;
	Material* shadowCookieOverlayMaterial;
	Material* shadowCookieBlurMaterial;
	Material* shadowCasterMaterial;
	Material* shadowOverlayMaterial;
	Material* depthPrepassMaterial;
	Material* glareBlindMaterial;
	Material* pixelCostAddDepthAlwaysMaterial;
	Material* pixelCostAddDepthDisableMaterial;
	Material* pixelCostAddDepthEqualMaterial;
	Material* pixelCostAddDepthLessMaterial;
	Material* pixelCostAddDepthWriteMaterial;
	Material* pixelCostAddNoDepthWriteMaterial;
	Material* pixelCostColorCodeMaterial;
	Material* stencilShadowMaterial;
	Material* stencilDisplayMaterial;
	Material* floatZDisplayMaterial;
	Material* colorChannelMixerMaterial;
	Material* frameColorDebugMaterial;
	Material* frameAlphaDebugMaterial;
	struct GfxImage* rawImage;
	struct GfxWorld* world;
	Material* feedbackReplaceMaterial;
	Material* feedbackBlendMaterial;
	Material* feedbackFilmBlendMaterial;
	Material* cinematicMaterial;
	Material* dofDownsampleMaterial;
	Material* dofNearCocMaterial;
	Material* smallBlurMaterial;
	Material* postFxDofMaterial;
	Material* postFxDofColorMaterial;
	Material* postFxColorMaterial;
	Material* postFxMaterial;
	Material* symmetricFilterMaterial[8];
	Material* shellShockBlurredMaterial;
	Material* shellShockFlashedMaterial;
	Material* glowConsistentSetupMaterial;
	Material* glowApplyBloomMaterial;
	int savedScreenTimes[4];
};
struct cplane_s
{
	float normal[3];
	float dist;
	char type;  // for fast side tests: 0,1,2 = axial, 3 = nonaxial
	char signbits;  // signx + (signy<<1) + (signz<<2), used as lookup during collision
	char pad[2];
};
struct cbrushside_t
{
	cplane_s* plane;
	unsigned __int16 materialNum;
	char firstAdjacentSideOffset;
	char edgeCount;
};
struct Bounds
{
	float midPoint[3];
	float halfSize[3];
};

struct cLeaf_t
{
	unsigned __int16 firstCollAabbIndex;
	unsigned __int16 collAabbCount;
	int brushContents;
	int terrainContents;
	Bounds bounds;
	int leafBrushNode;
};

struct cLeafBrushNodeLeaf_t
{
	unsigned __int16* brushes;
};

struct cLeafBrushNodeChildren_t
{
	float dist;
	float range;
	unsigned __int16 childOffset[2];
};

union cLeafBrushNodeData_t
{
	cLeafBrushNodeLeaf_t leaf;
	cLeafBrushNodeChildren_t children;
};

struct cLeafBrushNode_s
{
	char axis;
	__int16 leafBrushCount;
	int contents;
	cLeafBrushNodeData_t data;
};
/* 860 */
struct cmodel_t
{
	float mins[3];
	float maxs[3];
	float radius;
	cLeaf_t leaf;
};

struct cbrush_t
{
	unsigned short numsides;
	unsigned short glassPieceIndex;
	cbrushside_t* sides;
	unsigned char* baseAdjacentSide;
	unsigned short axialMaterialNum[2][3];
	unsigned char firstAdjacentSideOffsets[2][3];
	unsigned char edgeCount[2][3];
};

struct CollisionBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
};

struct CollisionPartition
{
	unsigned char triCount;
	unsigned char borderCount;
	unsigned char firstVertSegment;
	int firstTri;
	CollisionBorder* borders;
};

struct cStaticModel_s
{
	struct XModel* xmodel;
	float origin[3];
	float invScaledAxis[3][3];
	Bounds absBounds;
};
struct ClipMaterial
{
	const char* name;
	int surfaceFlags;
	int contents;
};
struct cNode_t
{
	cplane_s* plane;
	__int16 children[2];
};

union CollisionAabbTreeIndex
{
	int firstChildIndex;
	int partitionIndex;
};

struct CollisionAabbTree
{
	float midPoint[3];
	unsigned __int16 materialIndex;
	unsigned __int16 childCount;
	float halfSize[3];
	CollisionAabbTreeIndex u;
};
struct TriggerModel
{
	int contents;
	unsigned __int16 hullCount;
	unsigned __int16 firstHull;
};

struct TriggerHull
{
	Bounds bounds;
	int contents;
	unsigned __int16 slabCount;
	unsigned __int16 firstSlab;
};

struct TriggerSlab
{
	float dir[3];
	float midPoint;
	float halfSize;
};

struct MapTriggers
{
	unsigned int count;
	TriggerModel* models;
	unsigned int hullCount;
	TriggerHull* hulls;
	unsigned int slabCount;
	TriggerSlab* slabs;
};

struct Stage
{
	const char* name;
	float origin[3];
	unsigned __int16 triggerIndex;
	char sunPrimaryLightIndex;
};

struct __declspec(align(4)) MapEnts
{
	const char* name;
	const char* entityString;
	int numEntityChars;
	MapTriggers trigger;
	Stage* stages;
	char stageCount;
};
struct SModelAabbNode
{
	Bounds bounds;
	unsigned __int16 firstChild;
	unsigned __int16 childCount;
};
#pragma warning(push)
#pragma warning(disable: 4324)
struct __declspec(align(64)) clipMap_t
{
	const char* name;
	int isInUse;
	unsigned int planeCount;
	cplane_s* planes;
	unsigned int numStaticModels;
	cStaticModel_s* staticModelList;
	unsigned int numMaterials;
	ClipMaterial* materials;
	unsigned int numBrushSides;
	cbrushside_t* brushsides;
	unsigned int numBrushEdges;
	unsigned char* brushEdges;
	unsigned int numNodes;
	cNode_t* nodes;
	unsigned int numLeafs;
	cLeaf_t* leafs;
	unsigned int leafbrushNodesCount;
	cLeafBrushNode_s* leafbrushNodes;
	unsigned int numLeafBrushes;
	unsigned __int16* leafbrushes;
	unsigned int numLeafSurfaces;
	unsigned int* leafsurfaces;
	unsigned int vertCount;
	float(*verts)[3];
	unsigned int triCount;
	unsigned __int16* triIndices;
	unsigned char* triEdgeIsWalkable;
	unsigned int borderCount;
	CollisionBorder* borders;
	unsigned int partitionCount;
	CollisionPartition* partitions;
	unsigned int aabbTreeCount;
	CollisionAabbTree* aabbTrees;
	unsigned int numSubModels;
	cmodel_t* cmodels;
	unsigned __int16 numBrushes;
	cbrush_t* brushes;
	Bounds* brushBounds;
	int* brushContents;
	MapEnts* mapEnts;
	unsigned __int16 smodelNodeCount;
	SModelAabbNode* smodelNodes;
	unsigned __int16 dynEntCount[2];
	struct DynEntityDef* dynEntDefList[2];
	struct DynEntityPose* dynEntPoseList[2];
	struct DynEntityClient* dynEntClientList[2];
	struct DynEntityColl* dynEntCollList[2];
	unsigned int checksum;
};
#pragma warning(pop)
struct SimplePlaneIntersection
{
	float xyz[3];
	int planeIndex[3];
};
struct adjacencyWinding_t
{
	int numsides;
	int sides[12];
};
enum $B87C0110D100A68234FECCEB9075A41E : int //channels
{
	CON_CHANNEL_DONT_FILTER = 0,
	CON_CHANNEL_ERROR = 1,
	CON_CHANNEL_GAMENOTIFY = 2,
	CON_CHANNEL_BOLDGAME = 3,
	CON_CHANNEL_SUBTITLE = 4,
	CON_CHANNEL_OBITUARY = 5,
	CON_CHANNEL_LOGFILEONLY = 6,
	CON_CHANNEL_CONSOLEONLY = 7,
	CON_CHANNEL_GFX = 8,
	CON_CHANNEL_SOUND = 9,
	CON_CHANNEL_FILES = 0xA,
	CON_CHANNEL_DEVGUI = 0xB,
	CON_CHANNEL_PROFILE = 0xC,
	CON_CHANNEL_UI = 0xD,
	CON_CHANNEL_CLIENT = 0xE,
	CON_CHANNEL_SERVER = 0xF,
	CON_CHANNEL_SYSTEM = 10,
	CON_CHANNEL_PLAYERWEAP = 11,
	CON_CHANNEL_AI = 12,
	CON_CHANNEL_ANIM = 13,
	CON_CHANNEL_PHYS = 14,
	CON_CHANNEL_FX = 15,
	CON_CHANNEL_LEADERBOARDS = 16,
	CON_CHANNEL_PARSERSCRIPT = 17,
	CON_CHANNEL_SCRIPT = 18,
	CON_BUILTIN_CHANNEL_COUNT = 19
};
struct CmdArgs
{
	int nesting;
	int localClientNum[8];
	int controllerIndex[8];
	int argc[8];
	const char** argv[8];
};
struct GfxMatrix
{
	float m[4][4];
};

struct GfxCodeMatrices
{
	GfxMatrix matrix[56];
};

struct GfxCamera
{
	float origin[3];
	float axis[3][3];
	float subWindowMins[2];
	float subWindowMaxs[2];
	float tanHalfFovX;
	float tanHalfFovY;
	float zNear;
	float depthHackNearClip;
};

struct GfxViewParms
{
	GfxMatrix viewMatrix;
	GfxMatrix projectionMatrix;
	GfxMatrix viewProjectionMatrix;
	GfxMatrix inverseViewProjectionMatrix;
	GfxCamera camera;
};
struct GfxSceneDef
{
	int time;
	float floatTime;
	float viewOffset[3];
	struct GfxImage* sunShadowImage;
	float sunShadowPixelAdjust[4];
};
enum ShadowType
{
	SHADOW_NONE = 0x0,
	SHADOW_MAP = 0x1,
};
struct GfxDepthOfField
{
	float viewModelStart;
	float viewModelEnd;
	float nearStart;
	float nearEnd;
	float farStart;
	float farEnd;
	float nearBlur;
	float farBlur;
};

struct GfxFilm
{
	bool enabled;
	float brightness;
	float contrast;
	float desaturation;
	float desaturationDark;
	bool invert;
	float tintDark[3];
	float tintMedium[3];
	float tintLight[3];
};

struct GfxGlow
{
	bool enabled;
	float bloomCutoff;
	float bloomDesaturation;
	float bloomIntensity;
	float radius;
};

struct GfxLightScale
{
	float diffuseScale;
	float specularScale;
};

struct GfxStageInfo
{
	Stage activeStage;
	bool activeStageValid;
};

struct GfxCompositeFx
{
	GfxFilm film;
	float distortionScale[3];
	float blurRadius;
	float distortionMagnitude;
	float frameRate;
	int lastUpdate;
	int frame;
	int startMSec;
	int currentTime;
	int duration;
	bool enabled;
	bool scriptEnabled;
};
struct GfxVertexBufferState
{
	volatile int used;
	int total;
	struct IDirect3DVertexBuffer9* buffer;
	char* verts;
};

struct GfxMeshData
{
	unsigned int indexCount;
	unsigned int totalIndexCount;
	unsigned __int16* indices;
	GfxVertexBufferState vb;
	unsigned int vertSize;
};

struct GfxQuadMeshData
{
	float x;
	float y;
	float width;
	float height;
	GfxMeshData meshData;
};
struct GfxBspSurfList
{
	unsigned int count;
	const unsigned __int16* stream;
};

struct GfxSModelSurfList
{
	unsigned int surfDataBytes;
	const char* surfData;
	const char* visData;
};

struct GfxDrawSurfList
{
	GfxDrawSurf* array;
	unsigned int count;
};
struct GfxDrawGroupSetupFields
{
	unsigned __int16 materialSortedIndex : 15;
	unsigned __int16 useHeroLighting : 1;
	char sceneLightIndex;
	char surfType;
};

union GfxDrawGroupSetup
{
	GfxDrawGroupSetupFields fields;
	unsigned int packed;
};
struct __declspec(align(4)) GfxGlassSurf
{
	GfxDrawGroupSetup drawGroup;
	char pad;
	char reflectionProbeIndex;
	unsigned __int16 triCount;
	unsigned __int16* indices;
	unsigned __int16 lightingHandle;
};

struct GfxCloudSurfFields
{
	unsigned __int16 materialSortedIndex;
	char cloudDataIndex;
	char surfType;
};

union GfxCloudSurf
{
	GfxCloudSurfFields fields;
	unsigned int packed;
};

struct GfxSparkSurfFields
{
	unsigned __int16 materialSortedIndex;
	unsigned __int16 sparkDataIndex;
};

union GfxSparkSurf
{
	GfxSparkSurfFields fields;
	unsigned int packed;
};

struct GfxSparkSurfList
{
	GfxSparkSurf* surfs;
	unsigned int count;
};

enum GfxCodeSurfListType
{
	GFX_CODE_SURFLIST_INVALID = -1,
	GFX_CODE_SURFLIST_TRANS = 0x0,
	GFX_CODE_SURFLIST_EMISSIVE = 0x1,
	GFX_CODE_SURFLIST_TYPE_COUNT = 0x2,
};


struct GfxViewInfo;

struct GfxDrawListInfo
{
	MaterialTechniqueType baseTechType;
	GfxViewInfo* viewInfo;
	float eyeOffset[3];
	unsigned int sceneLightIndex;
	int cameraView;
	GfxCodeSurfListType codeSurfListType;
};

struct GfxPreTessSurf
{
	GfxDrawGroupSetup drawGroup;
	char lightmapIndex;
	char reflectionProbeIndex;
	unsigned __int16 triCount;
	unsigned int baseIndex;
	unsigned int firstVertex;
	struct IDirect3DVertexBuffer9* vb;
	unsigned int vertexCount;
};

struct GfxPreTessSurfList
{
	GfxPreTessSurf* surfs;
	unsigned int count;
};
struct GfxCodeSurf
{
	GfxDrawGroupSetup drawGroup;
	unsigned int triCount;
	unsigned __int16* indices;
	unsigned __int16 argOffset;
	unsigned __int16 argCount;
};

struct GfxCodeSurfList
{
	GfxCodeSurf* surfs;
	unsigned int count;
};
struct GfxMarkSurfLightingFields
{
	char lmapIndex;
	char reflectionProbeIndex;
	unsigned __int16 modelIndex;
};

union GfxMarkSurfLighting
{
	GfxMarkSurfLightingFields fields;
	unsigned int packed;
};

struct GfxMarkSurf
{
	GfxDrawGroupSetup drawGroup;
	unsigned __int16* indices;
	unsigned __int16 triCount;
	char modelType;
	char pad;
	GfxMarkSurfLighting lighting;
};
struct GfxMarkSurfList
{
	GfxMarkSurf* surfs;
	unsigned int count;
};

struct GfxGlassSurfList
{
	GfxGlassSurf* surfs;
	unsigned int count;
};
struct GfxPlacement
{
	float quat[4];
	float origin[3];
};
struct GfxScaledPlacement
{
	GfxPlacement base;
	float scale;
};

struct GfxParticleCloud
{
	GfxScaledPlacement placement;
	float endpos[3];
	GfxColor color;
	float radius[2];
	unsigned int flags;
	float timeOffset;
};

struct GfxCloudSurfList
{
	GfxParticleCloud* particles;
	GfxCloudSurf* surfs;
	unsigned int count;
};

struct GfxDrawList
{
	GfxBspSurfList bspSurfList;
	GfxPreTessSurfList bspPreTessSurfList;
	GfxSModelSurfList smodelSurfList[4];
	GfxDrawSurfList drawSurfList;
	GfxCodeSurfList codeSurfList;
	GfxMarkSurfList markSurfList;
	GfxGlassSurfList glassSurfList;
	GfxCloudSurfList cloudSurfList;
	GfxSparkSurfList sparkSurfList;
	GfxDrawListInfo info;
};
struct GfxViewInfo
{
	GfxViewParms viewParms;
	GfxViewport sceneViewport;
	GfxViewport displayViewport;
	GfxViewport scissorViewport;
	GfxSceneDef sceneDef;
	ShadowType dynamicShadowType;
	char floatZUsage;
	bool needsDistortionResolve;
	bool viewModelHasDistortion;
	char forceSunShadowsGenerate;
	unsigned int sceneLightCount;
	float blurRadius;
	float frustumPlanes[4][4];
	GfxDepthOfField dof;
	GfxFilm film;
	GfxGlow glow;
	GfxLightScale charPrimaryLightScale;
	GfxStageInfo stageInfo;
	GfxCompositeFx waterSheetingFx;
	const void* displayCmds;
	GfxQuadMeshData* fullSceneViewMesh;
	GfxDrawList drawList[10];
	//__declspec(align(16)) GfxCmdBufInput input;
	GfxRenderTargetId renderTargetId;
	bool useShadows;
	unsigned int sunShadowResolution;
	GfxRenderTargetId sunShadowRenderTargetId;
	unsigned int sunShadowTileCount;
};


struct DpvsPlane{
	float coeffs[4];
};
struct GfxPortal;
struct GfxPortalWritable
{
	char isQueued;
	char isAncestor;
	char recursionDepth;
	char hullPointCount;
	float(*hullPoints)[2];
	GfxPortal* queuedParent;
};

struct GfxCell;
struct GfxPortal
{
	GfxPortalWritable writable;
	DpvsPlane plane;
	GfxCell* cell;
	float(*vertices)[3];
	char vertexCount;
	float hullAxis[2][3];
};
struct GfxAabbTree
{
	float mins[3];
	float maxs[3];
	unsigned __int16 childCount;
	unsigned __int16 surfaceCount;
	unsigned __int16 startSurfIndex;
	unsigned __int16 surfaceCountNoDecal;
	unsigned __int16 startSurfIndexNoDecal;
	unsigned __int16 smodelIndexCount;
	unsigned __int16* smodelIndexes;
	int childrenOffset;
};
struct GfxCell
{
	float mins[3];
	float maxs[3];
	int aabbTreeCount;
	GfxAabbTree* aabbTree;
	int portalCount;
	GfxPortal* portals;
	int cullGroupCount;
	int* cullGroups;
	char reflectionProbeCount;
	char* reflectionProbes;
};

struct PortalHeapNode
{
	GfxPortal* portal;
	float dist;
};
union GfxHullPointsPool
{
	GfxHullPointsPool* nextFree;
	float points[64][2];
};
struct DpvsView
{
	unsigned int renderFxFlagsCull;
	DpvsPlane frustumPlanes[14];
	int frustumPlaneCount;
};
struct DpvsGlob
{
	DpvsPlane viewPlane;
	DpvsPlane fogPlane;
	DpvsPlane* nearPlane;
	DpvsPlane* farPlane;
	GfxMatrix* viewProjMtx;
	GfxMatrix* invViewProjMtx;
	float viewOrg[4];
	int viewOrgIsDir;
	int queuedCount;
	PortalHeapNode* portalQueue;
	GfxHullPointsPool* nextFreeHullPoints;
	float cullDist;
	DpvsPlane childPlanes[2048];
	DpvsView views[3];
	unsigned int cameraCellIndex;
	DpvsPlane* sideFrustumPlanes;
	unsigned int* entVisBits[4];
	struct GfxEntCellRefInfo* entInfo[4];
	unsigned int* cellBits;
	unsigned int cellVisibleBits[32];
};
struct ScreenPlacement
{
	float scaleVirtualToReal[2];
	float scaleVirtualToFull[2];
	float scaleRealToVirtual[2];
	float virtualViewableMin[2];
	float virtualViewableMax[2];
	float realViewportSize[2];
	float realViewableMin[2];
	float realViewableMax[2];
	float subScreenLeft;
};
enum trType_t
{
	TR_STATIONARY = 0x0,
	TR_INTERPOLATE = 0x1,
	TR_LINEAR = 0x2,
	TR_LINEAR_STOP = 0x3,
	TR_SINE = 0x4,
	TR_GRAVITY = 0x5,
	TR_ACCELERATE = 0x6,
	TR_DECELERATE = 0x7,
	TR_PHYSICS = 0x8,
	TR_FIRST_RAGDOLL = 0x9,
	TR_RAGDOLL = 0x9,
	TR_RAGDOLL_GRAVITY = 0xA,
	TR_RAGDOLL_INTERPOLATE = 0xB,
	TR_LAST_RAGDOLL = 0xB,
};
struct trajectory_t
{
	trType_t trType;
	int trTime;
	int trDuration;
	float trBase[3];
	float trDelta[3];
};

union entityState_sndwait
{
	unsigned __int8 scale;
	unsigned __int8 eventParm2;
	unsigned __int8 vehicleCompassType;
};
struct LerpEntityStateTurret
{
	float gunAngles[3];
	int lastBarrelRotChangeTime;
	int lastBarrelRotChangeRate;
	int lastHeatChangeLevel;
	int lastHeatChangeTime;
	bool isBarrelRotating;
	bool isOverheat;
	bool isHeatingUp;
	bool isBeingCarried;
};

union LerpEntityStateTypeUnion
{
	LerpEntityStateTurret turret;
	char pad0[36];
};

struct LerpEntityState
{
	int eFlags;
	trajectory_t pos;
	trajectory_t apos;
	LerpEntityStateTypeUnion u;
};


union $C889CF518587CB2833BFE41358FA5E4A
{
	int brushmodel;
	int item;
	int xmodel;
	int primaryLight;
};
struct EntHandle
{
	unsigned __int16 number;
	unsigned __int16 infoIndex;
};
enum entityType_t
{
	ET_GENERAL = 0x0,
	ET_PLAYER = 0x1,
	ET_PLAYER_CORPSE = 0x2,
	ET_ITEM = 0x3,
	ET_MISSILE = 0x4,
	ET_INVISIBLE = 0x5,
	ET_SCRIPTMOVER = 0x6,
	ET_SOUND_BLEND = 0x7,
	ET_FX = 0x8,
	ET_LOOP_FX = 0x9,
	ET_PRIMARY_LIGHT = 0xA,
	ET_TURRET = 0xB,
	ET_HELICOPTER = 0xC,
	ET_PLANE = 0xD,
	ET_VEHICLE = 0xE,
	ET_VEHICLE_COLLMAP = 0xF,
	ET_VEHICLE_CORPSE = 0x10,
	ET_VEHICLE_SPAWNER = 0x11,
	ET_EVENTS = 0x12,
};
struct entityState_s
{
	unsigned __int8 eType;
	unsigned __int8 surfType;
	unsigned __int8 weapon;
	unsigned __int8 weaponModel;
	entityState_sndwait un1;
	unsigned int eventParm;
	LerpEntityState lerp;
	unsigned __int16 loopSound;
	unsigned __int16 number;
	unsigned __int16 otherEntityNum;
	unsigned __int16 groundEntityNum;
	$C889CF518587CB2833BFE41358FA5E4A index;
	int time2;
	int solid;
	int eventSequence;
	unsigned __int8 events[4];
	unsigned int eventParms[4];
	unsigned __int8 un2[4];
	char pad[4];
};
struct entityShared_t
{
	char isLinked;
	char modelType;
	char svFlags;
	char isInUse;
	Bounds box;
	int contents;
	Bounds absBox;
	float currentOrigin[3];
	float currentAngles[3];
	EntHandle ownerNum;
	int eventTime;
};
union $B62A4B71B7088F8B102AB9DD52F45DCF
{
	char pad[96];
};
struct gentity_waitsnd
{
	unsigned __int16 notifyString;
	unsigned __int16 index;
	unsigned __int8 stoppable;
	int basetime;
	int duration;
};

struct gentity_s
{
	entityState_s s;
	entityShared_t r;
	struct gclient_s* client;
	struct actor_s* actor;
	struct sentient_s* sentient;
	struct scr_vehicle_s* scr_vehicle;
	struct TurretInfo* pTurretInfo;
	unsigned __int8 physicsObject;
	unsigned __int8 takedamage;
	unsigned __int8 active;
	unsigned __int8 nopickup;
	unsigned __int16 model;
	unsigned __int8 handler;
	unsigned __int16 classname;
	unsigned __int16 script_linkName;
	unsigned __int16 script_noteworthy;
	unsigned __int16 target;
	unsigned __int16 targetname;
	unsigned int attachIgnoreCollision;
	int spawnflags;
	int flags;
	int clipmask;
	int processedFrame;
	EntHandle parent;
	int nextthink;
	int health;
	int maxHealth;
	int nexteq;
	int damage;
	int count;
	struct gentity_s* chain;
	struct gentity_s* activator;
	$B62A4B71B7088F8B102AB9DD52F45DCF ___u32;
	EntHandle missileTargetEnt;
	unsigned __int16 lookAtText0;
	unsigned __int16 lookAtText1;
	gentity_waitsnd snd_wait;
	struct tagInfo_s* tagInfo;
	struct gentity_s* tagChildren;
	struct animscripted_s* scripted;
	unsigned __int16 attachModelNames[31];
	unsigned __int16 attachTagNames[31];
	unsigned __int16 disconnectedLinks;
	struct gentity_s* nextFree;
};
static_assert(sizeof(gentity_s) == 624);

struct SpawnVar
{
	bool spawnVarsValid;
	int numSpawnVars;
	char* spawnVars[64][2];
	int numSpawnVarChars;
	char spawnVarChars[2048];
};
enum loading_t : __int32
{
	LOADING_DONE = 0x0,
	LOADING_LEVEL = 0x1,
	LOADING_SAVEGAME = 0x2,
};
struct cached_tag_mat_t
{
	int time;
	int entnum;
	unsigned __int16 name;
	float tagMat[4][3];
};
struct trigger_info_t
{
	unsigned __int16 entnum;
	unsigned __int16 otherEntnum;
	int useCount;
	int otherUseCount;
};
struct actor_prone_info_s
{
	bool bCorpseOrientation;
	bool orientPitch;
	bool prone;
	int iProneTime;
	int iProneTrans;
	float fBodyHeight;
	int ___u6;
	int ___u7;
};
struct com_parse_mark_t
{
	int lines;
	const char* text;
	int ungetToken;
	int backup_lines;
	const char* backup_text;
};
struct level_locals_t
{
	struct gclient_s* clients;
	struct gentity_s* gentities;
	int num_entities;
	struct gentity_s* firstFreeEnt;
	struct gentity_s* lastFreeEnt;
	struct sentient_s* sentients;
	struct actor_s* actors;
	struct scr_vehicle_s* vehicles;
	struct TurretInfo* turrets;
	int initializing;
	int clientIsSpawning;
	int maxclients;
	int framenum;
	int time;
	int previousTime;
	int snapTime;
	int newSession;
	int actorCorpseCount;
	char pad1[8];
	SpawnVar spawnVar;
	int reloadDelayTime;
	int absoluteReloadDelayTime;
	EntHandle droppedWeaponCue[32];
	int changelevel;
	int bMissionSuccess;
	int bMissionFailed;
	int exitTime;
	int savepersist;
	char cinematic[64];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	unsigned int grenadeHintCount;
	int remapCount;
	int iSearchFrame;
	loading_t loading;
	int actorPredictDepth;
	int bDrawCompassFriendlies;
	int bPlayerIgnoreRadiusDamage;
	int bPlayerIgnoreRadiusDamageLatched;
	unsigned __int8 triggerIndex;
	int currentEntityThink;
	int currentIndex;
	bool checkAnimChange;
	int bRegisterItems;
	int framePos;
	bool demoplaying;
	int mpviewer;
	cached_tag_mat_t cachedTagMat;
	cached_tag_mat_t cachedEntTargetTagMat;
	unsigned __int16 soundAliasFirst;
	unsigned __int16 soundAliasLast;
	trigger_info_t pendingTriggerList[256];
	trigger_info_t currentTriggerList[256];
	int pendingTriggerListSize;
	int currentTriggerListSize;
	unsigned __int8 entTriggerIndex[2176];
	unsigned __int8 specialIndex[2176];
	actor_prone_info_s cgData_actorProneInfo[48];
	unsigned __int8 cgData_actorOnCompass[32];
	unsigned __int8 cgData_actorTeam[32];
	unsigned __int16 modelMap[512];
	float priorityNodeBias;
	void* openScriptIOFileHandles[1];
	char* openScriptIOFileBuffers[1];
	com_parse_mark_t currentScriptIOLineMark[1];
	int scriptPrintChannel;
	float compassMapUpperLeft[2];
	float compassMapWorldSize[2];
	float compassNorth[2];
	float mapSunColor[3];
	float mapSunDirection[3];
};

enum fieldtype_t : __int32
{
	F_INT = 0x0,
	F_SHORT = 0x1,
	F_BYTE = 0x2,
	F_FLOAT = 0x3,
	F_STRING = 0x4,
	F_VECTOR = 0x5,
	F_ENTITY = 0x6,
	F_ENTHANDLE = 0x7,
	F_ACTOR = 0x8,
	F_SENTIENT = 0x9,
	F_SENTIENTHANDLE = 0xA,
	F_CLIENT = 0xB,
	F_PATHNODE = 0xC,
	F_VECTORHACK = 0xD,
	F_MODEL = 0xE,
	F_ACTORGROUP = 0xF,
};

struct ent_field_t
{
	const char* name;
	int ofs;
	fieldtype_t type;
	void (*setter)(gentity_s*, int);
	void (*getter)(gentity_s*, int);
};

struct RefdefView
{
	float tanHalfFovX;
	float tanHalfFovY;
	float org[3];
	float axis[3][3];
	float zNear;
};
struct GfxLight
{
	char type;
	char canUseShadowMap;
	char unused[2];
	float color[3];
	float dir[3];
	float origin[3];
	float radius;
	float cosHalfFovOuter;
	float cosHalfFovInner;
	int exponent;
	unsigned int spotShadowIndex;
	struct GfxLightDef* def;
};
struct refdef_t
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	RefdefView view;
	float viewOffset[3];
	int time;
	float blurRadius;
	GfxDepthOfField dof;
	GfxFilm film;
	GfxGlow glow;
	GfxLightScale charPrimaryLightScale;
	GfxCompositeFx waterSheetingFx;
	GfxLight primaryLights[248];
	GfxViewport scissorViewport;
	bool useScissorViewport;
	bool viewModelHasDistortion;
	char forceSunShadowsGenerate;
	bool halfResParticles;
	bool playerTeleported;
	int localClientNum;
};

