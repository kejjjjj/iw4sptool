#pragma once

void G_SpawnEntitiesFromString();
void G_ResetEntityParsePoint();

[[nodiscard]] int G_ParseSpawnVars(struct SpawnVar* var);
[[nodiscard]] struct SpawnVar* G_GetGentitySpawnVars(const struct gentity_s* gent);


void G_ParseEntityFields();