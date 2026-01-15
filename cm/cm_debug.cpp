#include "cm_debug.hpp"
#include "cm/cm_brush.hpp"
#include "cmd/cmd.hpp"
#include "dvar/dvar.hpp"
#include "g/g_entity.hpp"

#include <print>

volatile int CGDebugData::tessVerts{};
volatile int CGDebugData::tessIndices{};
GfxPointVertex CGDebugData::g_debugPolyVerts[2725];
CGDebugData::TriggerData CGDebugData::currentTrigger{};

void CM_LoadDvars()
{
	Cmd_AddCommand("cm_showCollisionFilter", CM_ShowCollisionFilter);
	Cmd_AddCommand("cm_showEntities", Cmd_ShowEntities_f);

	//last index has to be a nullptr because Dvar_RegisterEnum checks the count in a strange way -> while ( valueList[i++] )
	static const char* showCollisionNames[] = { "Disabled", "Brushes", "Terrain", "Both", nullptr };
	static const char* poly_types[] = { "Edges", "Polygons", "Both", nullptr };
	static const char* entity_info[] = { "Disabled", "Enabled", "Verbose", nullptr };

	Dvar_RegisterEnum("cm_showCollision", showCollisionNames, 0, dvar_flags::saved,
		"Select whether to show the terrain, brush or all collision surface groups");

	Dvar_RegisterEnum("cm_showCollisionPolyType", poly_types, 2, dvar_flags::saved,
		"Select whether to display the collision surfaces as wireframe or poly interiors");

	Dvar_RegisterBool("cm_debug", dvar_flags::saved, false, "show debug information");
	//Dvar_RegisterBool("cm_entityConnections", dvar_flags::saved, true, "Draw lines between connected entities");
	Dvar_RegisterEnum("cm_entityInfo", entity_info, 3, dvar_flags::saved, "Display entity information");


	Dvar_RegisterFloat("cm_showCollisionDist", 2000.f, 0.f, FLT_MAX, dvar_flags::saved, "Maximum distance to show collision surfaces");
	Dvar_RegisterFloat("cm_showCollisionPolyAlpha", 0.1f, 0.f, 1.f, dvar_flags::saved, "Collision transparency");

	Dvar_RegisterBool("cm_ignoreNonColliding", dvar_flags::saved, true, "Don't display surfaces which don't have collisions");
	Dvar_RegisterBool("cm_depthTest", dvar_flags::saved, true, "Select whether to use depth test in surface display");

	Dvar_RegisterBool("cm_onlyBounces", dvar_flags::none, false, "Only display surfaces which can be bounced");
	Dvar_RegisterBool("cm_onlyElevators", dvar_flags::none, false, "Only display surfaces which can be elevated");

	Dvar_RegisterBool("cm_triggerDisable", dvar_flags::none, false, "Triggers will not have any effect");
	Dvar_RegisterBool("cm_triggerShowInfo", dvar_flags::none, false, "Show information about the trigger we're currently activating");

	Dvar_RegisterBool("pm_coordinates", dvar_flags::saved, false, "show player coordinates");

}